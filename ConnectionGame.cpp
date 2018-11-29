#include "ConnectionGame.h"

#include "HttpReader.h"
#include "Authorization.h"
#include "DataGame.h"
#include "md5.h"
#include <chrono>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

std::string strPad(std::string p1, int p2, std::string p3)
{
	while (p1.length() < (size_t)p2)
		p1 = p3 + p1;
	return p1;
}

std::string createSessionId()
{
	srand((unsigned int)time(NULL));
	unsigned int unix_epoch = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	unix_epoch = (unsigned int)floor(unix_epoch);
	unsigned int rand_num = (unsigned int)(rand() * 4.294967295E9);
	unix_epoch &= static_cast<unsigned int>(4.294967295E9);
	char buf[10];
	_itoa_s(unix_epoch, &buf[0], sizeof(buf), 36);
	std::string l1(buf);
	_itoa_s(rand_num, &buf[0], sizeof(buf), 36);
	std::string l2(buf);

	return strPad(l1, 7, "0") + strPad(l2, 7, "0");
}

ConnectionGame::ConnectionGame(DataGame *datagame)
	: m_datagame(datagame)
{
	// Создаем класс работы с сетью
	m_network = new HttpReader("vk.com", true);
	// Авторизуемся в соцсети
	m_auth = new Authorization(m_network);
	if (m_auth->AuthorizationVK())
	{
		// Если авторизация прошла, то коннектимся к игре
		m_datagame->setUserId(m_auth->getUserId());
		m_datagame->set_username(m_auth->getUserName());
		m_datagame->set_network_ident("vkontakte");
		m_datagame->set_library_version("1");

		Connection();
	}
}

ConnectionGame::~ConnectionGame()
{
	delete m_network;
	delete m_auth;
}

bool ConnectionGame::Send(const std::string & data, int reconnect)
{
	m_network->CloseConnection();
	m_network->OpenConnection("heroes-vk.nextersglobal.com");
	if (m_network->Post("/api/", "https://heroes.cdnvideo.ru/vk/v0393/assets/MiniHeroLoader.swf"))
	{
		setHeaderGame(data);
		m_network->Send(data);
		m_answer = m_network->GetData();
	}
	else {
		return false;
	}
	//Проверяю полученный ответ, если есть фигурная скобка значит пришел json ответ, значит все впорядке
	//Если в ответе пришло слово error то запрос не прошел
	if ((m_answer.find("{") != std::string::npos) && (m_answer.find("error") == std::string::npos))
		return true;
	else
	{
		//Если пришли слова Invalid signature, то пытаюсь переподключиться
		if (m_answer == "Invalid signature")
		{
			if (reconnect == 0)
			{
				Sleep(5000);
				// пытаюсь переподключиться к соцсети
				if (m_auth->AuthorizationVK())
				{
					Connection();
					return Send(data, 0);
				}
				else
					return false;
			}
			else
			{
				// Жду 5 секунд и пытаюсь переподключиться
				Sleep(5000);

				Connection();
				return Send(data, reconnect - 1);
			}
		}
		else
		{
			// Если пришли слова Internal error, то пытаюсь переподключиться
			if (m_answer.find("Internal error") != std::string::npos)
			{
				Sleep(5000);
				// пытаюсь переподключиться к соцсети
				if (m_auth->AuthorizationVK())
				{
					Connection();
					return Send(data, 0);
				}
			}
			else
			{
				// Если пришли слова Invalid session, то переподключаюсь
				if (m_answer.find("Invalid session") != std::string::npos)
				{
					Sleep(5000);
					// пытаюсь переподключиться к соцсети
					if (m_auth->AuthorizationVK())
					{
						Connection();
						return Send(data, 0);
					}
				}
				else
					return false;
			}
		}
	}

	return false; // заглушка чтоб убрать варнинг
}

std::string ConnectionGame::Answer() const
{
	return m_answer;
}

