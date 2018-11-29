#ifndef Authorization_h
#define Authorization_h

#include <string>

class HttpReader;

// ����� ����������� � ��� ����
class Authorization
{
public:
	Authorization(HttpReader *network);

	// ����������� � ��
	bool AuthorizationVK(int reconnect = 3);
	// ���������� ID ������������
	std::string getUserId();
	// ���������� ��� ������������
	std::string getUserName();

private:
	HttpReader *m_network;
	std::string m_login;
	std::string m_password;
	std::string m_userid;
	std::string m_username;

	// ������� ���
	bool ClearCache() const;
	// ������ ����� � ������ �� �����
	// ���������� true ���� ������� ����� � ������
	bool loginPassFromFile();
	// ���������� ����� � ������ � ����
	void loginPassToFile() const;
};

#endif // !Authorization_h

