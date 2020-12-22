#include "Map.h"

Map* Map::instance = 0;

Map::Map()
{
		//Load map
		wifstream file("Map.txt");
		wstring line;
		wstring file_contents;
		nMapHeight = 0;
		nMapWidth = 0;

		while (getline(file, line))
		{
			if (nMapWidth < line.length())
				nMapWidth = line.length();
			file_contents += line;
			nMapHeight++;
		}
		map = file_contents;
}

Map* Map::getInstance()
{
	if (!instance)
		instance = new Map;

	return instance;
}