void ConnectionGame::Connection()
{
	// Проставляю новый ид сессии при переподключении
	m_datagame->set_session_id(createSessionId());
	// Обнуляю счетчик отправленных пакетов
	Request_Id = 0;
	// Считываем данные для игры
	writedatagame();
	// Отправляю первый запрос в игру
	std::string data = "{\"calls\":[{\"ident\":\"body\",\"name\":\"userMergeGetStatus\",\"args\":{}},{\"ident\":\"getTime\",\"name\":\"getTime\",\"args\":{}}],\"session\":null}";
	if (!Send(data))
	{
		std::cout << "Запрос не обработался: " << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_answer << std::endl;
	}
	//запрос инфы
	data = "{\"calls\":[";
	//инфа об аккаунте
	data += "{\"name\":\"userGetInfo\",\"ident\":\"userGetInfo\",\"args\":{}}";
	//хз что за инфа
	data += ",{\"name\":\"offerVk\",\"ident\":\"offerVk\",\"args\":{\"vk_sid\":null,\"vk_hash\":null,\"vk_lead_id\":null,\"vk_uid\":null}}";
	//как я понял общая инфа
	data += ",{\"name\":\"billingGetAll\",\"ident\":\"billingGetAll\",\"args\":{}}";
	//информацию об инвентаре
	data += ",{\"name\":\"inventoryGet\",\"ident\":\"inventoryGet\",\"args\":{}}";
	//информация об героях
	data += ",{\"name\":\"heroGetAll\",\"ident\":\"heroGetAll\",\"args\":{}}";
	data += ",{\"name\":\"titanGetAll\",\"ident\":\"titanGetAll\",\"args\":{}}";
	data += ",{\"name\":\"titanSpiritGetAll\",\"ident\":\"titanSpiritGetAll\",\"args\":{}}";
	data += ",{\"name\":\"missionGetAll\",\"ident\":\"missionGetAll\",\"args\":{}}";
	data += ",{\"name\":\"missionGetReplace\",\"ident\":\"missionGetReplace\",\"args\":{}}";
	data += ",{\"name\":\"dailyBonusGetInfo\",\"ident\":\"dailyBonusGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"getTime\",\"ident\":\"getTime\",\"args\":{}}";
	data += ",{\"name\":\"teamGetAll\",\"ident\":\"teamGetAll\",\"args\":{}}";
	data += ",{\"name\":\"questGetAll\",\"ident\":\"questGetAll\",\"args\":{}}";
	data += ",{\"name\":\"questGetEvents\",\"ident\":\"questGetEvents\",\"args\":{}}";
	data += ",{\"name\":\"mailGetAll\",\"ident\":\"mailGetAll\",\"args\":{}}";
	data += ",{\"name\":\"arenaGetAll\",\"ident\":\"arenaGetAll\",\"args\":{}}";
	data += ",{\"name\":\"socialQuestGetInfo\",\"ident\":\"socialQuestGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"userGetAvailableAvatars\",\"ident\":\"userGetAvailableAvatars\",\"args\":{}}";
	data += ",{\"name\":\"settingsGetAll\",\"ident\":\"settingsGetAll\",\"args\":{}}";
	data += ",{\"name\":\"subscriptionGetInfo\",\"ident\":\"subscriptionGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"zeppelinGiftGet\",\"ident\":\"zeppelinGiftGet\",\"args\":{}}";
	data += ",{\"name\":\"tutorialGetInfo\",\"ident\":\"tutorialGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"offerGetAll\",\"ident\":\"offerGetAll\",\"args\":{}}";
	data += ",{\"name\":\"splitGetAll\",\"ident\":\"splitGetAll\",\"args\":{}}";
	data += ",{\"name\":\"billingGetLast\",\"ident\":\"billingGetLast\",\"args\":{}}";
	data += ",{\"name\":\"artifactGetChestLevel\",\"ident\":\"artifactGetChestLevel\",\"args\":{}}";
	data += ",{\"name\":\"titanArtifactGetChest\",\"ident\":\"titanArtifactGetChest\",\"args\":{}}";
	data += ",{\"name\":\"titanGetSummoningCircle\",\"ident\":\"titanGetSummoningCircle\",\"args\":{}}";
	data += ",{\"name\":\"newYearGetInfo\",\"ident\":\"newYearGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"clanWarGetBriefInfo\",\"ident\":\"clanWarGetBriefInfo\",\"args\":{}}";
	data += ",{\"name\":\"clanWarGetWarlordInfo\",\"ident\":\"clanWarGetWarlordInfo\",\"args\":{}}";
	data += ",{\"name\":\"chatGetAll\",\"ident\":\"chatGetAll\",\"args\":{\"chatType\":\"clan\"}}";
	data += ",{\"name\":\"chatGetInfo\",\"ident\":\"chatGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"clanGetInfo\",\"ident\":\"clanGetInfo\",\"args\":{}}";
	data += ",{\"name\":\"heroesMerchantGet\",\"ident\":\"heroesMerchantGet\",\"args\":{}}";
	data += ",{\"name\":\"freebieHaveGroup\",\"ident\":\"freebieHaveGroup\",\"args\":{}}";
	data += ",{\"name\":\"pirateTreasureIsAvailable\",\"ident\":\"pirateTreasureIsAvailable\",\"args\":{}}";
	data += ",{\"name\":\"expeditionGet\",\"ident\":\"expeditionGet\",\"args\":{}}";
	data += ",{\"name\":\"hallOfFameGetTrophies\",\"ident\":\"hallOfFameGetTrophies\",\"args\":{}}";
	data += ",{\"name\":\"titanArenaGetChestReward\",\"ident\":\"titanArenaGetChestReward\",\"args\":{}}";
	data += ",{\"name\":\"bossGetAll\",\"ident\":\"bossGetAll\",\"args\":{}}";

	data += "],\"session\":null}";
	if (Send(data))
	{
		//std::cout << std::to_string(Request_Id) + " - запрос обработался\n";
		//std::cout << "\n\nОтвет:" << m_html << std::endl;

		// Очищаю массив героев
		m_datagame->clearHeroes();

		std::stringstream ss(m_answer);
		pt::ptree root;
		pt::read_json(ss, root);
		for (pt::ptree::value_type &r : root.get_child("results"))
		{
			std::string ident = r.second.get_child("ident").data();

			if (ident == "userGetInfo")
			{
				m_datagame->set_player_id(r.second.get_child("result.response.id").data());
			}
			else
			{
				if (ident == "heroGetAll")
				{
					for (pt::ptree::value_type &c : r.second.get_child("result.response"))
					{
						Hero h;
						h.set_id(c.second.get_child("id").data());
						h.set_xp(c.second.get_child("xp").data());
						h.set_level(c.second.get_child("level").data());
						h.set_color(c.second.get_child("color").data());
						h.set_power(c.second.get_child("power").data());
						h.set_star(c.second.get_child("star").data());
						//h.set_skins(c.second.get_child("xp").data());
						h.set_currentSkin(c.second.get_child("currentSkin").data());
						h.set_titanGiftLevel(c.second.get_child("titanGiftLevel").data());
						h.set_scale(c.second.get_child("scale").data());

						m_datagame->set_heroes(h);
					}
				}
			}

		}
	}
	else {
		std::cout << "Запрос не обработался: " << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_answer << std::endl;
	}
}

