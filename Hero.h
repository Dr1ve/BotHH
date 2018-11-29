#ifndef HERO_H
#define HERO_H

#include <iostream>

class Hero
{
public:
	void set_id(std::string id);
	void set_xp(std::string xp);
	void set_level(std::string level);
	void set_color(std::string color);
	void set_power(std::string power);
	void set_star(std::string star);
	void set_skins(std::string skins);
	void set_currentSkin(std::string currentSkin);
	void set_titanGiftLevel(std::string titanGiftLevel);
	void set_scale(std::string scale);

	std::string get_id();
	std::string get_xp();
	std::string get_level();
	std::string get_color();
	std::string get_power();
	std::string get_star();
	std::string get_skins();
	std::string get_currentSkin();
	std::string get_titanGiftLevel();
	std::string get_scale();

private:
	std::string id;
	std::string xp;
	std::string level;
	std::string color;
	std::string power;
	std::string star;
	std::string skins;
	std::string currentSkin;
	std::string titanGiftLevel;
	std::string scale;
};

#endif // !HERO_H

