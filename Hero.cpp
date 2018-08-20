#include "Hero.h"

Hero::Hero()
{
}

Hero::~Hero()
{
}


void Hero::set_id(std::string id)
{
	this->id = id;
}

void Hero::set_xp(std::string xp)
{
	this->xp = xp;
}

void Hero::set_level(std::string level)
{
	this->level = level;
}

void Hero::set_color(std::string color)
{
	this->color = color;
}

void Hero::set_power(std::string power)
{
	this->power = power;
}

void Hero::set_star(std::string star)
{
	this->star = star;
}

void Hero::set_skins(std::string skins)
{
	this->skins = skins;
}

void Hero::set_currentSkin(std::string currentSkin)
{
	this->currentSkin = currentSkin;
}

void Hero::set_titanGiftLevel(std::string titanGiftLevel)
{
	this->titanGiftLevel = titanGiftLevel;
}

void Hero::set_scale(std::string scale)
{
	this->scale = scale;
}
