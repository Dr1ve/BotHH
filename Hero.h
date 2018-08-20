#ifndef HERO_H
#define HERO_H

#include "includes.h"

class Hero
{
public:
	Hero();

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

	std::string get_id() { return id; }
	std::string get_xp() { return xp; }
	std::string get_level() { return level; }
	std::string get_color() { return color; }
	std::string get_power() { return power; }
	std::string get_star() { return star; }
	std::string get_skins() { return skins; }
	std::string get_currentSkin() { return currentSkin; }
	std::string get_titanGiftLevel() { return titanGiftLevel; }
	std::string get_scale() { return scale; }

	~Hero();

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

