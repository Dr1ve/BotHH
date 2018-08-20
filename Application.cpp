#include "Application.h"

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

Application::Application()
	: m_authorization(false),m_socialnetwork(NULL)
{
	//������ ��� �� ������� ����������
	DelCache();
	//������ ������ �� ����
	m_datagame.ClearData();
	//���� ��� ����� ����� � ������ ������������
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

			s_meny += "\n����\n";
			if (!m_authorization)
				s_meny += "1 - �������������� ���������\n";
			else
			{
				s_meny += "1 - ����� �� ���������\n";
				s_meny += "2 - ��������� ����\n";
				s_meny += "3 - ��������� ����\n";
			}

			s_meny += "ESC �����\n";

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
					//������� �� ���������
					//������ ���� � ������ �� ����
					DelCache();
					m_datagame.ClearData();
					delete m_socialnetwork;//������� ���������, ���������� ������ ������� ������������ ����������
					m_socialnetwork = NULL;
					m_authorization = false;
					m_header = "";
				}
				else
				{
					//������������ ���������
					//���������, ���� ��������������, �� ������ ���� ��� ������ �����������
					m_authorization = AuthorizationVK();
				}
				paint = true;
				break;
			case L'2':
				//��������� ����
				m_bot->Loop();
				paint = true;
				break;
			case 27://������ ESC
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
				dwEntrySize = dwTrySize;//����� ������ ������
			}
			break;
			//��� �������
		case ERROR_NO_MORE_ITEMS:
			bDone = TRUE;
			bResult = TRUE;
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
	//��� ����������� � �� ���������� ������� ������� ������ �������� � ������� �������� ip_h � lg_h
	//��� �������� ����� ��� ��������� POST ������� ��� �����������
	m_socialnetwork = new HttpReader("vk.com", true);
	if (m_socialnetwork->Get("/"))
	{
		m_socialnetwork->Send();
		m_html = m_socialnetwork->GetData();
	}
	//��������� ip_h � lg_h
	std::string ip_h = m_socialnetwork->getParam(m_html, "name=\"ip_h\"", "\"", "\"");
	std::string lg_h = m_socialnetwork->getParam(m_html, "name=\"lg_h\"", "\"", "\"");
	//�������� ���������� �� ��� ��������
	if (!ip_h.empty() && !lg_h.empty())
	{
		//������ ������ � ������
		/*std::cout << "������� �����: ";
		std::cin >> m_login;
		std::cout << "������� ������: ";
		std::cin >> m_password;*/
		//��������� POST ������ �� �����������
		std::cout << "��������� ������ �� �����������" << std::endl;
		m_socialnetwork->CloseConnection();
		m_socialnetwork->OpenConnection("login.vk.com");
		m_socialnetwork->Post("/?act=login");
		m_socialnetwork->Send("act=login&role=al_frame&expire=&captcha_sid=&captcha_key=&_origin=https://vk.com&ip_h=" + ip_h + "&lg_h=" + lg_h + "&email=" + m_login + "&pass=" + m_password);
		//�������� ��� ��� ������� ��������, � ��������, ���� �� �������� ���� ����� logout ������ �� �����.
		m_socialnetwork->CloseConnection();
		m_socialnetwork->OpenConnection("vk.com");
		if (m_socialnetwork->Get("/"))
		{
			m_socialnetwork->Send();
			m_html = m_socialnetwork->GetData();
		}
		//��� logout
		if (m_html.find("logout") != -1)
		{
			//���� ����� �� ��������� ��������� �� ��������
			std::string profileData = m_socialnetwork->getParam(m_html, "\"profileData\"", "[", "]");
			//�������� ������ �� ������
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_datagame.setUserId(temp[0]);
			m_datagame.set_username(temp[3]);
			m_header = temp[3];
			bResult = true;
		} else {
			//���� �� ����� �� ������ ��������� � �����
			m_header = "����������� �� �������";
			bResult = false;
		}
	} else {
		//���� �������� ip_h � lg_h �� ���������� �� �������� ���� �� ������ logout ���� ���� �� ��� ���� �����������
		//����� ����������� �� �������
		if (m_html.find("logout") != -1)
		{
			//���� ����� �� ��������� ��������� �� ��������
			std::string profileData = m_socialnetwork->getParam(m_html, "\"profileData\"", "[", "]");
			//�������� ������ �� ������
			std::vector<std::string> temp = ExtractWord(profileData, ',');
			m_datagame.setUserId(temp[0]);
			m_datagame.set_username(temp[3]);
			m_header = temp[3];
			bResult = true;
		} else {
			m_header = "����������� �� �������";
			bResult = false;
		}
	}

	return bResult;
}