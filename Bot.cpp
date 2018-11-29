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
	// �������������� � ����
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

			std::cout << "��������� �������:" << std::endl;
			if (!lastevent.empty())
				std::cout << lastevent << std::endl;
			std::cout << "\n����\n";
			std::cout << "1 - ������� ���������� ������\n";
			std::cout << "2 - ������ �����\n";
			std::cout << "3 - ������ ���������� (� ����������)\n";
			std::cout << "4 - ���� ������ ������ ����������\n";
			std::cout << "5 - ���������\n";
			std::cout << "6 - ��������� �������� ����������� �������\n";
			std::cout << "ESC �����\n";

			paint = false;
		}
		
		if (_kbhit())
		{
			sim = _getwch();
			switch (sim)
			{
			case L'1'://���� ������ ������ 1
					  //���� ���������� ������ true ������ ��� �������
					  //if (m_configbot.freeChest)
				data = "{\"session\":null,\"calls\":[{\"name\":\"chestBuy\",\"ident\":\"body\",\"args\":{\"pack\":false,\"chest\":\"town\",\"free\":true}}]}";
				if (m_connect->Send(data))
				{
					lastevent = "���������� ���������� ������ ��� ������";
				}
				else {
					if (m_connect->Answer().find("error") != std::string::npos)
					{
						lastevent = "���������� ���������� ������ ��� ������";
					} else {
						std::cout << "������ �� �����������" << std::endl;
						std::cout << "\n\n������:" << data << std::endl;
						std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
						system("pause");
					}
				}
				paint = true;
				break;
			case L'2'://���� ������ ������ 2
				tower();//�����
				paint = true;
				break;
			case L'3':
				//dungeon();//����������
				paint = true;
				break;
			case L'4':
				chestboss();//����������
				paint = true;
				break;
			case L'5':
				zeppelin();//���������
				paint = true;
				break;
			case L'6':
				getEnergy();//������� �������
				paint = true;
				break;
			case 27://���� ������ ESC ������� �� �����
				b_exit = true;
				break;
			}
		}
	}
}

