#include "Bot.h"

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

Bot::Bot(DataGame *datagame)
	: Request_Id(0)
{
	m_configbot.freeChest = false;

	m_datagame = datagame;
	m_network = new HttpReader("vk.com", true);

	m_datagame->set_session_id(createSessionId());
	m_datagame->set_library_version("1");
	//Пока что не знаю от куда вытягивать player_id, свой ид я знаю так что пока что заполню константно
	//m_datagame->set_player_id("2762012");
	m_datagame->set_network_ident("vkontakte");
}

Bot::~Bot()
{
	if (m_network)
		delete m_network;
}

void showcache()
{
	BOOL bDone = FALSE;
	HANDLE hCacheDir = NULL;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
	DWORD dwTrySize, dwEntrySize = 4096;
	delete[] lpCacheEntry;
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	lpCacheEntry->dwStructSize = dwEntrySize;
	dwTrySize = dwEntrySize;
	do
	{
		BOOL bSuccess;
		if (hCacheDir == NULL)
			bSuccess = (hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwTrySize)) != NULL;
		else
			bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);

		if (!bSuccess)
			bDone = TRUE;
	} while (!bDone);
}

void Bot::writedatagame()
{
	//запрос для получения данных
	std::string a = m_datagame->getUserId();
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
		m_html = m_network->GetData();
	}

	//Вытягиваю api_url
	std::string tmp = m_network->getParam(m_html, "\\\"api_url\\\"", "\\\"", "\\\"");
	std::string::iterator it = std::remove(tmp.begin(), tmp.end(), '\\');
	tmp.erase(it, tmp.end());
	m_datagame->set_api_url(tmp);
	//Вытягиваю api_id ид приложения
	m_datagame->set_api_id(m_network->getParam(m_html, "\\\"api_id\\\"", ":", ","));
	//Вытягиваю api_settings
	m_datagame->set_api_settings(m_network->getParam(m_html, "\\\"api_settings\\\"", ":", ","));
	//Вытягиваю viewer_type
	m_datagame->set_viewer_type(m_network->getParam(m_html, "\\\"viewer_type\\\"", ":", ","));
	//Вытягиваю sid
	m_datagame->set_sid(m_network->getParam(m_html, "\\\"sid\\\"", "\\\"", "\\\""));
	//Вытягиваю secret
	m_datagame->set_secret(m_network->getParam(m_html, "\\\"secret\\\"", "\\\"", "\\\""));
	//Вытягиваю access_token
	m_datagame->set_access_token(m_network->getParam(m_html, "\\\"access_token\\\"", "\\\"", "\\\""));
	//Вытягиваю group_id
	m_datagame->set_group_id(m_network->getParam(m_html, "\\\"group_id\\\"", ":", ","));
	//Вытягиваю is_app_user
	m_datagame->set_is_app_user(m_network->getParam(m_html, "\\\"is_app_user\\\"", ":", ","));
	//Вытягиваю auth_key
	m_datagame->set_auth_key(m_network->getParam(m_html, "\\\"auth_key\\\"", "\\\"", "\\\""));
	//Вытягиваю language
	m_datagame->set_language(m_network->getParam(m_html, "\\\"language\\\"", ":", ","));
	//Вытягиваю parent_language
	m_datagame->set_parent_language(m_network->getParam(m_html, "\\\"parent_language\\\"", ":", ","));
	//Вытягиваю ad_info
	m_datagame->set_ad_info(m_network->getParam(m_html, "\\\"ad_info\\\"", "\\\"", "\\\""));
	//Вытягиваю is_secure
	m_datagame->set_is_secure(m_network->getParam(m_html, "\\\"is_secure\\\"", ":", ","));
	//Вытягиваю stats_hash
	m_datagame->set_stats_hash(m_network->getParam(m_html, "\\\"stats_hash\\\"", "\\\"", "\\\""));
	//Вытягиваю ads_app_id
	m_datagame->set_ads_app_id(m_network->getParam(m_html, "\\\"ads_app_id\\\"", "\\\"", "\\\""));
	//Вытягиваю referrer
	m_datagame->set_referrer(m_network->getParam(m_html, "\\\"referrer\\\"", "\\\"", "\\\""));
	//Вытягиваю lc_name
	m_datagame->set_lc_name(m_network->getParam(m_html, "\\\"lc_name\\\"", "\\\"", "\\\""));
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

	//showcache();

	m_network->OpenConnection("i-heroes-vk.nextersglobal.com");
	if (m_network->Get(url))
	{
		m_network->Send();
		m_html = m_network->GetData();
	}
	m_datagame->set_auth_key(m_network->getParam(m_html, "auth_key", "=", "&"));
	

}

void Bot::setHeaderGame(std::string datasend)
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
	m_network->AddRequestHeader("X-Auth-Network-Ident: vkontakte");
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

