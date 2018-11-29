#ifndef DATAGAME_H
#define DATAGAME_H

#include <string>
#include <vector>

#include "Hero.h"

class DataGame
{
public:
	DataGame();

	void ClearData();

	void setUserId(const std::string &userid);
	void set_api_url(const std::string &api_url);
	void set_api_id(const std::string &api_id);
	void set_api_settings(const std::string &api_settings);
	void set_viewer_type(const std::string &viewer_type);
	void set_sid(const std::string &sid);
	void set_secret(const std::string &secret);
	void set_access_token(const std::string &access_token);
	void set_group_id(const std::string &group_id);
	void set_is_app_user(const std::string &is_app_user);
	void set_auth_key(const std::string &auth_key);
	void set_language(const std::string &language);
	void set_parent_language(const std::string &parent_language);
	void set_ad_info(const std::string &ad_info);
	void set_is_secure(const std::string &is_secure);
	void set_ads_app_id(const std::string &ads_app_id);
	void set_referrer(const std::string &referrer);
	void set_lc_name(const std::string &lc_name);
	void set_hash(const std::string &hash);
	void set_username(const std::string &username);
	void set_stats_hash(const std::string &stats_hash);
	void set_player_id(const std::string &player_id);
	void set_session_id(const std::string &session_id);
	void set_library_version(const std::string &library_version);
	void set_network_ident(const std::string &network_ident);
	void set_heroes(const Hero &h);

	std::string getUserId();
	std::string get_api_url();
	std::string get_api_id();
	std::string get_api_settings();
	std::string get_viewer_type();
	std::string get_sid();
	std::string get_secret();
	std::string get_access_token();
	std::string get_group_id();
	std::string get_is_app_user();
	std::string get_auth_key();
	std::string get_language();
	std::string get_parent_language();
	std::string get_ad_info();
	std::string get_is_secure();
	std::string get_ads_app_id();
	std::string get_referrer();
	std::string get_lc_name();
	std::string get_hash();
	std::string get_username();
	std::string get_stats_hash();
	std::string get_player_id();
	std::string get_session_id();
	std::string get_library_version();
	std::string get_network_ident();

	std::vector<Hero> get_heroes();
	void clearHeroes();

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