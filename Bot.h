#pragma once
#ifndef BOT_H
#define BOT_H

#include <string>
#include <vector>

class Hero;
class DataGame;
class ConnectionGame;

struct ConfigBot {
	bool freeChest;
};

class Bot
{
public:
	Bot();
	~Bot();

	void Loop();

private:
	void tower();
	void dungeon();
	void chestboss();
	void zeppelin();
	void getEnergy();

	void recursive(int, unsigned int, int, int);
	std::vector<int> currentTeam;
	std::vector<int> minPowerTeam;
	std::vector<Hero> list;
	unsigned int power;
	unsigned int minPower = INT_MAX;
	unsigned int maxPowerHero = 0;

	DataGame * m_datagame;
	ConfigBot m_configbot;
	std::string lastevent;
	ConnectionGame *m_connect;
};

#endif // !BOT_H