void Bot::Connection()
{
	Request_Id = 0;
	//считываем данные для игры
	writedatagame();

	//попытка отправки первого запроса
	std::string data = "{\"calls\":[{\"ident\":\"body\",\"name\":\"userMergeGetStatus\",\"args\":{}},{\"ident\":\"getTime\",\"name\":\"getTime\",\"args\":{}}],\"session\":null}";
	if (sendapi(data))
	{
		std::cout << std::to_string(Request_Id) + " - запрос обработался\n";
		//std::cout << "\n\nОтвет:" << m_html << std::endl;
	}
	else {
		std::cout << "Запрос не обработался: " << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_html << std::endl;
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
	if (sendapi(data))
	{
		std::cout << std::to_string(Request_Id) + " - запрос обработался\n";
		//std::cout << "\n\nОтвет:" << m_html << std::endl;

		std::stringstream ss(m_html);
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

						m_datagame->set_heroes(&h);
					}
				}
			}

		}
	}
	else {
		std::cout << "Запрос не обработался: " << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_html << std::endl;
	}
}

void Bot::Loop()
{
	bool b_exit = false;
	wchar_t sim = 0;

	system("cls");

	Connection();
	
	std::string data;

	bool paint = true;
	while (!b_exit)
	{
		sim = 0;

		if (paint)
		{
			system("cls");

			std::cout << "Последнее событие:" << std::endl;
			if (!lastevent.empty())
				std::cout << lastevent << std::endl;
			std::cout << "\nМеню\n";
			std::cout << "1 - собрать ежедневный сундук\n";
			std::cout << "2 - пройти башню\n";
			std::cout << "3 - пройти подземелье (в разработке)\n";
			std::cout << "4 - сбор добычи Боссов Запределья\n";
			std::cout << "5 - дирижабль\n";
			std::cout << "6 - запустить автосбор выполненных заданий\n";
			std::cout << "ESC Выход\n";

			paint = false;
		}
		
		if (_kbhit())
		{
			sim = _getwch();
			switch (sim)
			{
			case L'1'://Если нажали кнопку 1
					  //Если бесплатный сундук true значит уже собрали
					  //if (m_configbot.freeChest)
				data = "{\"session\":null,\"calls\":[{\"name\":\"chestBuy\",\"ident\":\"body\",\"args\":{\"pack\":false,\"chest\":\"town\",\"free\":true}}]}";
				if (sendapi(data))
				{
					lastevent = "Ежедневный бесплатный сундук был собран";
				}
				else {
					if (m_html.find("error") != std::string::npos)
					{
						lastevent = "Ежедневный бесплатный сундук был собран";
					} else {
						std::cout << "Запрос не обработался" << std::endl;
						std::cout << "\n\nЗапрос:" << data << std::endl;
						std::cout << "\n\nОтвет:" << m_html << std::endl;
						system("pause");
					}
				}
				paint = true;
				break;
			case L'2'://Если нажали кнопку 2
				tower();//башня
				paint = true;
				break;
			case L'3':
				//dungeon();//подземелье
				paint = true;
				break;
			case L'4':
				chestboss();//запределье
				paint = true;
				break;
			case L'5':
				zeppelin();//дирижабль
				paint = true;
				break;
			case L'6':
				getEnergy();//собираю энергию
				paint = true;
				break;
			case 27://Если нажали ESC выходим из цикла
				b_exit = true;
				break;
			}
		}
	}
}

void Bot::getEnergy()
{
	std::string text = "Идет сбор наград выполненных заданий\n";
	while (true)
	{
		system("cls");
		std::cout << text << std::endl << std::endl;
		std::cout << "Нажмите ESC чтоб завершить сбор\n";

		if (_kbhit())
		{
			wchar_t sim = _getwch();
			switch (sim)
			{
			case 27://если нажали ESC завершаем сбор
				return;
				break;
			}
		}
		else
		{
			//делаю паузу на 10 сек
			Sleep(10000);
			//отправляю запрос на получение информации по квестам
			std::string data = "{\"calls\":[{\"name\":\"questGetAll\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
			if (sendapi(data))
			{
				struct quest
				{
					std::string id;
					std::string state;
					std::string progress;
					//reward
				};
				std::vector<quest> quests;
				//парсю полученный ответ
				std::stringstream ss(m_html);
				pt::ptree root;
				pt::read_json(ss, root);
				for (pt::ptree::value_type &r : root.get_child("results..result.response"))
				{
					quest q;
					//записываю только те которые можно собрать (state=2)
					if ((q.state = r.second.get_child("state").data()) == "2")
					{
						q.id = r.second.get_child("id").data();
						q.progress = r.second.get_child("progress").data();

						quests.push_back(q);
					}					
				}
				//если массив не пустой то отправляю запросы на сбор выполненных заданий
				for (size_t i = 0; i < quests.size(); ++i)
				{
					data = "{\"calls\":[{\"name\":\"questFarm\",\"ident\":\"body\",\"args\":{\"questId\":" + quests[i].id + "}}],\"session\":null}";
					if (sendapi(data))
						text += "собрал награду с задания " + quests[i].id + "\n";
				}
			}
			else {
				//Проверяю если Invalid signature то делаю переподключение
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:" << data << std::endl;
				std::cout << "\n\nОтвет:" << m_html << std::endl;
				system("pause");
				return;
			}
		}
	}
}

