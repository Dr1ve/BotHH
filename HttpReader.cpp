#include "HttpReader.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

#pragma comment (lib, "Wininet.lib")

HttpReader::HttpReader(const std::string &ServerName, bool bUseSSL)
	: m_hInternet(NULL), 
	m_hConnection(NULL), 
	m_hRequest(NULL), 
	m_bUseSSL(bUseSSL), 
	m_ServerName(ServerName) 
{
}

HttpReader::~HttpReader()
{
	CloseInternet();
}

bool HttpReader::OpenInternet(std::string Agent)
{
	if (m_hInternet == NULL)
		m_hInternet = InternetOpen(Agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	return CheckError(m_hInternet != NULL);
}

void HttpReader::CloseInternet()
{
	CloseConnection();

	if (m_hInternet)
		InternetCloseHandle(m_hInternet);
	m_hInternet = NULL;
}

bool HttpReader::OpenConnection(std::string ServerName)
{
	if (OpenInternet() && m_hConnection == NULL)
	{
		if (ServerName.size()>0)
			m_ServerName = ServerName;

		m_hConnection=InternetConnect(m_hInternet,
			m_ServerName.c_str(),
			m_bUseSSL ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
			NULL,NULL,
			INTERNET_SERVICE_HTTP, 0, 1u);
	}

	return CheckError(m_hConnection != NULL);
}

bool HttpReader::CheckError(bool bTest)
{
	if (bTest == false)
		m_dwLastError = GetLastError();

	return bTest;
}

bool HttpReader::SendRequest(std::string Verb, std::string Action, /*std::string Data,*/ std::string Referer)
{
	if (OpenConnection())
	{
		CloseRequest();

		LPCTSTR AcceptTypes[] = { TEXT("*/*"),NULL };
		DWORD dwOpenRequestFlag = 0;
		if (m_bUseSSL)
			dwOpenRequestFlag = INTERNET_FLAG_IGNORE_CERT_CN_INVALID |//не делать полную проверку сертификата
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |//игнонирование недействительные даты в сертификатах
			INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |//разрешаю переадресацию из https на http
			INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |//разрешаю переадресацию из http на https
			INTERNET_FLAG_KEEP_CONNECTION |//использую keep-alive если доступно
			INTERNET_FLAG_SECURE;//Использую SSL транзакции
		else
			dwOpenRequestFlag = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |//разрешаю переадресацию из http на https
			INTERNET_FLAG_KEEP_CONNECTION;//использую keep-alive если доступно

		m_hRequest = HttpOpenRequest(m_hConnection, Verb.c_str(), Action.c_str(), NULL, Referer.c_str(), AcceptTypes, dwOpenRequestFlag, 1);

		/*if (m_hRequest != NULL)
		{
			if (HttpSendRequest(
				m_hRequest,
				NULL,
				0,
				(LPVOID)Data.c_str(),
				Data.size()) == FALSE)
			{
				CheckError(false);
				CloseRequest();
				return false;
			}
		}*/
	}

	return CheckError(m_hRequest != NULL);
}

void HttpReader::CloseConnection()
{
	CloseRequest();

	if (m_hConnection)
		InternetCloseHandle(m_hConnection);
	m_hConnection = NULL;
}

bool HttpReader::Get(std::string Action, std::string Referer)
{
	return SendRequest(TEXT("GET"), Action, /*"",*/ Referer);
}

bool HttpReader::Post(std::string Action, /*std::string Data,*/ std::string Referer)
{
	return SendRequest(TEXT("POST"), Action, /*Data,*/ Referer);
}

void HttpReader::Send(std::string Data)
{
	if (m_hRequest != NULL)
	{
		if (HttpSendRequest(
			m_hRequest,
			NULL,
			0,
			(LPVOID)Data.c_str(),
			Data.size()) == FALSE)
		{
			CheckError(false);
			CloseRequest();
		}
	}
}

void HttpReader::CloseRequest()
{
	if (m_hRequest)
		InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;
}

std::string HttpReader::GetData()
{
	BOOL bRead;
	char cBuffer[1024];
	DWORD dwBytesRead;
	std::string source = "";

	do {
		bRead = InternetReadFile(m_hRequest, (LPSTR)cBuffer, sizeof(cBuffer) - 1, &dwBytesRead);
		// Не знаю как так получилось но вылетела ошибка выхода за предел массива
		if (dwBytesRead < 1024)
		{
			cBuffer[dwBytesRead] = 0;
			source += cBuffer;
		}
	} while (bRead && (dwBytesRead != 0));

	return source;
}

std::string HttpReader::getParam(const std::string &html, const std::string &name, const std::string &leftsymbol, std::string rightsymbol)
{
	/*проверка на существование параметра*/
	if (html.find(name) == -1)
	{
		if (leftsymbol == ":")
			return "0";
		else
			return "";
	}
	if (name.find("\"date\"") != -1)
	{
		rightsymbol = html.find(".") == -1 ? "," : rightsymbol;
	}
	int first = html.find(name) + name.length();
	first = html.find(leftsymbol, first) + leftsymbol.length();
	int last = html.find(rightsymbol, first) - first;
	return html.substr(first, last);
}

std::string HttpReader::getVal(const std::string &json, const std::string &path, const std::string &def)
{
	std::stringstream ss(json);
	pt::ptree root;
	pt::read_json(ss, root);
	return root.get<std::string>(path, def);
}

bool HttpReader::AddRequestHeader(std::string header)
{
	if (m_hRequest != NULL)
		return HttpAddRequestHeaders(m_hRequest, header.c_str(), header.length(), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	else
		return false;
}
