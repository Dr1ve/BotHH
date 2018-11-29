#include "DataGame.h"

DataGame::DataGame()
{
	
}

void DataGame::ClearData()
{
	UserId = "";
	api_url = "";
	api_id = "";
	api_settings = "";
	viewer_type = "";
	sid = "";
	secret = "";
	access_token = "";
	group_id = "";
	is_app_user = "";
	auth_key = "";
	language = "";
	parent_language = "";
	ad_info = "";
	is_secure = "";
	stats_hash = "";
	ads_app_id = "";
	referrer = "";
	lc_name = "";
	hash = "";
	level = "";
	network_ident = "";
	library_version = "";
	session_id = "";
	offer = "";
	ownerid = "";
	UserName = "";
}

void DataGame::setUserId(const std::string & userid)
{
	this->UserId = userid;
}

void DataGame::set_api_url(const std::string & api_url)
{
	this->api_url = api_url;
}

void DataGame::set_api_id(const std::string & api_id)
{
	this->api_id = api_id;
}

void DataGame::set_api_settings(const std::string & api_settings)
{
	this->api_settings = api_settings;
}

void DataGame::set_viewer_type(const std::string & viewer_type)
{
	this->viewer_type = viewer_type;
}

void DataGame::set_sid(const std::string & sid)
{
	this->sid = sid;
}

void DataGame::set_secret(const std::string & secret)
{
	this->secret = secret;
}

void DataGame::set_access_token(const std::string & access_token)
{
	this->access_token = access_token;
}

void DataGame::set_group_id(const std::string & group_id)
{
	this->group_id = group_id;
}

void DataGame::set_is_app_user(const std::string & is_app_user)
{
	this->is_app_user = is_app_user;
}

void DataGame::set_auth_key(const std::string & auth_key)
{
	this->auth_key = auth_key;
}

void DataGame::set_language(const std::string & language)
{
	this->language = language;
}

void DataGame::set_parent_language(const std::string & parent_language)
{
	this->parent_language = parent_language;
}

void DataGame::set_ad_info(const std::string & ad_info)
{
	this->ad_info = ad_info;
}

void DataGame::set_is_secure(const std::string & is_secure)
{
	this->is_secure = is_secure;
}

void DataGame::set_ads_app_id(const std::string & ads_app_id)
{
	this->ads_app_id = ads_app_id;
}

void DataGame::set_referrer(const std::string & referrer)
{
	this->referrer = referrer;
}

void DataGame::set_lc_name(const std::string & lc_name)
{
	this->lc_name = lc_name;
}

void DataGame::set_hash(const std::string & hash)
{
	this->hash = hash;
}

void DataGame::set_username(const std::string & username)
{
	this->UserName = username;
}

void DataGame::set_stats_hash(const std::string & stats_hash)
{
	this->stats_hash = stats_hash;
}

void DataGame::set_player_id(const std::string & player_id)
{
	this->player_id = player_id;
}

void DataGame::set_session_id(const std::string & session_id)
{
	this->session_id = session_id;
}

void DataGame::set_library_version(const std::string & library_version)
{
	this->library_version = library_version;
}

void DataGame::set_network_ident(const std::string & network_ident)
{
	this->network_ident = network_ident;
}

void DataGame::set_heroes(const Hero & h)
{
	heroes.push_back(h);
}

std::string DataGame::getUserId()
{
	return UserId;
}

std::string DataGame::get_api_url()
{
	return api_url;
}

std::string DataGame::get_api_id()
{
	return api_id;
}

std::string DataGame::get_api_settings()
{
	return api_settings;
}

std::string DataGame::get_viewer_type()
{
	return viewer_type;
}

std::string DataGame::get_sid()
{
	return sid;
}

std::string DataGame::get_secret()
{
	return secret;
}

std::string DataGame::get_access_token()
{
	return access_token;
}

std::string DataGame::get_group_id()
{
	return group_id;
}

std::string DataGame::get_is_app_user()
{
	return is_app_user;
}

std::string DataGame::get_auth_key()
{
	return auth_key;
}

std::string DataGame::get_language()
{
	return language;
}

std::string DataGame::get_parent_language()
{
	return parent_language;
}

std::string DataGame::get_ad_info()
{
	return ad_info;
}

std::string DataGame::get_is_secure()
{
	return is_secure;
}

std::string DataGame::get_ads_app_id()
{
	return ads_app_id;
}

std::string DataGame::get_referrer()
{
	return referrer;
}

std::string DataGame::get_lc_name()
{
	return lc_name;
}

std::string DataGame::get_hash()
{
	return hash;
}

std::string DataGame::get_username()
{
	return UserName;
}

std::string DataGame::get_stats_hash()
{
	return stats_hash;
}

std::string DataGame::get_player_id()
{
	return player_id;
}

std::string DataGame::get_session_id()
{
	return session_id;
}

std::string DataGame::get_library_version()
{
	return library_version;
}

std::string DataGame::get_network_ident()
{
	return network_ident;
}

std::vector<Hero> DataGame::get_heroes()
{
	return heroes;
}

void DataGame::clearHeroes()
{
	heroes.clear();
}