void ConnectionGame::writedatagame()
{
	std::string html;
	//запрос для получения данных
	std::string param = "/al_profile.php?";
	param += "__query=app5327745_" + m_datagame->getUserId();
	param += "&_ref=left_nav";
	param += "&al=-1";
	param += "&al_id=" + m_datagame->getUserId();
	param += "&ref=1";

	m_network->CloseConnection();
	m_network->OpenConnection("vk.com");
	if (m_network->Get(param))
	{
		m_network->Send();
		html = m_network->GetData();
	}

	//Вытягиваю api_url
	std::string tmp = m_network->getParam(html, "\\\"api_url\\\"", "\\\"", "\\\"");
	std::string::iterator it = std::remove(tmp.begin(), tmp.end(), '\\');
	tmp.erase(it, tmp.end());
	m_datagame->set_api_url(tmp);
	//Вытягиваю api_id ид приложения
	m_datagame->set_api_id(m_network->getParam(html, "\\\"api_id\\\"", ":", ","));
	//Вытягиваю api_settings
	m_datagame->set_api_settings(m_network->getParam(html, "\\\"api_settings\\\"", ":", ","));
	//Вытягиваю viewer_type
	m_datagame->set_viewer_type(m_network->getParam(html, "\\\"viewer_type\\\"", ":", ","));
	//Вытягиваю sid
	m_datagame->set_sid(m_network->getParam(html, "\\\"sid\\\"", "\\\"", "\\\""));
	//Вытягиваю secret
	m_datagame->set_secret(m_network->getParam(html, "\\\"secret\\\"", "\\\"", "\\\""));
	//Вытягиваю access_token
	m_datagame->set_access_token(m_network->getParam(html, "\\\"access_token\\\"", "\\\"", "\\\""));
	//Вытягиваю group_id
	m_datagame->set_group_id(m_network->getParam(html, "\\\"group_id\\\"", ":", ","));
	//Вытягиваю is_app_user
	m_datagame->set_is_app_user(m_network->getParam(html, "\\\"is_app_user\\\"", ":", ","));
	//Вытягиваю auth_key
	m_datagame->set_auth_key(m_network->getParam(html, "\\\"auth_key\\\"", "\\\"", "\\\""));
	//Вытягиваю language
	m_datagame->set_language(m_network->getParam(html, "\\\"language\\\"", ":", ","));
	//Вытягиваю parent_language
	m_datagame->set_parent_language(m_network->getParam(html, "\\\"parent_language\\\"", ":", ","));
	//Вытягиваю ad_info
	m_datagame->set_ad_info(m_network->getParam(html, "\\\"ad_info\\\"", "\\\"", "\\\""));
	//Вытягиваю is_secure
	m_datagame->set_is_secure(m_network->getParam(html, "\\\"is_secure\\\"", ":", ","));
	//Вытягиваю stats_hash
	m_datagame->set_stats_hash(m_network->getParam(html, "\\\"stats_hash\\\"", "\\\"", "\\\""));
	//Вытягиваю ads_app_id
	m_datagame->set_ads_app_id(m_network->getParam(html, "\\\"ads_app_id\\\"", "\\\"", "\\\""));
	//Вытягиваю referrer
	m_datagame->set_referrer(m_network->getParam(html, "\\\"referrer\\\"", "\\\"", "\\\""));
	//Вытягиваю lc_name
	m_datagame->set_lc_name(m_network->getParam(html, "\\\"lc_name\\\"", "\\\"", "\\\""));
	//Вытягиваю hash
	m_datagame->set_hash("");

	//Вытягиваю auth_key для шапки пакетов игры
	m_network->CloseConnection();
	std::string url = "/iframe/vkontakte/iframe.new.php?";
	url += "api_url=" + m_datagame->get_api_url();
	url += "&api_id=" + m_datagame->get_api_id();
	url += "&api_settings=" + m_datagame->get_api_settings();
	url += "&viewer_id=" + m_datagame->getUserId();
	url += "&viewer_type=" + m_datagame->get_viewer_type();
	url += "&sid=" + m_datagame->get_sid();
	url += "&secret=" + m_datagame->get_secret();
	url += "&access_token=" + m_datagame->get_access_token();
	url += "&user_id=" + m_datagame->getUserId();
	url += "&group_id=" + m_datagame->get_group_id();
	url += "&is_app_user=" + m_datagame->get_is_app_user();
	url += "&auth_key=" + m_datagame->get_auth_key();
	url += "&language=" + m_datagame->get_language();
	url += "&parent_language=" + m_datagame->get_parent_language();
	url += "&is_secure=" + m_datagame->get_is_secure();
	url += "&stats_hash=" + m_datagame->get_stats_hash();
	url += "&ads_app_id=" + m_datagame->get_ads_app_id();
	url += "&referrer=" + m_datagame->get_referrer();
	url += "&lc_name=" + m_datagame->get_lc_name();
	url += "&hash=" + m_datagame->get_hash();

	m_network->OpenConnection("i-heroes-vk.nextersglobal.com");
	if (m_network->Get(url))
	{
		m_network->Send();
		html = m_network->GetData();
	}
	m_datagame->set_auth_key(m_network->getParam(html, "auth_key", "=", "&"));
}

