#include "Bot.h"

#include "HttpReader.h"
#include "DataGame.h"
#include "ConnectionGame.h"
#include <ctime>
#include <conio.h>
#include <map>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

Bot::Bot()
{
	m_configbot.freeChest = false;

	m_datagame = new DataGame;
	// Присоединяемся к игре
	m_connect = new ConnectionGame(m_datagame);
}

Bot::~Bot()
{
	delete m_datagame;
	delete m_connect;
}

void Bot::Loop()
{
	bool b_exit = false;
	wchar_t sim = 0;

	system("cls");

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
				if (m_connect->Send(data))
				{
					lastevent = "Ежедневный бесплатный сундук был собран";
				}
				else {
					if (m_connect->Answer().find("error") != std::string::npos)
					{
						lastevent = "Ежедневный бесплатный сундук был собран";
					} else {
						std::cout << "Запрос не обработался" << std::endl;
						std::cout << "\n\nЗапрос:" << data << std::endl;
						std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
	unsigned int interval = 5 * 1000; // 5 сек
	clock_t timer = clock() - interval; // отнимаем 5 сек чтоб первый соб был сразу
	// Перерисовываю только когда есть изменение, чтоб не мерцал текст
	bool draw = true; // перерисовать текст
	while (true)
	{
		if (draw)
		{
			system("cls");
			std::cout << text << std::endl << std::endl;
			std::cout << "Нажмите ESC чтоб завершить сбор\n";
			draw = false;
		}
		

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
			if ((clock() - timer) > interval)
			{
				timer = clock();
				//отправляю запрос на получение информации по квестам
				std::string data = "{\"calls\":[{\"name\":\"questGetAll\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
				if (m_connect->Send(data))
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
					std::stringstream ss(m_connect->Answer());
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
						if (m_connect->Send(data))
						{
							text += "собрал награду с задания " + quests[i].id + "\n";
							draw = true;
						}
					}
				}
				else {
					//Проверяю если Invalid signature то делаю переподключение
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:" << data << std::endl;
					std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
					system("pause");
					return;
				}
			}
		}
	}
}

