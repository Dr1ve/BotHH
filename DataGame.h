#ifndef DATAGAME_H
#define DATAGAME_H

#include "includes.h"

class Hero;

class DataGame
{
public:
	DataGame();

	void ClearData();

	void setUserId(std::string userid) { this->UserId = userid; }
	std::string getUserId() { return UserId; }
	void set_api_url(std::string api_url) { this->api_url = api_url; }
	std::string get_api_url() { return api_url; }
	void set_api_id(std::string api_id) { this->api_id = api_id; }
	std::string get_api_id() { return api_id; }
	void set_api_settings(std::string api_settings) { this->api_settings = api_settings; }
	std::string get_api_settings() { return api_settings; }
	void set_viewer_type(std::string viewer_type) { this->viewer_type = viewer_type; }
	std::string get_viewer_type() { return viewer_type; }
	void set_sid(std::string sid) { this->sid = sid; }
	std::string get_sid() { return sid; }
	void set_secret(std::string secret) { this->secret = secret; }
	std::string get_secret() { return secret; }
	void set_access_token(std::string access_token) { this->access_token = access_token; }
	std::string get_access_token() { return access_token; }
	void set_group_id(std::string group_id) { this->group_id = group_id; }
	std::string get_group_id() { return group_id; }
	void set_is_app_user(std::string is_app_user) { this->is_app_user = is_app_user; }
	std::string get_is_app_user() { return is_app_user; }
	void set_auth_key(std::string auth_key) { this->auth_key = auth_key; }
	std::string get_auth_key() { return auth_key; }
	void set_language(std::string language) { this->language = language; }
	std::string get_language() { return language; }
	void set_parent_language(std::string parent_language) { this->parent_language = parent_language; }
	std::string get_parent_language() { return parent_language; }
	void set_ad_info(std::string ad_info) { this->ad_info = ad_info; }
	std::string get_ad_info() { return ad_info; }
	void set_is_secure(std::string is_secure) { this->is_secure = is_secure; }
	std::string get_is_secure() { return is_secure; }
	void set_ads_app_id(std::string ads_app_id) { this->ads_app_id = ads_app_id; }
	std::string get_ads_app_id() { return ads_app_id; }
	void set_referrer(std::string referrer) { this->referrer = referrer; }
	std::string get_referrer() { return referrer; }
	void set_lc_name(std::string lc_name) { this->lc_name = lc_name; }
	std::string get_lc_name() { return lc_name; }
	void set_hash(std::string hash) { this->hash = hash; }
	std::string get_hash() { return hash; }
	void set_username(std::string username) { this->UserName = username; }
	std::string get_username() { return UserName; }
	void set_stats_hash(std::string stats_hash) { this->stats_hash = stats_hash; }
	std::string get_stats_hash() { return stats_hash; }
	void set_player_id(std::string player_id) { this->player_id = player_id; }
	std::string get_player_id() { return player_id; }
	void set_session_id(std::string session_id) { this->session_id = session_id; }
	std::string get_session_id() { return session_id; }
	void set_library_version(std::string library_version) { this->library_version = library_version; }
	std::string get_library_version() { return library_version; }
	void set_network_ident(std::string network_ident) { this->network_ident = network_ident; }
	std::string get_network_ident() { return network_ident; }
	void set_heroes(Hero *h) { heroes.push_back(*h); }
	std::vector<Hero> get_heroes() { return heroes; }
	void clearHeroes() { heroes.clear(); }

private:
	std::string UserId;
	std::string api_url;
	std::string api_id;
	std::string api_settings;
	std::string viewer_type;
	std::string sid;
	std::string secret;
	std::string access_token;
	std::string group_id;
	std::string is_app_user;
	std::string auth_key;
	std::string language;
	std::string parent_language;
	std::string ad_info;
	std::string is_secure;
	std::string stats_hash;
	std::string ads_app_id;
	std::string referrer;
	std::string lc_name;
	std::string hash;
	std::string level;
	std::string network_ident;
	std::string library_version;
	std::string session_id;

	std::string offer;
	std::string ownerid;

	std::string UserName;
	std::string player_id;

	std::vector<Hero> heroes;
};

#endif // !DATAGAME_H