#include "Application.h"

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

Application::Application()
	: m_authorization(false),m_socialnetwork(NULL)
{
	//Очищаю кеш на запуске приложения
	DelCache();
	//Очищаю данные по игре
	m_datagame.ClearData();
	//Пока что делаю логин и пароль константными
	m_login = "nazin_konstantin@mail.ru";
	m_password = "rjycnfynby197955";

	m_bot = new Bot(&m_datagame);
}

Application::~Application()
{
	if (m_socialnetwork)
		delete m_socialnetwork;

	delete m_bot;
}

void Application::Loop()
{
	std::string s_meny = "";
	bool b_exit = false;
	bool paint = true;
	wchar_t sim = 0;

	while (!b_exit)
	{
		sim = 0;
		if (paint)
		{
			system("cls");

			s_meny = m_header;

			s_meny += "\nМеню\n";
			if (!m_authorization)
				s_meny += "1 - Авторизоваться Вконтакте\n";
			else
			{
				s_meny += "1 - Выйти из Вконтакте\n";
				s_meny += "2 - Запустить Бота\n";
				s_meny += "3 - Настройки Бота\n";
			}

			s_meny += "ESC Выход\n";

			std::cout << s_meny;

			paint = false;
		}
		
		if (_kbhit())
		{
			sim = _getwch();

			switch (sim)
			{
			case L'1':
				if (m_authorization)
				{
					//выходим из Вконтакте
					//очищаю куки и данные об игре
					DelCache();
					m_datagame.ClearData();
					delete m_socialnetwork;//удаляем соединени, дескриптор класса закроет существующие соединения
					m_socialnetwork = NULL;
					m_authorization = false;
					m_header = "";
				}
				else
				{
					//Авторизуемся Вконтакте
					//Проверяем, если авторизовались, то ставим флаг что прошли авторизацию
					m_authorization = AuthorizationVK();
				}
				paint = true;
				break;
			case L'2':
				//запускаем бота
				m_bot->Loop();
				paint = true;
				break;
			case 27://кнопка ESC
				b_exit = true;
				break;
			}
		}
	}
}

BOOL Application::DelCache()
{
	BOOL bResult = FALSE;
	BOOL bDone = FALSE;
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
			BOOL bSuccess;
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
			bDone = TRUE;
			bResult = TRUE;
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
			bDone = TRUE;
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

bool Application::AuthorizationVK()
{
	bool bResult = false;
	//Для авторизации в вк необходимо сначала открыть просто страницу и считать значения ip_h и lg_h
	//эти значения нужны для генерации POST запроса для авторизации
	m_socialnetwork = new HttpReader("vk.com", true);
	if (m_socialnetwork->Get("/"))
	{
		m_socialnetwork->Send();
		m_html = m_socialnetwork->GetData();
	}
	//вытягиваю ip_h и lg_h
	std::string ip_h = m_socialnetwork->getParam(m_html, "name=\"ip_h\"", "\"", "\"");
	std::string lg_h = m_socialnetwork->getParam(m_html, "name=\"lg_h\"", "\"", "\"");
	//Проверяю вытянулись ли эти значения
	if (!ip_h.empty() && !lg_h.empty())
	{
		//Запрос логина и пароля
		/*std::cout << "Введите логин: ";
		std::cin >> m_login;
		std::cout << "Введите пароль: ";
		std::cin >> m_password;*/
		//отправляю POST запрос на авторизацию
		std::cout << "Отправляю запрос на авторизацию" << std::endl;
		m_socialnetwork->CloseConnection();
		m_socialnetwork->OpenConnection("login.vk.com");
		m_socialnetwork->Post("/?act=login");
		m_socialnetwork->Send("act=login&role=al_frame&expire=&captcha_sid=&captcha_key=&_origin=https://vk.com&ip_h=" + ip_h + "&lg_h=" + lg_h + "&email=" + m_login + "&pass=" + m_password);
		//считываю еще раз главную страницу, и проверяю, если на странице есть слово logout значит мы зашли.
		m_socialnetwork->CloseConnection();
		m_socialnetwork->OpenConnection("vk.com");
		if (m_socialnetwork->Get("/"))
		{
			m_socialnetwork->Send();
			m_html = m_socialnetwork->GetData();
		}
		//ищу logout
		if (m_html.find("logout") != -1)
		{
			//Если зашли то считываем параметры со страницы
			std::string profileData = m_socialnetwork->getParam(m_html, "\"profileData\"", "[", "]");
			//разбиваю строку на массив
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_datagame.setUserId(temp[0]);
			m_datagame.set_username(temp[3]);
			m_header = temp[3];
			bResult = true;
		} else {
			//если не зашли то выдаем сообщение в шапке
			m_header = "Авторизация не удалась";
			bResult = false;
		}
	} else {
		//если значения ip_h и lg_h не вытянулись то проверяю есть ли кнопка logout если есть то уже есть авторизация
		//иначе авторизация не удалась
		if (m_html.find("logout") != -1)
		{
			//Если зашли то считываем параметры со страницы
			std::string profileData = m_socialnetwork->getParam(m_html, "\"profileData\"", "[", "]");
			//разбиваю строку на массив
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_datagame.setUserId(temp[0]);
			m_datagame.set_username(temp[3]);
			m_header = temp[3];
			bResult = true;
		} else {
			m_header = "Авторизация не удалась";
			bResult = false;
		}
	}

	return bResult;
}