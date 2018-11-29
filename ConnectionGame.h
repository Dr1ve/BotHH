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
	// ���������� ������ � ����
	bool Send(const std::string &data, int reconnect = 20);
	// ����� �� ������
	std::string Answer() const;
private:
	HttpReader *m_network;
	Authorization *m_auth;
	DataGame *m_datagame;
	std::string m_answer;

	// ����� ������������� ������ � ����
	unsigned int Request_Id;

	// ������������� ���������� � �����
	void Connection();
	void writedatagame();
	void setHeaderGame(const std::string &datasend);
};

#endif // !ConnectionGame_h