bool Bot::sendapi(std::string data, int reconnect)
{
	m_network->CloseConnection();
	m_network->OpenConnection("heroes-vk.nextersglobal.com");
	if (m_network->Post("/api/","https://heroes.cdnvideo.ru/vk/v0393/assets/MiniHeroLoader.swf"))
	{
		setHeaderGame(data);
		m_network->Send(data);
		m_html = m_network->GetData();
	} else {
		return false;
	}
	//Проверяю полученный ответ, если есть фигурная скобка значит пришел json ответ, значит все впорядке
	//Если в ответе пришло слово error то запрос не прошел
	if ((m_html.find("{") != std::string::npos) && (m_html.find("error") == std::string::npos))
		return true;
	else
	{
		//Если пришло слова Invalid signature то пытаюсь переподключиться
		if (m_html == "Invalid signature")
		{
			if (reconnect == 0)
			{
				return false;
			}
			else
			{
				Connection();
				return sendapi(data, reconnect - 1);
			}			
		}
		else
		{
			return false;
		}
	}
}

void Bot::tower()
{
	bool b_exit = false;
	//запрашиваю информацию о башне
	std::string data = "{\"calls\":[{\"name\":\"towerGetInfo\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (sendapi(data))
	{
		std::cout << "Успешно запросили информацию о башне\n";
	} else {
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_html << std::endl;
		system("pause");
		b_exit = true;
	}

	//После каждого переходна на следующий этаж, информация об этаже приходит в ответе на запросы nextFloor и SkipFloor
	//Делаю цикл пока не закончатся этажи для скипа, выпадет ошибка в запросе или не нажмут отмену при выборе бафа
	while (!b_exit)
	{
		//Узнаю тип комнаты
		//std::string floorType = m_network->getVal(m_html, "results..result.response.tower.floorType");
		std::string floorType;
		std::string floorNumber;//максимум двузначное число
		std::string maySkipFloor;//максимум двузначное число
		floorType.reserve(7);//может быть только 3 значения battle,buff,chest поэтому резервируем по максимальному значению
		floorNumber.reserve(3);
		maySkipFloor.reserve(3);
		std::stringstream ss1(m_html);
		pt::ptree root1;
		pt::read_json(ss1, root1);
		for (pt::ptree::value_type &r : root1.get_child("results..result.response"))
		{
			if (r.first == "tower")
			{
				floorType = r.second.get_child("floorType").data();
				floorNumber = r.second.get_child("floorNumber").data();
				maySkipFloor = r.second.get_child("maySkipFloor").data();
				/*std::cout << floorType << std::endl << floorNumber << std::endl << maySkipFloor << std::endl;
				system("pause");
				for (pt::ptree::value_type &c : r.second)
				{
					if (c.first == "floorType") floorType = c.second.data();
					if (c.first == "floorNumber") floorNumber = c.second.data();
					if (c.first == "maySkipFloor") maySkipFloor = c.second.data();
				}*/
			}
			else
			{
				if (r.first == "floorType") floorType = r.second.data();
				if (r.first == "floorNumber") floorNumber = r.second.data();
				if (r.first == "maySkipFloor") maySkipFloor = r.second.data();
			}
		}
		//Текущий этаж
		/*std::string tmp = m_network->getParam(m_html, "\"floorNumber\"", ":", ",");
		std::string::iterator it = std::remove(tmp.begin(), tmp.end(), '\"');
		tmp.erase(it, tmp.end());
		std::string floorNumber = tmp;*/
		//std::string floorNumber = m_network->getVal(m_html, "results..result.response.floorNumber");

		//Получаю информацию сколько этажей я могу скипнуть
		//std::string maySkipFloor = m_network->getVal(m_html, "results..result.response.maySkipFloor");

		//Запрос на переход в следующую комнату
		std::string nextFloor = "{\"calls\":[{\"name\":\"towerNextFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";

		if (floorType == "battle")
		{
			//если текущий этаж меньше или равен количеству этажей которые я могу скипнуть, то скипаю
			if (std::stoi(maySkipFloor) >= std::stoi(floorNumber))
			{
				//Запрос на скип боя
				data = "{\"calls\":[{\"name\":\"towerSkipFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
				if (sendapi(data))
				{
					lastevent = "Скипнули бой на " + floorNumber + " этаже";
				}
				else {
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:" << data << std::endl;
					std::cout << "\n\nОтвет:" << m_html << std::endl;
					system("pause");
					b_exit = true;
				}
			}
			else {
				//Провести бой
				//Запрос на начало битвы
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[7,13,5,25,2]}}]}";
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[25]}}]}";
				/*if (sendapi(data))
				{
				std::cout << "Начал битву" << std::endl;
				} else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:\n" << data << std::endl;
				std::cout << "\n\nОтвет:\n" << m_html << std::endl;
				system("pause");
				}*/
				//Считываем seed
				//std::string seed = m_network->getParam(m_html, "\"seed\"", ":", ",");

				//Сделаю паузу в 5 сек
				//Sleep(5000);

				//Запрос на конец битвы
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630},\"2\":{\"energy\":1000,\"isDead\":false,\"hp\":233755},\"13\":{\"energy\":1000,\"isDead\":false,\"hp\":79010},\"5\":{\"energy\":1000,\"isDead\":false,\"hp\":70830},\"7\":{\"energy\":1000,\"isDead\":false,\"hp\":64347}},\"input\":[\"auto\",0,0,\"auto\"]},\"v\":142,\"defenders\":{\"heroes\":{},\"input\":[]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"stars\":3,\"win\":true}}}]}";
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630}},\"input\":[\"auto\",0,0,\"auto\"]},\"v\":139,\"defenders\":{\"heroes\":{},\"input\":[]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"stars\":3,\"win\":true}}}]}";
				/*if (sendapi(data))
				{
				std::cout << "Закончил битву" << std::endl;
				} else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:\n" << data << std::endl;
				std::cout << "\n\nОтвет:\n" << m_html << std::endl;
				system("pause");
				}*/
				//Запрос на переход на сл этаж
				/*if (sendapi(nextFloor))
				{
				std::cout << "Перешел на следующий этаж\n";
				} else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:\n" << data << std::endl;
				std::cout << "\n\nОтвет:\n" << m_html << std::endl;
				system("pause");
				}*/
				//std::cout << "Не могу скипнуть боевой этаж\n";

				//битва пока что не получается
				lastevent = "Все этажи закончились которые можно скипнуть";
				b_exit = true;
			}
		}
		else if (floorType == "buff")
		{
			struct buff
			{
				std::string name;
				unsigned int cost;
			};
			/*Комната с бафами*/
			std::map<int, buff> arrbuff;

			arrbuff.insert(std::pair<int, buff>(1, { "Усиление атаки I (3%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(2, { "Усиление атаки II (2%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(3, { "Усиление атаки III (4%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(4, { "Усиление атаки IV (8%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(5, { "Усиление брони I (10%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(6, { "Усиление брони II (5%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(7, { "Усиление брони III (10%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(8, { "Усиление брони IV (20%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(9, { "Защита от магии I (10%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(10, { "Защита от магии II (5%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(11, { "Защита от магии III (10%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(12, { "Защита от магии IV (20%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(13, { "Лечение I (40%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(14, { "Лечение II (40%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(15, { "Лечение III (80%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(16, { "Массовое лечение (40%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(17, { "Повышение энергии I (40%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(18, { "Повышение энергии II (40%)(3)", 3 }));
			arrbuff.insert(std::pair<int, buff>(19, { "Повышение энергии III (80%)(8)", 8 }));
			arrbuff.insert(std::pair<int, buff>(20, { "Повышение энергии (40%)(20)", 20 }));

			arrbuff.insert(std::pair<int, buff>(21, { "Воскрешение (40)", 40 }));

			std::vector<std::string> v;
			std::stringstream ss(m_html);
			pt::ptree root;
			pt::read_json(ss, root);
			for (pt::ptree::value_type &row : root.get_child("results..result.response"))
			{
				std::string id;
				std::string bought;
				if (row.first == "tower")
				{
					for (pt::ptree::value_type &r1 : row.second)
					{
						if (r1.first == "floor")
						{
							for (pt::ptree::value_type &r : r1.second)
							{
								for (pt::ptree::value_type &cell : r.second)
								{
									if (cell.first == "id") id = cell.second.data();
									if (cell.first == "bought") bought = cell.second.data();
								}

								if (bought == "0")
									v.push_back(id);
							}
						}
					}
				}
				else
				{
					if (row.first == "floor")
					{
						for (pt::ptree::value_type &r : row.second)
						{
							for (pt::ptree::value_type &cell : r.second)
							{
								if (cell.first == "id") id = cell.second.data();
								if (cell.first == "bought") bought = cell.second.data();
							}

							if (bought == "0")
								v.push_back(id);
						}
					}
				}
			}

			//Запрос количества доступных черепов
			data = "{\"calls\":[{\"name\":\"inventoryGet\",\"ident\":\"inventoryGet\",\"args\":{}}],\"session\":null}";
			if (sendapi(data))
			{
				//Успешно получил инфу по инвентарю
				/*std::string s_coin = m_network->getParam(m_html, "\"coin\"", "{", "}");
				unsigned int coinSkull = std::stoi(m_network->getParam(s_coin, "\"7\"", ":", "}"));*/
				unsigned int coinSkull = std::stoi(m_network->getVal(m_html, "results..result.response.coin.7", "0"));//7 - это ид валюты черепов
				//Пока что сделаю меню с выбором бафов
				bool b_exit1 = false;
				char sim = 0;

				//для автоматического выбора бафов необходимо оставлять 20 черепов про запас
				//Сначала собираю все бафы с атакой
				std::vector<std::string> t;
				for (int i = (int)(v.size() - 1); i >= 0; --i)
				{
					int num = std::stoi(v[i]);
					//если баф усиление атаки и хватает черепов для покупки
					if ((num >= 1) && (num <= 4) && (coinSkull >= arrbuff[num].cost))
					{
						//покупаю этот баф
						data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[i] + "}}],\"session\":null}";
						if (sendapi(data))
						{
							std::cout << "Купил баф\n";
							//отнимаем от доступных черепов количество потраченных черепов
							coinSkull = coinSkull - arrbuff[num].cost;
						}
						else {
							std::cout << "Запрос не обработался" << std::endl;
							std::cout << "\n\nЗапрос:" << data << std::endl;
							std::cout << "\n\nОтвет:" << m_html << std::endl;
							system("pause");
							b_exit1 = true;
							b_exit = true;
						}
					}
					else
					{
						//если баф не покупается то заполняем временный массив, чтобы оставить только не купленные бафы
						t.push_back(v[i]);
					}
				}
				//обновляем массив чтоб оставить только бафы которые не куплены
				v.swap(t);

				//если количество черепов больше чем 20 то покупаем бафы, чтоб осталось больше 20 черепов
				if ((coinSkull > 20) && !b_exit)
				{
					for (int i = (int)(v.size() - 1); i >= 0; --i)
					{
						int num = std::stoi(v[i]);
						
						if ((num >= 5) && (num <= 12) && ((coinSkull - 20) >= arrbuff[num].cost))
						{
							//покупаю этот баф
							data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[i] + "}}],\"session\":null}";
							if (sendapi(data))
							{
								std::cout << "Купил баф\n";
								//отнимаем от доступных черепов количество потраченных черепов
								coinSkull = coinSkull - arrbuff[num].cost;
							}
							else {
								std::cout << "Запрос не обработался" << std::endl;
								std::cout << "\n\nЗапрос:" << data << std::endl;
								std::cout << "\n\nОтвет:" << m_html << std::endl;
								system("pause");
								b_exit1 = true;
								b_exit = true;
							}
						}
					}
				}
				//если все прошло нормально, то делаю переход на следующий этаж
				if (!b_exit)
				{
					if (sendapi(nextFloor))
					{
						lastevent = "Перешли на этаж " + std::to_string(std::stoi(floorNumber) + 1);
					}
					else 
					{
						std::cout << "Запрос не обработался" << std::endl;
						std::cout << "\n\nЗапрос:" << data << std::endl;
						std::cout << "\n\nОтвет:" << m_html << std::endl;
						system("pause");
						b_exit1 = true;
						b_exit = true;
					}
				}
				//цикл пока не нажму выход или не перейду на следующий этаж
				/*while (!b_exit1)
				{
					sim = 0;
					system("cls");
					std::cout << "Количество черепов: " << coinSkull << std::endl << std::endl;
					for (size_t i = 0; i < v.size(); ++i)
					{
						int num = std::stoi(v[i]);

						std::cout << (i + 1) << " - " << (arrbuff[num].name.empty() ? "Неизвестное усиление" : arrbuff[num].name) << std::endl;
					}
					std::cout << "\nEnter для перехода на следующий этаж\n";
					std::cout << "ESC если есть неизвестное усиление\n";

					sim = _getch();

					switch (sim)
					{
					case '1'://49 - ASCII
					case '2':
					case '3':
						//запрос покупки бафа
						data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[sim - 49] + "}}],\"session\":null}";
						if (sendapi(data))
						{
							std::cout << "Купил баф\n";
							//отнимаем от доступных черепов количество потраченных черепов
							coinSkull = coinSkull - arrbuff[std::stoi(v[sim - 49])].cost;
							//после покупки бафа удаляем элемент
							std::vector<std::string> t;
							for (size_t i = 0; i < v.size(); ++i)
							{
								if ((sim - 49) != i)
									t.push_back(v[i]);
							}
							v.swap(t);
						}
						else {
							std::cout << "Запрос не обработался" << std::endl;
							std::cout << "\n\nЗапрос:" << data << std::endl;
							std::cout << "\n\nОтвет:" << m_html << std::endl;
							system("pause");
							b_exit1 = true;
							b_exit = true;
						}
						break;
					case 13://Кнопка Enter
							//переход в следующую комнату
						if (sendapi(nextFloor))
						{
							lastevent = "Перешли на этаж " + std::to_string(std::stoi(floorNumber) + 1);
						}
						else {
							std::cout << "Запрос не обработался" << std::endl;
							std::cout << "\n\nЗапрос:" << data << std::endl;
							std::cout << "\n\nОтвет:" << m_html << std::endl;
							system("pause");
							b_exit = true;
						}
						b_exit1 = true;
						break;
					case 27://кнопка ESC
						lastevent = "Отменили выбор бафа";
						b_exit1 = true;
						b_exit = true;
						break;
					}
				}*/
			}
			else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:" << data << std::endl;
				std::cout << "\n\nОтвет:" << m_html << std::endl;
				system("pause");
				b_exit = true;
			}
		}
		else if (floorType == "chest")
		{
			//Запрос открытия сундука
			data = "{\"calls\":[{\"name\":\"towerOpenChest\",\"ident\":\"body\",\"args\":{\"num\":" + std::to_string(rand() % 3) + "}}],\"session\":null}";
			if (sendapi(data))
			{
				lastevent = "Собрал сундук на этаже " + floorNumber;
				//собрали сундук и переходим в следующую комнату
				if (sendapi(nextFloor))
				{
					lastevent += "\nПерешли на этаж " + std::to_string(std::stoi(floorNumber) + 1);
				}
				else {
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:" << data << std::endl;
					std::cout << "\n\nОтвет:" << m_html << std::endl;
					system("pause");
					b_exit = true;
				}
			}
			else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:" << data << std::endl;
				std::cout << "\n\nОтвет:" << m_html << std::endl;
				system("pause");
				b_exit = true;
			}
		}
	}
}

void Bot::dungeon()
{
	//запрашиваю информацию о подземелье
	std::string data = "{\"calls\":[{\"name\":\"dungeonGetInfo\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (sendapi(data))
	{
		std::cout << "Успешно запросили информацию о подземелье\n";
	} else {
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_html << std::endl;
		system("pause");
	}

	//Узнаю тип кем надо нападать
	std::string attackerType = m_network->getParam(m_html, "\"attackerType\"", "\"", "\"");

	if (attackerType == "hero")
	{
		//если бой героями
		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[13,5,12,25,2],\"teamNum\":0}}]}";
		if (sendapi(data))
		{
			std::cout << "Начал битву\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
			system("pause");
		}
		//Считываем seed
		std::string seed = m_network->getParam(m_html, "\"seed\"", ":", ",");

		Sleep(5000);

		unsigned int unix_epoch = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		unix_epoch = (unsigned int)floor(unix_epoch);
		std::string l1 = std::to_string(unix_epoch);
		std::string l2 = std::to_string(unix_epoch + 1);
		
		data = "{\"session\":null,\"calls\":[{\"ident\":\"group_0_body\",\"name\":\"stashClient\",\"args\":{\"data\":[{\"params\":{\"windowName\":\"game.view.popup.team::ArenaAttackTeamGatherPopupWithEnemyTeam\",";
		data += "\"timestamp\":" + l1;
		data += "},\"type\":\".client.window.open\"},{\"params\":{\"windowName\":\"game.view.popup.team::ArenaAttackTeamGatherPopupWithEnemyTeam\",";
		data += "\"timestamp\":" + l1;
		data += ",\"buttonName\":\"go\"},\"type\":\".client.button.click\"},{\"params\":{\"windowName\":\"game.mechanics.dungeon.popup.battle::DungeonBattleFloorPopup\",";
		data += "\"timestamp\":" + l1;
		data += ",\"prevActionName\":\".client.button.click\",\"prevButtonName\":\"dungeon_floor\",\"prevWindowName\":\"game.mechanics.dungeon.popup::DungeonScreen\"},\"type\":\".client.window.close\"},{\"params\":{\"windowName\":\"game.view.popup.team::ArenaAttackTeamGatherPopupWithEnemyTeam\",";
		data += "\"timestamp\":" + l1;
		data += "},\"type\":\".client.window.close\"},{\"params\":{\"windowName\":\"game.view.popup.battle::BattlePreloaderPopup\",";
		data += "\"timestamp\":" + l2;
		data += "},\"type\":\".client.window.open\"},{\"params\":{\"windowName\":\"game.view.popup.battle::BattlePreloaderPopup\",";
		data += "\"timestamp\":" + l2;
		data += "},\"type\":\".client.window.close\"}]}}]}";
		if (sendapi(data))
		{
			std::cout << "Отправил события\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
			system("pause");
		}

		Sleep(5000);


		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630},\"2\":{\"energy\":203,\"isDead\":false,\"hp\":232649},\"12\":{\"energy\":200,\"isDead\":false,\"hp\":41745},\"13\":{\"energy\":100,\"isDead\":false,\"hp\":79010},\"5\":{\"energy\":500,\"isDead\":false,\"hp\":70830}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
		if (sendapi(data))
		{
			std::cout << "Окончил битву\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
			system("pause");
		}
	} else if (attackerType == "fire") {
		//если бой героями
		/*data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[13,5,12,25,2],\"teamNum\":0}}]}";
		if (sendapi(data))
		{
			std::cout << "Начал битву\n";
		}
		else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
		}*/
		//Считываем seed
		std::string seed = m_network->getParam(m_html, "\"seed\"", ":", ",");

		Sleep(60000);

		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"4010\":{\"energy\":712,\"isDead\":false,\"hp\":1083343},\"4011\":{\"energy\":400,\"isDead\":false,\"hp\":635294},\"4012\":{\"energy\":400,\"isDead\":false,\"hp\":736886},\"4013\":{\"energy\":900,\"isDead\":false,\"hp\":750450}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
	}
}

void Bot::chestboss()
{
	//Запрос получения информации о боссах
	std::string data = "{\"calls\":[{\"name\":\"bossGetAll\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (sendapi(data))
	{
		std::cout << "Получили информацию о боссах" << std::endl;
		//всего 3 босса, но навсякий случай делаем анализ и создаем массив на каждого босса с его информацией
		struct boss
		{
			boss() {};
			boss(unsigned int id, unsigned int bossLevel, unsigned int chestNum, unsigned int chestId, bool mayRaid)
				:id(id), bossLevel(bossLevel), chestNum(chestNum), chestId(chestId), mayRaid(mayRaid)
			{};

			unsigned int id;
			unsigned int bossLevel;
			unsigned int chestNum;
			unsigned int chestId;

			bool mayRaid;
		};
		std::vector<boss> arrboss;
		//Блок с информацией о боссах
		std::stringstream ss(m_html);
		pt::ptree root;
		pt::read_json(ss, root);
		for (pt::ptree::value_type &row : root.get_child("results..result.response"))
		{
			boss b;
			for (pt::ptree::value_type &cell : row.second)
			{
				if (cell.first == "id")
					b.id = std::stoi(cell.second.data());
				if (cell.first == "bossLevel")
					b.bossLevel = std::stoi(cell.second.data());
				if (cell.first == "chestNum")
					b.chestNum = std::stoi(cell.second.data());
				if (cell.first == "chestId")
					b.chestId = std::stoi(cell.second.data());
				if (cell.first == "mayRaid")
					b.mayRaid = cell.second.data() == "true";
			}
			arrboss.push_back(b);
		}

		//Собираю запрос 
		data = "{\"calls\":[";
		std::string openChest;
		bool first = true;
		for (size_t i = 0; i < arrboss.size(); ++i)
		{
			if (arrboss[i].mayRaid)
			{
				if (!first)
				{
					data += ",";
					openChest += ",";
				}
				else
					first = false;
				data += "{\"name\":\"bossRaid\",\"ident\":\"group_" + std::to_string(i) + "_body\",\"args\":{\"bossId\":" + std::to_string(arrboss[i].id) + "}}";
				openChest += "{\"name\":\"bossOpenChest\",\"ident\":\"group_" + std::to_string(i) + "_body\",\"args\":{\"bossId\":" + std::to_string(arrboss[i].id) + ",\"starmoney\":0,\"amount\":1}}";
			}
		}
		data += ",{\"name\":\"bossGetAll\",\"ident\":\"bossGetAll\",\"args\":{}}],\"session\":null}";

		if (sendapi(data))
		{
			//отправил запрос на активацию боссов
			//теперь отправляю запрос на открытие сундуков
			//Собираю запрос 
			data = "{\"calls\":[" + openChest + "],\"session\":null}";
			if (sendapi(data))
			{
				//Отправил запрос на открытие бесплатных сундуков
				lastevent = "Открыл бесплатные сундуки боссов";
			}
			else
			{
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:\n" << data << std::endl;
				std::cout << "\n\nОтвет:\n" << m_html << std::endl;
				system("pause");
			}
		}
		else
		{
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
			system("pause");
		}
	}
	else
	{
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_html << std::endl;
		system("pause");
	}
}

bool sotrByPower(Hero &l, Hero &r) { return std::stoi(l.get_power()) < std::stoi(r.get_power()); }

void Bot::zeppelin()
{
	//проверяю доступен ли ежедневный ключ
	std::string data = "{\"calls\":[{\"name\":\"zeppelinGiftGet\",\"ident\":\"zeppelinGiftGet\",\"args\":{}}],\"session\":null}";
	//Отправил запрос проверки доступности бесплатного ключа
	if (!sendapi(data))
	{
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_html << std::endl;
		system("pause");
	}
	//Если доступен ежедневный ключ то отправляю запрос на сбор его
	if (m_network->getParam(m_html, "\"available\"", ":", ",") == "true")
	{
		data = "{\"calls\":[{\"name\":\"zeppelinGiftFarm\",\"ident\":\"zeppelinGiftFarm\",\"args\":{}}],\"session\":null}";
		if (sendapi(data))
		{
			lastevent = "Забрал ежедневный ключ";
		}
		else
		{
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_html << std::endl;
			system("pause");
		}
	}
	else
	{
		lastevent = "Ежедневный ключ не доступен";
	}

	//Запрашиваю информацию о экспедициях
	data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (sendapi(data))
	{
		//структура экспедиции
		struct expedition
		{
			expedition() {};
			
			unsigned int id;
			unsigned int slotId;
			unsigned int status;
			unsigned int endTime;
			unsigned int duration;
			unsigned int power;
			unsigned int rarity;
			unsigned int storyId;
			std::vector<int> heroes;
		};
		std::vector<expedition> expeditions;
		//Блок с информацией о экспедициях
		std::stringstream ss(m_html);
		pt::ptree root;
		pt::read_json(ss, root);
		for (pt::ptree::value_type &row : root.get_child("results..result.response"))
		{
			expedition e;
			e.id = std::stoi(row.second.get_child("id").data());
			e.slotId = std::stoi(row.second.get_child("slotId").data());
			e.status = std::stoi(row.second.get_child("status").data());
			e.endTime = std::stoi(row.second.get_child("endTime").data());
			e.duration = std::stoi(row.second.get_child("duration").data());
			e.power = std::stoi(row.second.get_child("power").data());
			e.rarity = std::stoi(row.second.get_child("rarity").data());
			e.storyId = std::stoi(row.second.get_child("storyId").data());

			for (pt::ptree::value_type &r : row.second.get_child("heroes"))
			{
				e.heroes.push_back(std::stoi(r.second.data()));
			}

			expeditions.push_back(e);
		}

		unsigned int unix_epoch = (unsigned int)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		unix_epoch = (unsigned int)floor(unix_epoch);
		unix_epoch &= static_cast<unsigned int>(4.294967295E9);
		//цикл по всем экспедициям
		for (size_t i = 0; i < expeditions.size(); ++i)
		{
			//status = 0 экспедицию можно начинать
			//status = 2 экспедиция уже идет или закончилась но трофеи не собраны
			//status = 3 экспедиция уже пройдена
			//собираем трофеи если экспедиция в статусе 2 и время окончания уже настало
			if ((expeditions[i].status == 2) && (expeditions[i].endTime <= unix_epoch))
			{
				data = "{\"calls\":[{\"name\":\"expeditionFarm\",\"ident\":\"body\",\"args\":{\"expeditionId\":" + std::to_string(expeditions[i].id) + "}}],\"session\":null}";
				if (sendapi(data))
				{
					lastevent += "\nСобрал добычу с экспедиции " + std::to_string(expeditions[i].id);
					expeditions[i].status = 3;
					expeditions[i].endTime = 0;
					expeditions[i].heroes.clear();
				}
				else
				{
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:\n" << data << std::endl;
					std::cout << "\n\nОтвет:\n" << m_html << std::endl;
					system("pause");
				}
			}
		}

		expeditions.clear();
		//Запрашиваю информацию о экспедициях еще раз.
		data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
		if (sendapi(data))
		{
			//Блок с информацией о экспедициях
			std::stringstream ss(m_html);
			pt::ptree root;
			pt::read_json(ss, root);
			for (pt::ptree::value_type &row : root.get_child("results..result.response"))
			{
				expedition e;
				e.id = std::stoi(row.second.get_child("id").data());
				e.slotId = std::stoi(row.second.get_child("slotId").data());
				e.status = std::stoi(row.second.get_child("status").data());
				e.endTime = std::stoi(row.second.get_child("endTime").data());
				e.duration = std::stoi(row.second.get_child("duration").data());
				e.power = std::stoi(row.second.get_child("power").data());
				e.rarity = std::stoi(row.second.get_child("rarity").data());
				e.storyId = std::stoi(row.second.get_child("storyId").data());

				for (pt::ptree::value_type &r : row.second.get_child("heroes"))
				{
					e.heroes.push_back(std::stoi(r.second.data()));
				}

				expeditions.push_back(e);
			}
		}

		//список героев которые доступны для отправки в экспедицию
		std::vector<Hero> h(m_datagame->get_heroes());
		list.clear();
		for (std::vector<Hero>::iterator it = h.begin(); it < h.end(); ++it)
		{
			//Проверяю учавствует ли герой в экспедиции
			bool isAvailable = true;
			for (size_t i = 0; i < expeditions.size(); ++i)
			{
				for (size_t j = 0; j < expeditions[i].heroes.size(); ++j)
				{
					if (expeditions[i].heroes[j] == std::stoi((*it).get_id()))
						isAvailable = false;
				}
			}

			//если герой не учавствует ни в какой экспедиции то он свободный(доступный)
			if (isAvailable)
				list.push_back(*it);
		}
		std::sort(list.begin(), list.end(), sotrByPower);
		maxPowerHero = 0;
		for (std::vector<Hero>::reverse_iterator it = list.rbegin(); it < list.rbegin() + 5; ++it)
		{
			maxPowerHero += std::stoi((*it).get_power());
		}

		//Определяю экспедицию на которую надо отправить героев
		//status - 0 экспедиции которые доступны только с випом
		//status = 1 доступные в данный момент
		//status = 2 которые проходятся или ожидаю сбора добычи
		//status = 3 пройденные
		//подбираю экспедиции со статусом 1 и с силой на которую мне хватает героев
		for (size_t i = 0; i < expeditions.size(); ++i)
		{
			if (expeditions[i].status == 1 && expeditions[i].power <= maxPowerHero)
			{
				//собираем команду с минимальным количеством силы
				currentTeam.clear();
				minPowerTeam.clear();
				currentTeam.resize(5, 0);
				minPowerTeam.resize(5, 0);
				power = expeditions[i].power;

				recursive(0, 0, 0, list.size());
				//если собрали команду то отпраляем ее на экспедицию
				std::string sendHeroes;
				for (std::vector<int>::iterator it = minPowerTeam.begin(); it < minPowerTeam.end(); ++it)
				{
					if (it == minPowerTeam.begin())
						sendHeroes += list[*it].get_id();
					else
						sendHeroes += "," + list[*it].get_id();
				}
				data = "{\"calls\":[";
				data += "{\"name\":\"expeditionSendHeroes\",\"ident\":\"body\",\"args\":{\"expeditionId\":" + std::to_string(expeditions[i].id) + ",\"heroes\":[" + sendHeroes + "]}}";
				data += "],\"session\":null}";

				if (sendapi(data))
				{
					lastevent += "\nОтправил экспедицию";
					//если экспедиция отправилась, то я убираю из списка доступных героев, отправленные герои
					std::vector<Hero> tmp;
					for (size_t i = 0; i < list.size(); ++i)
					{
						auto res = std::find(minPowerTeam.begin(), minPowerTeam.end(), i);
						if (res == minPowerTeam.end())
							tmp.push_back(list[i]);
					}
					list.swap(tmp);//меняю местами векторы
					//сортировка вектора должна была остаться не нарушенной, так что пересчитываю 
					//максимальное количество доступной силы
					maxPowerHero = 0;
					for (std::vector<Hero>::reverse_iterator it = list.rbegin(); it < list.rbegin() + 5; ++it)
					{
						maxPowerHero += std::stoi((*it).get_power());
					}
				}
				else
				{
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:\n" << data << std::endl;
					std::cout << "\n\nОтвет:\n" << m_html << std::endl;
					system("pause");
					//если ошибка то выходим из функции
					return;
				}
			}
		}
	}
	else
	{
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_html << std::endl;
		system("pause");
	}
}

void Bot::recursive(int param1, unsigned int param2, int param3, int param4)
{
	int index = 0;
	Hero hero;
	unsigned int currentPower = 0;
	index = param3;
	while (index < param4)
	{
		currentTeam[param1] = index;
		hero = list[index];
		currentPower = param2 + std::stoi(hero.get_power());
		if (param1 == 4)
		{
			if (currentPower >= power)
			{
				if (currentPower < minPower)
				{
					minPower = currentPower;
					std::copy(currentTeam.begin(), currentTeam.end(), minPowerTeam.begin());
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			recursive(param1 + 1, currentPower, index + 1, param4);
		}
		index++;
	}
}