void Bot::tower()
{
	bool b_exit = false;
	//запрашиваю информацию о башне
	std::string data = "{\"calls\":[{\"name\":\"towerGetInfo\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
	{
		std::cout << "Успешно запросили информацию о башне\n";
	} else {
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:" << data << std::endl;
		std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
		system("pause");
		b_exit = true;
	}

	//После каждого переходна на следующий этаж, информация об этаже приходит в ответе на запросы nextFloor и SkipFloor
	//Делаю цикл пока не закончатся этажи для скипа, выпадет ошибка в запросе или не нажмут отмену при выборе бафа
	while (!b_exit)
	{
		//Узнаю тип комнаты
		std::string floorType;
		std::string floorNumber;//максимум двузначное число
		std::string maySkipFloor;//максимум двузначное число
		floorType.reserve(7);//может быть только 3 значения battle,buff,chest поэтому резервируем по максимальному значению
		floorNumber.reserve(3);
		maySkipFloor.reserve(3);
		std::stringstream ss1(m_connect->Answer());
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
		//Запрос на переход в следующую комнату
		std::string nextFloor = "{\"calls\":[{\"name\":\"towerNextFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";

		if (floorType == "battle")
		{
			//если текущий этаж меньше или равен количеству этажей которые я могу скипнуть, то скипаю
			if (std::stoi(maySkipFloor) >= std::stoi(floorNumber))
			{
				//Запрос на скип боя
				data = "{\"calls\":[{\"name\":\"towerSkipFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
				if (m_connect->Send(data))
				{
					lastevent = "Скипнули бой на " + floorNumber + " этаже";
				}
				else {
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:" << data << std::endl;
					std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
			std::stringstream ss(m_connect->Answer());
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
			if (m_connect->Send(data))
			{
				//Успешно получил инфу по инвентарю
				unsigned int coinSkull = std::stoi(HttpReader::getVal(m_connect->Answer(), "results..result.response.coin.7", "0"));//7 - это ид валюты черепов
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
						if (m_connect->Send(data))
						{
							std::cout << "Купил баф " << arrbuff[num].name << std::endl;
							//отнимаем от доступных черепов количество потраченных черепов
							coinSkull = coinSkull - arrbuff[num].cost;
						}
						else {
							std::cout << "Запрос не обработался" << std::endl;
							std::cout << "\n\nЗапрос:" << data << std::endl;
							std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
							if (m_connect->Send(data))
							{
								std::cout << "Купил баф " << arrbuff[num].name << std::endl;
								//отнимаем от доступных черепов количество потраченных черепов
								coinSkull = coinSkull - arrbuff[num].cost;
							}
							else {
								std::cout << "Запрос не обработался" << std::endl;
								std::cout << "\n\nЗапрос:" << data << std::endl;
								std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
					if (m_connect->Send(nextFloor))
					{
						lastevent = "Перешли на этаж " + std::to_string(std::stoi(floorNumber) + 1);
					}
					else 
					{
						std::cout << "Запрос не обработался" << std::endl;
						std::cout << "\n\nЗапрос:" << data << std::endl;
						std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
				std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
				system("pause");
				b_exit = true;
			}
		}
		else if (floorType == "chest")
		{
			//Запрос открытия сундука
			data = "{\"calls\":[{\"name\":\"towerOpenChest\",\"ident\":\"body\",\"args\":{\"num\":" + std::to_string(rand() % 3) + "}}],\"session\":null}";
			if (m_connect->Send(data))
			{
				lastevent = "Собрал сундук на этаже " + floorNumber;
				//собрали сундук и переходим в следующую комнату
				if (m_connect->Send(nextFloor))
				{
					lastevent += "\nПерешли на этаж " + std::to_string(std::stoi(floorNumber) + 1);
				}
				else {
					std::cout << "Запрос не обработался" << std::endl;
					std::cout << "\n\nЗапрос:" << data << std::endl;
					std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
					system("pause");
					b_exit = true;
				}
			}
			else {
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:" << data << std::endl;
				std::cout << "\n\nОтвет:" << m_connect->Answer() << std::endl;
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
	if (m_connect->Send(data))
	{
		std::cout << "Успешно запросили информацию о подземелье\n";
	} else {
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}

	//Узнаю тип кем надо нападать
	std::string attackerType = HttpReader::getParam(m_connect->Answer(), "\"attackerType\"", "\"", "\"");

	if (attackerType == "hero")
	{
		//если бой героями
		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[13,5,12,25,2],\"teamNum\":0}}]}";
		if (m_connect->Send(data))
		{
			std::cout << "Начал битву\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
		//Считываем seed
		std::string seed = HttpReader::getParam(m_connect->Answer(), "\"seed\"", ":", ",");

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
		if (m_connect->Send(data))
		{
			std::cout << "Отправил события\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}

		Sleep(5000);


		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630},\"2\":{\"energy\":203,\"isDead\":false,\"hp\":232649},\"12\":{\"energy\":200,\"isDead\":false,\"hp\":41745},\"13\":{\"energy\":100,\"isDead\":false,\"hp\":79010},\"5\":{\"energy\":500,\"isDead\":false,\"hp\":70830}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
		if (m_connect->Send(data))
		{
			std::cout << "Окончил битву\n";
		} else {
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
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
		std::string seed = HttpReader::getParam(m_connect->Answer(), "\"seed\"", ":", ",");

		Sleep(60000);

		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"4010\":{\"energy\":712,\"isDead\":false,\"hp\":1083343},\"4011\":{\"energy\":400,\"isDead\":false,\"hp\":635294},\"4012\":{\"energy\":400,\"isDead\":false,\"hp\":736886},\"4013\":{\"energy\":900,\"isDead\":false,\"hp\":750450}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
	}
}

void Bot::chestboss()
{
	//Запрос получения информации о боссах
	std::string data = "{\"calls\":[{\"name\":\"bossGetAll\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
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
		std::stringstream ss(m_connect->Answer());
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

		if (m_connect->Send(data))
		{
			//отправил запрос на активацию боссов
			//теперь отправляю запрос на открытие сундуков
			//Собираю запрос 
			data = "{\"calls\":[" + openChest + "],\"session\":null}";
			if (m_connect->Send(data))
			{
				//Отправил запрос на открытие бесплатных сундуков
				lastevent = "Открыл бесплатные сундуки боссов";
			}
			else
			{
				std::cout << "Запрос не обработался" << std::endl;
				std::cout << "\n\nЗапрос:\n" << data << std::endl;
				std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
				system("pause");
			}
		}
		else
		{
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
	}
	else
	{
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}
}

bool sotrByPower(Hero &l, Hero &r) { return std::stoi(l.get_power()) < std::stoi(r.get_power()); }

void Bot::zeppelin()
{
	//проверяю доступен ли ежедневный ключ
	std::string data = "{\"calls\":[{\"name\":\"zeppelinGiftGet\",\"ident\":\"zeppelinGiftGet\",\"args\":{}}],\"session\":null}";
	//Отправил запрос проверки доступности бесплатного ключа
	if (!m_connect->Send(data))
	{
		std::cout << "Запрос не обработался" << std::endl;
		std::cout << "\n\nЗапрос:\n" << data << std::endl;
		std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}
	//Если доступен ежедневный ключ то отправляю запрос на сбор его
	if (HttpReader::getParam(m_connect->Answer(), "\"available\"", ":", ",") == "true")
	{
		data = "{\"calls\":[{\"name\":\"zeppelinGiftFarm\",\"ident\":\"zeppelinGiftFarm\",\"args\":{}}],\"session\":null}";
		if (m_connect->Send(data))
		{
			lastevent = "Забрал ежедневный ключ";
		}
		else
		{
			std::cout << "Запрос не обработался" << std::endl;
			std::cout << "\n\nЗапрос:\n" << data << std::endl;
			std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
	}
	else
	{
		lastevent = "Ежедневный ключ не доступен";
	}

	//Запрашиваю информацию о экспедициях
	data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
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
		std::stringstream ss(m_connect->Answer());
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
				if (m_connect->Send(data))
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
					std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
					system("pause");
				}
			}
		}

		expeditions.clear();
		//Запрашиваю информацию о экспедициях еще раз.
		data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
		if (m_connect->Send(data))
		{
			//Блок с информацией о экспедициях
			std::stringstream ss(m_connect->Answer());
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
				if (expeditions[i].status == 2)
				{
					for (size_t j = 0; j < expeditions[i].heroes.size(); ++j)
					{
						if (expeditions[i].heroes[j] == std::stoi((*it).get_id()))
							isAvailable = false;
					}
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
				minPower = INT_MAX;

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

				if (m_connect->Send(data))
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
					std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
					std::copy(minPowerTeam.begin(), minPowerTeam.end(), std::ostream_iterator<int>(std::cout, " "));
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
		std::cout << "\n\nОтвет:\n" << m_connect->Answer() << std::endl;
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

