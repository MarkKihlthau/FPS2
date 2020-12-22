#pragma once
#include <fstream>
#include <string>

using namespace std;

class Map
{
public:
	static Map* getInstance();

	int get_nMapHeight()
	{
		return this->nMapHeight;
	}

	int get_nMapWidth()
	{
		return this->nMapWidth;
	}

	wstring get_map()
	{
		return this->map;
	}

private:
	Map();

	static Map* instance;
	int nMapWidth;
	int nMapHeight;
	wstring map;

};