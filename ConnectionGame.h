#ifndef ConnectionGame_h
#define ConnectionGame_h

#include <string>

class HttpReader;
class Authorization;
class DataGame;

class ConnectionGame
{
public:
	ConnectionGame(DataGame *datagame);
	~ConnectionGame();
	// Отправляет запрос в игру
	bool Send(const std::string &data, int reconnect = 20);
	// Ответ на запрос
	std::string Answer() const;
private:
	HttpReader *m_network;
	Authorization *m_auth;
	DataGame *m_datagame;
	std::string m_answer;

	// Номер отправленного пакета в игру
	unsigned int Request_Id;

	// Устанавливает соединение с игрой
	void Connection();
	void writedatagame();
	void setHeaderGame(const std::string &datasend);
};

#endif // !ConnectionGame_h

