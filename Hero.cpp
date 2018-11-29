#include "Hero.h"

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

std::string Hero::get_id()
{
	return id;
}

std::string Hero::get_xp()
{
	return xp;
}

std::string Hero::get_level()
{
	return level;
}

std::string Hero::get_color()
{
	return color;
}

std::string Hero::get_power()
{
	return power;
}

std::string Hero::get_star()
{
	return star;
}

std::string Hero::get_skins()
{
	return skins;
}

std::string Hero::get_currentSkin()
{
	return currentSkin;
}

std::string Hero::get_titanGiftLevel()
{
	return titanGiftLevel;
}

std::string Hero::get_scale()
{
	return scale;
}
