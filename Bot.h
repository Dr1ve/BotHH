#pragma once
#ifndef BOT_H
#define BOT_H

#include "includes.h"

class DataGame;
class HttpReader;

struct ConfigBot {
	bool freeChest;
};

class Bot
{
public:
	Bot(DataGame *datagame);
	~Bot();

	void set_datagame(DataGame *datagame) { m_datagame = datagame; }

	void writedatagame();

	void setHeaderGame(std::string datasend);

	void Loop();

private:
	bool sendapi(std::string data);

	void tower();
	void dungeon();
	void chestboss();
	void zeppelin();

	void recursive(int, unsigned int, int, int);
	std::vector<int> currentTeam;
	std::vector<int> minPowerTeam;
	std::vector<Hero> list;
	unsigned int power;
	unsigned int minPower = INT_MAX;
	unsigned int maxPowerHero = 0;

	DataGame * m_datagame;
	HttpReader * m_network;
	std::string m_html;
	unsigned int Request_Id;
	ConfigBot m_configbot;
	std::string lastevent;
};

#endif // !BOT_H