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
				dwEntrySize = dwTrySize;//����� ������ ������
			}
			break;
			//��� �������
		case ERROR_NO_MORE_ITEMS:
			bDone = true;
			bResult = true;
			break;
			//��� ������
		case ERROR_SUCCESS:
			//������ �� �������
			//if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
			DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

			//������� ��������� �������
			dwTrySize = dwEntrySize;
			if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
				dwError = ERROR_SUCCESS;
			else
			{
				dwError = GetLastError();
				dwEntrySize = dwTrySize;//����� ������ ������
			}
			break;
			//�� ���������
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
	// �������� ��� ������ � ������ ���������� �����
	ofs.open("auth.conf", std::ofstream::out | std::ofstream::trunc);
	ofs << m_login;
	ofs << "\n";
	ofs << m_password;
	ofs.close();
}

//������� ��������� ������ �� ������ � � �������� ������������
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
	// ����� ������������ ������ ���
	ClearCache();
	std::string html;
	//��� ����������� � �� ���������� ������� ������� ������ �������� � ������� �������� ip_h � lg_h
	//��� �������� ����� ��� ��������� POST ������� ��� �����������
	m_network->CloseConnection();
	m_network->OpenConnection("vk.com");
	if (!m_network->Get("/"))
	{
		if (reconnect == 0)
			return false;

		// ����� 5 ��� ������� ������ ����������������
		Sleep(5000);
		return AuthorizationVK(reconnect - 1);
	}
		

	m_network->Send();
	html = m_network->GetData();

	//��������� ip_h � lg_h
	std::string ip_h = m_network->getParam(html, "name=\"ip_h\"", "\"", "\"");
	std::string lg_h = m_network->getParam(html, "name=\"lg_h\"", "\"", "\"");

	//�������� ���������� �� ��� ��������
	if (!ip_h.empty() && !lg_h.empty())
	{
		//������ ������ � ������ �������� ���� �������, ���� �� ������� �� ���� ����� � ���� �� ���������� ������ �������
		if (!loginPassFromFile())
		{
			std::cout << "����������� � ��� ���� \n";
			std::cout << "������� �����: ";
			std::cin >> m_login;
			std::cout << "������� ������: ";
			std::cin >> m_password;
		}
		//��������� POST ������ �� �����������
		std::cout << "��������� ������ �� �����������" << std::endl;
		m_network->CloseConnection();
		m_network->OpenConnection("login.vk.com");
		m_network->Post("/?act=login");
		m_network->Send("act=login&role=al_frame&expire=&captcha_sid=&captcha_key=&_origin=https://vk.com&ip_h=" + ip_h + "&lg_h=" + lg_h + "&email=" + m_login + "&pass=" + m_password);
		//�������� ��� ��� ������� ��������, � ��������, ���� �� �������� ���� ����� logout ������ �� �����.
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

		//��� logout
		if (html.find("logout") != -1)
		{
			//���� ����� �� ��������� ��������� �� ��������
			std::string profileData = m_network->getParam(html, "\"profileData\"", "[", "]");
			//�������� ������ �� ������
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_userid = temp[0];
			m_username = temp[3];
			// ���� ���� ��������, �� ��������� ����� � ������ � ����
			loginPassToFile();
			return true;
		}
		else {
			//���� �� ����� �� ������� ���������������� ��� ���
			if (reconnect == 0)
				return false;

			Sleep(5000);
			return AuthorizationVK(reconnect - 1);
		}
	}
	else {
		//���� �������� ip_h � lg_h �� ���������� �� �������� ���� �� ������ logout ���� ���� �� ��� ���� �����������
		//����� ����������� �� �������
		if (html.find("logout") != -1)
		{
			//���� ����� �� ��������� ��������� �� ��������
			std::string profileData = m_network->getParam(html, "\"profileData\"", "[", "]");
			//�������� ������ �� ������
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_userid = temp[0];
			m_username = temp[3];
			// ���� ���� ��������, �� ��������� ����� � ������ � ����
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
