#include "Application.h"

#include "Bot.h"

Application::Application()
{
	//������ ������ �� ����
	m_bot = new Bot;
}

Application::~Application()
{
	delete m_bot;
}

void Application::Loop()
{
	m_bot->Loop();
}
