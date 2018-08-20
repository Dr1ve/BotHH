#include "includes.h"
#include "Application.h"



int main()
{
	setlocale(LC_ALL, "rus");
	
	//json parser
	std::string str1;
	str1 = "{\"date\":1532084243.3765881,\"results\":[{\"ident\":\"body\",\"result\":{\"response\":{\"tower\":{\"userId\":\"2762012\",\"teamLevel\":\"117\",\"points\":13350,\"maySkipFloor\":\"26\",\"floorNumber\":18,\"floorType\":\"buff\",\"states\":{\"heroes\":{\"2\":{\"hp\":255695,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":255695},\"20\":{\"hp\":22700,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":22700},\"7\":{\"hp\":78967,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":78967},\"6\":{\"hp\":17772,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":17772},\"4\":{\"hp\":29140,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":29140},\"12\":{\"hp\":41745,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":41745},\"13\":{\"hp\":80105,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":80105},\"14\":{\"hp\":23960,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":23960},\"1\":{\"hp\":22902,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":22902},\"38\":{\"hp\":18530,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":18530},\"15\":{\"hp\":36930,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":36930},\"28\":{\"hp\":19663,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":19663},\"17\":{\"hp\":19205,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":19205},\"21\":{\"hp\":27290,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":27290},\"3\":{\"hp\":14820,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":14820},\"36\":{\"hp\":23067,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":23067},\"25\":{\"hp\":150310,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":150310},\"8\":{\"hp\":17260,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":17260},\"19\":{\"hp\":10900,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":10900},\"5\":{\"hp\":71390,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":71390},\"31\":{\"hp\":7700,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":7700}},\"mercenaries\":[]},\"effects\":{\"percentBuff_allAttacks\":13,\"percentBuff_goldBonus\":50},\"floor\":[{\"id\":10,\"bought\":0},{\"id\":3,\"bought\":0},{\"id\":8,\"bought\":0}],\"reward\":{\"4\":{\"fragmentGear\":{\"94\":\"3\"}},\"8\":{\"gold\":83100},\"10\":{\"fragmentGear\":{\"95\":\"4\"}},\"14\":{\"coin\":{\"3\":\"75\"}},\"16\":{\"coin\":{\"3\":\"75\"}}},\"mayBuySkip\":true,\"mayFullSkip\":false,\"skipBought\":false,\"chestSkip\":false,\"fullSkipCost\":{\"starmoney\":1150}},\"reward\":{\"towerPoint\":1890,\"coin\":{\"7\":15}}}}}]}";
	//str1 = "{\"date\":1532082843.8434319,\"results\":[{\"ident\":\"body\",\"result\":{\"response\":{\"userId\":\"2762012\",\"teamLevel\":\"117\",\"points\":\"1200\",\"maySkipFloor\":\"26\",\"floorNumber\":\"2\",\"floorType\":\"buff\",\"states\":{\"heroes\":{\"2\":{\"hp\":255695,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":255695},\"20\":{\"hp\":22700,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":22700},\"7\":{\"hp\":78967,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":78967},\"6\":{\"hp\":17772,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":17772},\"4\":{\"hp\":29140,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":29140},\"12\":{\"hp\":41745,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":41745},\"13\":{\"hp\":80105,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":80105},\"14\":{\"hp\":23960,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":23960},\"1\":{\"hp\":22902,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":22902},\"38\":{\"hp\":18530,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":18530},\"15\":{\"hp\":36930,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":36930},\"28\":{\"hp\":19663,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":19663},\"17\":{\"hp\":19205,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":19205},\"21\":{\"hp\":27290,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":27290},\"3\":{\"hp\":14820,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":14820},\"36\":{\"hp\":23067,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":23067},\"25\":{\"hp\":150310,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":150310},\"8\":{\"hp\":17260,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":17260},\"19\":{\"hp\":10900,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":10900},\"5\":{\"hp\":71390,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":71390},\"31\":{\"hp\":7700,\"energy\":\"1000\",\"isDead\":false,\"maxHp\":7700}},\"mercenaries\":[]},\"effects\":{\"percentBuff_goldBonus\":50},\"floor\":[{\"id\":1,\"bought\":0},{\"id\":9,\"bought\":0},{\"id\":13,\"bought\":0}],\"reward\":[],\"mayBuySkip\":true,\"mayFullSkip\":false,\"skipBought\":false,\"chestSkip\":false,\"fullSkipCost\":{\"starmoney\":1150}}}}]}";
	std::stringstream ss(str1);
	pt::ptree root;
	pt::read_json(ss, root);
	

	Application app;
	
	app.Loop();
}
