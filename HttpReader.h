#ifndef HTTPREADER_H
#define HTTPREADER_H

#include "includes.h"

class HttpReader
{
public:
	HttpReader(std::string ServerName = NULL, bool bUseSSL = false);
	~HttpReader();

	bool OpenInternet(std::string Agent = TEXT("Mozilla / 5.0 (Windows NT 6.1; WOW64; Trident / 7.0; rv:11.0) like Gecko"));
	void CloseInternet();
	bool OpenConnection(std::string ServerName = "");
	void CloseConnection();

	bool Get(std::string Action, std::string Referer = "");
	bool Post(std::string Action, /*std::string Data,*/ std::string Referer = "");
	void Send(std::string Data = "");
	void CloseRequest();

	std::string GetData();

	static std::string getParam(std::string html, std::string name, std::string leftsymbol, std::string rightsymbol);
	static std::string getVal(std::string json, std::string path, std::string def = "");

	bool AddRequestHeader(std::string header);

private:
	bool CheckError(bool bTest);
	bool SendRequest(std::string Verb, std::string Action, /*std::string Data,*/ std::string Referer);

	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;

	DWORD m_dwLastError;
	bool m_bUseSSL;
	std::string m_ServerName;

};

#endif // !HTTPREADER_H