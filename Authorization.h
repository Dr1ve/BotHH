#ifndef Authorization_h
#define Authorization_h

#include <string>

class HttpReader;

// Класс авторизации в соц сети
class Authorization
{
public:
	Authorization(HttpReader *network);

	// Авторизация в ВК
	bool AuthorizationVK(int reconnect = 3);
	// Возвращает ID пользователя
	std::string getUserId();
	// Возвращает Имя пользователя
	std::string getUserName();

private:
	HttpReader *m_network;
	std::string m_login;
	std::string m_password;
	std::string m_userid;
	std::string m_username;

	// Очищает кэш
	bool ClearCache() const;
	// Читает логин и пароль из файла
	// Возвращает true если получил логин и пароль
	bool loginPassFromFile();
	// Записывает логин и пароль в файл
	void loginPassToFile() const;
};

#endif // !Authorization_h