void ConnectionGame::setHeaderGame(const std::string &datasend)
{
	Request_Id++;
	std::string source = std::to_string(Request_Id);
	source += ":" + m_datagame->get_auth_key();
	source += ":" + m_datagame->get_session_id();
	source += ":" + datasend;
	/*надо собирать все атрибуты начинающиеся на X-Env потом сортировать их по алфавиту и дописывать в конец.
	  Например в пакете присутствует
	  X-Env-Referrer: menu
	  X-Env-Library-Version: 1
	  отсекается X-Env и сортируется и в верхний регист переводится
	  и записывается в итоге :LIBRARY-VERSION=1REFERRER=menu

	  Но я чтоб не заморачиваться только одно значени буду отправлять если пакет будет ругаться
	  буду пробовать с несколькими значениями*/
	source += ":LIBRARY-VERSION=" + m_datagame->get_library_version();

	//X-Auth-Signature
	std::string signature = "";
	signature = md5(source);

	//X-Auth-Signature-Key
	/*этого атрибута нету*/

	m_network->AddRequestHeader("X-Auth-User-Id: " + m_datagame->getUserId());
	if (!m_datagame->get_player_id().empty())
		m_network->AddRequestHeader("X-Auth-Player-Id: " + m_datagame->get_player_id());
	//m_network->AddRequestHeader("X-Env-Referrer: menu");
	m_network->AddRequestHeader("X-Auth-Network-Ident: " + m_datagame->get_network_ident());
	m_network->AddRequestHeader("X-Auth-Token: " + m_datagame->get_auth_key());
	m_network->AddRequestHeader("X-Env-Library-Version: " + m_datagame->get_library_version());
	m_network->AddRequestHeader("X-Requested-With: ShockwaveFlash/30.0.0.113");
	m_network->AddRequestHeader("X-Request-Id: " + std::to_string(Request_Id));
	m_network->AddRequestHeader("X-Auth-Signature: " + signature);
	m_network->AddRequestHeader("X-Server-Time: 0");
	m_network->AddRequestHeader("X-Auth-Session-Id: " + m_datagame->get_session_id());
	m_network->AddRequestHeader("X-Auth-Session-Key: \"\"");
	m_network->AddRequestHeader("X-Auth-Application-Id: " + m_datagame->get_api_id());
	m_network->AddRequestHeader("Origin: https://heroes.cdnvideo.ru");
	if (Request_Id == 1)
	{
		m_network->AddRequestHeader("X-Auth-Session-Init: 1");
	}

	//std::cout << md5(":LIBRARY-VERSION=1REFERRER=menu") << std::endl;
}
