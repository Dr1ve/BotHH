#include "Authorization.h"

#include <Windows.h>
#include <WinInet.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "HttpReader.h"

#pragma comment (lib, "Wininet.lib")

Authorization::Authorization(HttpReader * network)
	: m_network(network)
{
	
}

bool Authorization::ClearCache() const
{
	bool bResult = false;
	bool bDone = false;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;

	DWORD dwTrySize, dwEntrySize = 4096;
	HANDLE hCacheDir = NULL;
	DWORD dwError = ERROR_INSUFFICIENT_BUFFER;

	do
	{
		switch (dwError)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			delete[] lpCacheEntry;
			lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
			lpCacheEntry->dwStructSize = dwEntrySize;
			dwTrySize = dwEntrySize;
			bool bSuccess;
			if (hCacheDir == NULL)
				bSuccess = (hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwTrySize)) != NULL;
			else
				bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);

			if (bSuccess)
				dwError = ERROR_SUCCESS;
			else
			{
				dwError = GetLastError();
				dwEntrySize = dwTrySize;//новый размер буфера
			}
			break;
			//все удалено
		case ERROR_NO_MORE_ITEMS:
			bDone = true;
			bResult = true;
			break;
			//Нет ошибок
		case ERROR_SUCCESS:
			//кукисы не удалять
			//if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
			DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

			//удаляем следующий элемент
			dwTrySize = dwEntrySize;
			if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
				dwError = ERROR_SUCCESS;
			else
			{
				dwError = GetLastError();
				dwEntrySize = dwTrySize;//новый размер буфера
			}
			break;
			//по умолчанию
		default:
			bDone = true;
			break;
		}

		if (bDone)
		{
			delete[] lpCacheEntry;
			if (hCacheDir)
				FindCloseUrlCache(hCacheDir);
		}
	} while (!bDone);

	return bResult;
}

bool Authorization::loginPassFromFile()
{
	std::ifstream fs;
	fs.open("auth.conf", std::ifstream::in);

	if (!fs.is_open())
		return false;

	fs >> m_login;
	fs >> m_password;

	fs.close();
	return (!m_login.empty() && !m_password.empty());
}

void Authorization::loginPassToFile() const
{
	std::ofstream ofs;
	// Открываю для записи и удаляю содержимое файла
	ofs.open("auth.conf", std::ofstream::out | std::ofstream::trunc);
	ofs << m_login;
	ofs << "\n";
	ofs << m_password;
	ofs.close();
}

//функция разбивает строку на массив с с заданным разделителем
std::vector<std::string> ExtractWord(std::string str, char delimiter)
{
	std::vector<std::string> v;
	int next = 0;
	int prev = 0;

	do {
		next = str.find(delimiter, prev);
		std::string tmp = str.substr(prev, next - prev);
		v.push_back(tmp);
		prev = next + 1;
	} while (next != std::string::npos);

	return v;
}

bool Authorization::AuthorizationVK(int reconnect)
{
	// Перед авторизацией очищаю кэш
	ClearCache();
	std::string html;
	//Для авторизации в вк необходимо сначала открыть просто страницу и считать значения ip_h и lg_h
	//эти значения нужны для генерации POST запроса для авторизации
	m_network->CloseConnection();
	m_network->OpenConnection("vk.com");
	if (!m_network->Get("/"))
	{
		if (reconnect == 0)
			return false;

		// Через 5 сек пытаюсь заново переподключиться
		Sleep(5000);
		return AuthorizationVK(reconnect - 1);
	}
		

	m_network->Send();
	html = m_network->GetData();

	//вытягиваю ip_h и lg_h
	std::string ip_h = m_network->getParam(html, "name=\"ip_h\"", "\"", "\"");
	std::string lg_h = m_network->getParam(html, "name=\"lg_h\"", "\"", "\"");

	//Проверяю вытянулись ли эти значения
	if (!ip_h.empty() && !lg_h.empty())
	{
		//Запрос логина и пароля считываю файл конфига, если не получил от туда логин и пасс то запрашиваю ввести вручную
		if (!loginPassFromFile())
		{
			std::cout << "Авторизация в соц сети \n";
			std::cout << "Введите логин: ";
			std::cin >> m_login;
			std::cout << "Введите пароль: ";
			std::cin >> m_password;
		}
		//отправляю POST запрос на авторизацию
		std::cout << "Отправляю запрос на авторизацию" << std::endl;
		m_network->CloseConnection();
		m_network->OpenConnection("login.vk.com");
		m_network->Post("/?act=login");
		m_network->Send("act=login&role=al_frame&expire=&captcha_sid=&captcha_key=&_origin=https://vk.com&ip_h=" + ip_h + "&lg_h=" + lg_h + "&email=" + m_login + "&pass=" + m_password);
		//считываю еще раз главную страницу, и проверяю, если на странице есть слово logout значит мы зашли.
		m_network->CloseConnection();
		m_network->OpenConnection("vk.com");
		if (!m_network->Get("/"))
		{
			if (reconnect == 0)
				return false;

			Sleep(5000);
			return AuthorizationVK(reconnect - 1);
		}			

		m_network->Send();
		html = m_network->GetData();

		//ищу logout
		if (html.find("logout") != -1)
		{
			//Если зашли то считываем параметры со страницы
			std::string profileData = m_network->getParam(html, "\"profileData\"", "[", "]");
			//разбиваю строку на массив
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_userid = temp[0];
			m_username = temp[3];
			// Если вход выполнен, то записываю логин и пароль в файл
			loginPassToFile();
			return true;
		}
		else {
			//если не зашли то пытаюсь переподключиться еще раз
			if (reconnect == 0)
				return false;

			Sleep(5000);
			return AuthorizationVK(reconnect - 1);
		}
	}
	else {
		//если значения ip_h и lg_h не вытянулись то проверяю есть ли кнопка logout если есть то уже есть авторизация
		//иначе авторизация не удалась
		if (html.find("logout") != -1)
		{
			//Если зашли то считываем параметры со страницы
			std::string profileData = m_network->getParam(html, "\"profileData\"", "[", "]");
			//разбиваю строку на массив
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_userid = temp[0];
			m_username = temp[3];
			// Если вход выполнен, то записываю логин и пароль в файл
			loginPassToFile();
			return true;
		}
		else {
			if (reconnect == 0)
				return false;

			Sleep(5000);
			return AuthorizationVK(reconnect - 1);
		}
	}
}

std::string Authorization::getUserId()
{
	return m_userid;
}

std::string Authorization::getUserName()
{
	return m_username;
}
