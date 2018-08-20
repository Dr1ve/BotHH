#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"

class Application
{
public:
	Application();
	~Application();
	
	void Loop();

	//������� ���
	BOOL DelCache();
	//����������� ��
	bool AuthorizationVK();

private:
	std::string m_login;
	std::string m_password;
	bool m_authorization;
	DataGame m_datagame;
	HttpReader* m_socialnetwork;
	std::string m_html;
	std::string m_header;
	Bot* m_bot;
};

#endif // !APPLICATION_H