void Bot::getEnergy()
{
	std::string text = "���� ���� ������ ����������� �������\n";
	unsigned int interval = 5 * 1000; // 5 ���
	clock_t timer = clock() - interval; // �������� 5 ��� ���� ������ ��� ��� �����
	// ������������� ������ ����� ���� ���������, ���� �� ������ �����
	bool draw = true; // ������������ �����
	while (true)
	{
		if (draw)
		{
			system("cls");
			std::cout << text << std::endl << std::endl;
			std::cout << "������� ESC ���� ��������� ����\n";
			draw = false;
		}
		

		if (_kbhit())
		{
			wchar_t sim = _getwch();
			switch (sim)
			{
			case 27://���� ������ ESC ��������� ����
				return;
				break;
			}
		}
		else
		{
			if ((clock() - timer) > interval)
			{
				timer = clock();
				//��������� ������ �� ��������� ���������� �� �������
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
					//����� ���������� �����
					std::stringstream ss(m_connect->Answer());
					pt::ptree root;
					pt::read_json(ss, root);
					for (pt::ptree::value_type &r : root.get_child("results..result.response"))
					{
						quest q;
						//��������� ������ �� ������� ����� ������� (state=2)
						if ((q.state = r.second.get_child("state").data()) == "2")
						{
							q.id = r.second.get_child("id").data();
							q.progress = r.second.get_child("progress").data();

							quests.push_back(q);
						}
					}
					//���� ������ �� ������ �� ��������� ������� �� ���� ����������� �������
					for (size_t i = 0; i < quests.size(); ++i)
					{
						data = "{\"calls\":[{\"name\":\"questFarm\",\"ident\":\"body\",\"args\":{\"questId\":" + quests[i].id + "}}],\"session\":null}";
						if (m_connect->Send(data))
						{
							text += "������ ������� � ������� " + quests[i].id + "\n";
							draw = true;
						}
					}
				}
				else {
					//�������� ���� Invalid signature �� ����� ���������������
					std::cout << "������ �� �����������" << std::endl;
					std::cout << "\n\n������:" << data << std::endl;
					std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
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
	//���������� ���������� � �����
	std::string data = "{\"calls\":[{\"name\":\"towerGetInfo\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
	{
		std::cout << "������� ��������� ���������� � �����\n";
	} else {
		std::cout << "������ �� �����������" << std::endl;
		std::cout << "\n\n������:" << data << std::endl;
		std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
		system("pause");
		b_exit = true;
	}

	//����� ������� ��������� �� ��������� ����, ���������� �� ����� �������� � ������ �� ������� nextFloor � SkipFloor
	//����� ���� ���� �� ���������� ����� ��� �����, ������� ������ � ������� ��� �� ������ ������ ��� ������ ����
	while (!b_exit)
	{
		//����� ��� �������
		std::string floorType;
		std::string floorNumber;//�������� ���������� �����
		std::string maySkipFloor;//�������� ���������� �����
		floorType.reserve(7);//����� ���� ������ 3 �������� battle,buff,chest ������� ����������� �� ������������� ��������
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
		//������ �� ������� � ��������� �������
		std::string nextFloor = "{\"calls\":[{\"name\":\"towerNextFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";

		if (floorType == "battle")
		{
			//���� ������� ���� ������ ��� ����� ���������� ������ ������� � ���� ��������, �� ������
			if (std::stoi(maySkipFloor) >= std::stoi(floorNumber))
			{
				//������ �� ���� ���
				data = "{\"calls\":[{\"name\":\"towerSkipFloor\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
				if (m_connect->Send(data))
				{
					lastevent = "�������� ��� �� " + floorNumber + " �����";
				}
				else {
					std::cout << "������ �� �����������" << std::endl;
					std::cout << "\n\n������:" << data << std::endl;
					std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
					system("pause");
					b_exit = true;
				}
			}
			else {
				//�������� ���
				//������ �� ������ �����
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[7,13,5,25,2]}}]}";
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[25]}}]}";
				/*if (sendapi(data))
				{
				std::cout << "����� �����" << std::endl;
				} else {
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:\n" << data << std::endl;
				std::cout << "\n\n�����:\n" << m_html << std::endl;
				system("pause");
				}*/

				//������ ����� � 5 ���
				//Sleep(5000);

				//������ �� ����� �����
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630},\"2\":{\"energy\":1000,\"isDead\":false,\"hp\":233755},\"13\":{\"energy\":1000,\"isDead\":false,\"hp\":79010},\"5\":{\"energy\":1000,\"isDead\":false,\"hp\":70830},\"7\":{\"energy\":1000,\"isDead\":false,\"hp\":64347}},\"input\":[\"auto\",0,0,\"auto\"]},\"v\":142,\"defenders\":{\"heroes\":{},\"input\":[]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"stars\":3,\"win\":true}}}]}";
				//data = "{\"session\":null,\"calls\":[{\"name\":\"towerEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630}},\"input\":[\"auto\",0,0,\"auto\"]},\"v\":139,\"defenders\":{\"heroes\":{},\"input\":[]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"stars\":3,\"win\":true}}}]}";
				/*if (sendapi(data))
				{
				std::cout << "�������� �����" << std::endl;
				} else {
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:\n" << data << std::endl;
				std::cout << "\n\n�����:\n" << m_html << std::endl;
				system("pause");
				}*/
				//������ �� ������� �� �� ����
				/*if (sendapi(nextFloor))
				{
				std::cout << "������� �� ��������� ����\n";
				} else {
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:\n" << data << std::endl;
				std::cout << "\n\n�����:\n" << m_html << std::endl;
				system("pause");
				}*/
				//std::cout << "�� ���� �������� ������ ����\n";

				//����� ���� ��� �� ����������
				lastevent = "��� ����� ����������� ������� ����� ��������";
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
			/*������� � ������*/
			std::map<int, buff> arrbuff;

			arrbuff.insert(std::pair<int, buff>(1, { "�������� ����� I (3%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(2, { "�������� ����� II (2%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(3, { "�������� ����� III (4%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(4, { "�������� ����� IV (8%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(5, { "�������� ����� I (10%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(6, { "�������� ����� II (5%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(7, { "�������� ����� III (10%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(8, { "�������� ����� IV (20%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(9, { "������ �� ����� I (10%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(10, { "������ �� ����� II (5%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(11, { "������ �� ����� III (10%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(12, { "������ �� ����� IV (20%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(13, { "������� I (40%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(14, { "������� II (40%)(6)", 6 }));
			arrbuff.insert(std::pair<int, buff>(15, { "������� III (80%)(16)", 16 }));
			arrbuff.insert(std::pair<int, buff>(16, { "�������� ������� (40%)(40)", 40 }));

			arrbuff.insert(std::pair<int, buff>(17, { "��������� ������� I (40%)(1)", 1 }));
			arrbuff.insert(std::pair<int, buff>(18, { "��������� ������� II (40%)(3)", 3 }));
			arrbuff.insert(std::pair<int, buff>(19, { "��������� ������� III (80%)(8)", 8 }));
			arrbuff.insert(std::pair<int, buff>(20, { "��������� ������� (40%)(20)", 20 }));

			arrbuff.insert(std::pair<int, buff>(21, { "����������� (40)", 40 }));

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

			//������ ���������� ��������� �������
			data = "{\"calls\":[{\"name\":\"inventoryGet\",\"ident\":\"inventoryGet\",\"args\":{}}],\"session\":null}";
			if (m_connect->Send(data))
			{
				//������� ������� ���� �� ���������
				unsigned int coinSkull = std::stoi(HttpReader::getVal(m_connect->Answer(), "results..result.response.coin.7", "0"));//7 - ��� �� ������ �������
				//���� ��� ������ ���� � ������� �����
				bool b_exit1 = false;
				char sim = 0;

				//��� ��������������� ������ ����� ���������� ��������� 20 ������� ��� �����
				//������� ������� ��� ���� � ������
				std::vector<std::string> t;
				for (int i = (int)(v.size() - 1); i >= 0; --i)
				{
					int num = std::stoi(v[i]);
					//���� ��� �������� ����� � ������� ������� ��� �������
					if ((num >= 1) && (num <= 4) && (coinSkull >= arrbuff[num].cost))
					{
						//������� ���� ���
						data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[i] + "}}],\"session\":null}";
						if (m_connect->Send(data))
						{
							std::cout << "����� ��� " << arrbuff[num].name << std::endl;
							//�������� �� ��������� ������� ���������� ����������� �������
							coinSkull = coinSkull - arrbuff[num].cost;
						}
						else {
							std::cout << "������ �� �����������" << std::endl;
							std::cout << "\n\n������:" << data << std::endl;
							std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
							system("pause");
							b_exit1 = true;
							b_exit = true;
						}
					}
					else
					{
						//���� ��� �� ���������� �� ��������� ��������� ������, ����� �������� ������ �� ��������� ����
						t.push_back(v[i]);
					}
				}
				//��������� ������ ���� �������� ������ ���� ������� �� �������
				v.swap(t);

				//���� ���������� ������� ������ ��� 20 �� �������� ����, ���� �������� ������ 20 �������
				if ((coinSkull > 20) && !b_exit)
				{
					for (int i = (int)(v.size() - 1); i >= 0; --i)
					{
						int num = std::stoi(v[i]);
						
						if ((num >= 5) && (num <= 12) && ((coinSkull - 20) >= arrbuff[num].cost))
						{
							//������� ���� ���
							data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[i] + "}}],\"session\":null}";
							if (m_connect->Send(data))
							{
								std::cout << "����� ��� " << arrbuff[num].name << std::endl;
								//�������� �� ��������� ������� ���������� ����������� �������
								coinSkull = coinSkull - arrbuff[num].cost;
							}
							else {
								std::cout << "������ �� �����������" << std::endl;
								std::cout << "\n\n������:" << data << std::endl;
								std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
								system("pause");
								b_exit1 = true;
								b_exit = true;
							}
						}
					}
				}
				//���� ��� ������ ���������, �� ����� ������� �� ��������� ����
				if (!b_exit)
				{
					if (m_connect->Send(nextFloor))
					{
						lastevent = "������� �� ���� " + std::to_string(std::stoi(floorNumber) + 1);
					}
					else 
					{
						std::cout << "������ �� �����������" << std::endl;
						std::cout << "\n\n������:" << data << std::endl;
						std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
						system("pause");
						b_exit1 = true;
						b_exit = true;
					}
				}
				//���� ���� �� ����� ����� ��� �� ������� �� ��������� ����
				/*while (!b_exit1)
				{
					sim = 0;
					system("cls");
					std::cout << "���������� �������: " << coinSkull << std::endl << std::endl;
					for (size_t i = 0; i < v.size(); ++i)
					{
						int num = std::stoi(v[i]);

						std::cout << (i + 1) << " - " << (arrbuff[num].name.empty() ? "����������� ��������" : arrbuff[num].name) << std::endl;
					}
					std::cout << "\nEnter ��� �������� �� ��������� ����\n";
					std::cout << "ESC ���� ���� ����������� ��������\n";

					sim = _getch();

					switch (sim)
					{
					case '1'://49 - ASCII
					case '2':
					case '3':
						//������ ������� ����
						data = "{\"calls\":[{\"name\":\"towerBuyBuff\",\"ident\":\"body\",\"args\":{\"buffId\":" + v[sim - 49] + "}}],\"session\":null}";
						if (sendapi(data))
						{
							std::cout << "����� ���\n";
							//�������� �� ��������� ������� ���������� ����������� �������
							coinSkull = coinSkull - arrbuff[std::stoi(v[sim - 49])].cost;
							//����� ������� ���� ������� �������
							std::vector<std::string> t;
							for (size_t i = 0; i < v.size(); ++i)
							{
								if ((sim - 49) != i)
									t.push_back(v[i]);
							}
							v.swap(t);
						}
						else {
							std::cout << "������ �� �����������" << std::endl;
							std::cout << "\n\n������:" << data << std::endl;
							std::cout << "\n\n�����:" << m_html << std::endl;
							system("pause");
							b_exit1 = true;
							b_exit = true;
						}
						break;
					case 13://������ Enter
							//������� � ��������� �������
						if (sendapi(nextFloor))
						{
							lastevent = "������� �� ���� " + std::to_string(std::stoi(floorNumber) + 1);
						}
						else {
							std::cout << "������ �� �����������" << std::endl;
							std::cout << "\n\n������:" << data << std::endl;
							std::cout << "\n\n�����:" << m_html << std::endl;
							system("pause");
							b_exit = true;
						}
						b_exit1 = true;
						break;
					case 27://������ ESC
						lastevent = "�������� ����� ����";
						b_exit1 = true;
						b_exit = true;
						break;
					}
				}*/
			}
			else {
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:" << data << std::endl;
				std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
				system("pause");
				b_exit = true;
			}
		}
		else if (floorType == "chest")
		{
			//������ �������� �������
			data = "{\"calls\":[{\"name\":\"towerOpenChest\",\"ident\":\"body\",\"args\":{\"num\":" + std::to_string(rand() % 3) + "}}],\"session\":null}";
			if (m_connect->Send(data))
			{
				lastevent = "������ ������ �� ����� " + floorNumber;
				//������� ������ � ��������� � ��������� �������
				if (m_connect->Send(nextFloor))
				{
					lastevent += "\n������� �� ���� " + std::to_string(std::stoi(floorNumber) + 1);
				}
				else {
					std::cout << "������ �� �����������" << std::endl;
					std::cout << "\n\n������:" << data << std::endl;
					std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
					system("pause");
					b_exit = true;
				}
			}
			else {
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:" << data << std::endl;
				std::cout << "\n\n�����:" << m_connect->Answer() << std::endl;
				system("pause");
				b_exit = true;
			}
		}
	}
}

void Bot::dungeon()
{
	//���������� ���������� � ����������
	std::string data = "{\"calls\":[{\"name\":\"dungeonGetInfo\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
	{
		std::cout << "������� ��������� ���������� � ����������\n";
	} else {
		std::cout << "������ �� �����������" << std::endl;
		std::cout << "\n\n������:\n" << data << std::endl;
		std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}

	//����� ��� ��� ���� ��������
	std::string attackerType = HttpReader::getParam(m_connect->Answer(), "\"attackerType\"", "\"", "\"");

	if (attackerType == "hero")
	{
		//���� ��� �������
		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[13,5,12,25,2],\"teamNum\":0}}]}";
		if (m_connect->Send(data))
		{
			std::cout << "����� �����\n";
		} else {
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
		//��������� seed
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
			std::cout << "�������� �������\n";
		} else {
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}

		Sleep(5000);


		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"25\":{\"energy\":1000,\"isDead\":false,\"hp\":149630},\"2\":{\"energy\":203,\"isDead\":false,\"hp\":232649},\"12\":{\"energy\":200,\"isDead\":false,\"hp\":41745},\"13\":{\"energy\":100,\"isDead\":false,\"hp\":79010},\"5\":{\"energy\":500,\"isDead\":false,\"hp\":70830}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
		if (m_connect->Send(data))
		{
			std::cout << "������� �����\n";
		} else {
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
	} else if (attackerType == "fire") {
		//���� ��� �������
		/*data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonStartBattle\",\"ident\":\"body\",\"args\":{\"heroes\":[13,5,12,25,2],\"teamNum\":0}}]}";
		if (sendapi(data))
		{
			std::cout << "����� �����\n";
		}
		else {
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_html << std::endl;
		}*/
		//��������� seed
		std::string seed = HttpReader::getParam(m_connect->Answer(), "\"seed\"", ":", ",");

		Sleep(60000);

		data = "{\"session\":null,\"calls\":[{\"name\":\"dungeonEndBattle\",\"ident\":\"body\",\"args\":{\"progress\":[{\"defenders\":{\"heroes\":{},\"input\":[]},\"v\":139,\"attackers\":{\"heroes\":{\"4010\":{\"energy\":712,\"isDead\":false,\"hp\":1083343},\"4011\":{\"energy\":400,\"isDead\":false,\"hp\":635294},\"4012\":{\"energy\":400,\"isDead\":false,\"hp\":736886},\"4013\":{\"energy\":900,\"isDead\":false,\"hp\":750450}},\"input\":[\"auto\",0,0,\"auto\",0,0]},\"b\":0,\"seed\":" + seed + "}],\"result\":{\"win\":true,\"stars\":3}}}]}";
	}
}

void Bot::chestboss()
{
	//������ ��������� ���������� � ������
	std::string data = "{\"calls\":[{\"name\":\"bossGetAll\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
	{
		std::cout << "�������� ���������� � ������" << std::endl;
		//����� 3 �����, �� �������� ������ ������ ������ � ������� ������ �� ������� ����� � ��� �����������
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
		//���� � ����������� � ������
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

		//������� ������ 
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
			//�������� ������ �� ��������� ������
			//������ ��������� ������ �� �������� ��������
			//������� ������ 
			data = "{\"calls\":[" + openChest + "],\"session\":null}";
			if (m_connect->Send(data))
			{
				//�������� ������ �� �������� ���������� ��������
				lastevent = "������ ���������� ������� ������";
			}
			else
			{
				std::cout << "������ �� �����������" << std::endl;
				std::cout << "\n\n������:\n" << data << std::endl;
				std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
				system("pause");
			}
		}
		else
		{
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
	}
	else
	{
		std::cout << "������ �� �����������" << std::endl;
		std::cout << "\n\n������:\n" << data << std::endl;
		std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}
}

bool sotrByPower(Hero &l, Hero &r) { return std::stoi(l.get_power()) < std::stoi(r.get_power()); }

void Bot::zeppelin()
{
	//�������� �������� �� ���������� ����
	std::string data = "{\"calls\":[{\"name\":\"zeppelinGiftGet\",\"ident\":\"zeppelinGiftGet\",\"args\":{}}],\"session\":null}";
	//�������� ������ �������� ����������� ����������� �����
	if (!m_connect->Send(data))
	{
		std::cout << "������ �� �����������" << std::endl;
		std::cout << "\n\n������:\n" << data << std::endl;
		std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
		system("pause");
	}
	//���� �������� ���������� ���� �� ��������� ������ �� ���� ���
	if (HttpReader::getParam(m_connect->Answer(), "\"available\"", ":", ",") == "true")
	{
		data = "{\"calls\":[{\"name\":\"zeppelinGiftFarm\",\"ident\":\"zeppelinGiftFarm\",\"args\":{}}],\"session\":null}";
		if (m_connect->Send(data))
		{
			lastevent = "������ ���������� ����";
		}
		else
		{
			std::cout << "������ �� �����������" << std::endl;
			std::cout << "\n\n������:\n" << data << std::endl;
			std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
			system("pause");
		}
	}
	else
	{
		lastevent = "���������� ���� �� ��������";
	}

	//���������� ���������� � �����������
	data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
	if (m_connect->Send(data))
	{
		//��������� ����������
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
		//���� � ����������� � �����������
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
		//���� �� ���� �����������
		for (size_t i = 0; i < expeditions.size(); ++i)
		{
			//status = 0 ���������� ����� ��������
			//status = 2 ���������� ��� ���� ��� ����������� �� ������ �� �������
			//status = 3 ���������� ��� ��������
			//�������� ������ ���� ���������� � ������� 2 � ����� ��������� ��� �������
			if ((expeditions[i].status == 2) && (expeditions[i].endTime <= unix_epoch))
			{
				data = "{\"calls\":[{\"name\":\"expeditionFarm\",\"ident\":\"body\",\"args\":{\"expeditionId\":" + std::to_string(expeditions[i].id) + "}}],\"session\":null}";
				if (m_connect->Send(data))
				{
					lastevent += "\n������ ������ � ���������� " + std::to_string(expeditions[i].id);
					expeditions[i].status = 3;
					expeditions[i].endTime = 0;
					expeditions[i].heroes.clear();
				}
				else
				{
					std::cout << "������ �� �����������" << std::endl;
					std::cout << "\n\n������:\n" << data << std::endl;
					std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
					system("pause");
				}
			}
		}

		expeditions.clear();
		//���������� ���������� � ����������� ��� ���.
		data = "{\"calls\":[{\"name\":\"expeditionGet\",\"ident\":\"body\",\"args\":{}}],\"session\":null}";
		if (m_connect->Send(data))
		{
			//���� � ����������� � �����������
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

		//������ ������ ������� �������� ��� �������� � ����������
		std::vector<Hero> h(m_datagame->get_heroes());
		list.clear();
		for (std::vector<Hero>::iterator it = h.begin(); it < h.end(); ++it)
		{
			//�������� ���������� �� ����� � ����������
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

			//���� ����� �� ���������� �� � ����� ���������� �� �� ���������(���������)
			if (isAvailable)
				list.push_back(*it);
		}
		std::sort(list.begin(), list.end(), sotrByPower);
		maxPowerHero = 0;
		for (std::vector<Hero>::reverse_iterator it = list.rbegin(); it < list.rbegin() + 5; ++it)
		{
			maxPowerHero += std::stoi((*it).get_power());
		}

		//��������� ���������� �� ������� ���� ��������� ������
		//status - 0 ���������� ������� �������� ������ � �����
		//status = 1 ��������� � ������ ������
		//status = 2 ������� ���������� ��� ������ ����� ������
		//status = 3 ����������
		//�������� ���������� �� �������� 1 � � ����� �� ������� ��� ������� ������
		for (size_t i = 0; i < expeditions.size(); ++i)
		{
			if (expeditions[i].status == 1 && expeditions[i].power <= maxPowerHero)
			{
				//�������� ������� � ����������� ����������� ����
				currentTeam.clear();
				minPowerTeam.clear();
				currentTeam.resize(5, 0);
				minPowerTeam.resize(5, 0);
				power = expeditions[i].power;
				minPower = INT_MAX;

				recursive(0, 0, 0, list.size());
				//���� ������� ������� �� ��������� �� �� ����������
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
					lastevent += "\n�������� ����������";
					//���� ���������� �����������, �� � ������ �� ������ ��������� ������, ������������ �����
					std::vector<Hero> tmp;
					for (size_t i = 0; i < list.size(); ++i)
					{
						auto res = std::find(minPowerTeam.begin(), minPowerTeam.end(), i);
						if (res == minPowerTeam.end())
							tmp.push_back(list[i]);
					}
					list.swap(tmp);//����� ������� �������
					//���������� ������� ������ ���� �������� �� ����������, ��� ��� ������������ 
					//������������ ���������� ��������� ����
					maxPowerHero = 0;
					for (std::vector<Hero>::reverse_iterator it = list.rbegin(); it < list.rbegin() + 5; ++it)
					{
						maxPowerHero += std::stoi((*it).get_power());
					}
				}
				else
				{
					std::cout << "������ �� �����������" << std::endl;
					std::cout << "\n\n������:\n" << data << std::endl;
					std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
					std::copy(minPowerTeam.begin(), minPowerTeam.end(), std::ostream_iterator<int>(std::cout, " "));
					system("pause");
					//���� ������ �� ������� �� �������
					return;
				}
			}
		}
	}
	else
	{
		std::cout << "������ �� �����������" << std::endl;
		std::cout << "\n\n������:\n" << data << std::endl;
		std::cout << "\n\n�����:\n" << m_connect->Answer() << std::endl;
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

