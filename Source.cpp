//Modified from existing code found here: https://github.com/OneLoneCoder/CommandLineFPS
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <Windows.h>
#include <chrono>
#include <time.h>
#include <cstdlib>
#include "Item.h"
#include "Weapon.h"
#include "Enemy.h"
using namespace std;

enum Type {Healthpack, Weapon, Special, Ammo};

const int nScreenWidth = 120; //Screen Size
const int nScreenHeight = 40;

float fPlayerX = 3.0f; //player X position
float fPlayerY = 14.0f; //player Y position
float fPlayerA = 1.5f; //player look Angle

int nMapHeight = 0; //Map Size
int nMapWidth = 0;

const float fFOV = 3.14159 / 4; //field of view
const float fDepth = 16.0f;
const float fSpeed = 5.0f; //player movement speed

void Controls(float, wstring, wchar_t*);
void Environment(wchar_t*, wstring);
//void MiniMap(wchar_t*);
void TacticalMap(wchar_t*);
void CalculateRows(wchar_t*, wstring&, int, int, int);
void DirectionalArrow(wstring&, int, int);
void PrintTacticalMap(wchar_t*, wstring&, int, int);
void UserInterface(wchar_t*);
void WeaponArt(wchar_t*);
void MoveEnemy(int);
void EnemyMoveHandler(int id);
void SpawnEnemy(int);
void LoadItems();
void PlayerPickup(int, Item*);
void ReloadMap();
void PlaceItem(int, int, Type);
void PlaceAllItems();
bool SearchItems(int, int&, int&);
int FindHitEnemyID(int, int);

//Items
Item* itemTypes[1000][1000]; //array for item objects
int items[1000][1000]; //holds the integer type value for item
int itemPosition[1000][1000]; //array for item map position values

int HitX; //X location of gunshot hit
int HitY; //Y location of gunshot hit
bool ReadyShot;

Enemy* Enemies[1000];
int CurrentSpawned = 0;

//timers
chrono::time_point<chrono::system_clock> tBegin = chrono::system_clock::now();
chrono::time_point<chrono::system_clock> tShoot = chrono::system_clock::now();
chrono::time_point<chrono::system_clock> tAnimation = chrono::system_clock::now();
chrono::time_point<chrono::system_clock> tNewSpawn = chrono::system_clock::now();
chrono::time_point<chrono::system_clock> tWeaponSwitch = chrono::system_clock::now();

//Player Stats
int health = 50;
wstring wname = L"";
int damage = 0;
int capacity = 0;
int reload = 0;
int kill_count = 49;
Item* WeaponInventory[2] = { Item::Create("Weapon"), Item::Create("Weapon") };
Item* WeaponInHand = Item::Create("Weapon");

wstring map; //string that holds the map
wstring map_copy; //string that holds a copy of the original map
//wstring minimap; //string that holds minimap display

int main()
{
	//Create Screen Buffer
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	srand(time(NULL)); //For random number generation

	//Load map
	wifstream file("Map2.txt");
	wstring line;
	wstring file_contents;
	while (getline(file, line))
	{
		nMapWidth = line.length();
		file_contents += line;
		nMapHeight++;
	}
	map = file_contents;
	map_copy = map;

	PlaceAllItems(); //places all items on the map based on map string

	auto TimeP1 = chrono::system_clock::now();
	auto TimeP2 = chrono::system_clock::now();
	auto TimeP3 = chrono::system_clock::now();
	float fElapsedGameTime = 0;

	//Game Loop
	while (1)
	{
		//Create Clock for each Game Loop
		TimeP2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = TimeP2 - TimeP1;
		TimeP1 = TimeP2;
		float fElapsedTime = elapsedTime.count();

		//Controls
		Controls(fElapsedTime, map, screen);

		//Picks up items based on player position
		LoadItems();

		if (((tNewSpawn - tBegin) + chrono::seconds(3) <= chrono::system_clock::now() - tBegin) && CurrentSpawned < 100)
			SpawnEnemy(CurrentSpawned);
		
		//Handles enemy movement and attacks
		ReloadMap();
		for (int i = 0; i < CurrentSpawned; i++)
			EnemyMoveHandler(i);
		
		//Dispay walls and shading
		Environment(screen, map);

		//Display Weapon Art
		WeaponArt(screen);

		//Display Stats
		swprintf_s(screen, 50, L"X=%3.2f, Y=%3.2f, A%3.2f FPS=%3.2f, Spawns:%d", 
			fPlayerX, fPlayerY, cosf(fPlayerA), 1.0f / fElapsedTime, CurrentSpawned);
		
		//Display User Interface
		UserInterface(screen);

		if (kill_count >= 50)
			WeaponInventory[1]->ChangeState(WeaponInventory[1], Item::Super_Pistol);

		//Display Frame 
		screen[nScreenWidth * nScreenHeight - 1] = '/0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		if (health <= 0)
			break;
		
	} //end game loop

	auto TimeP4 = chrono::system_clock::now();
	chrono::duration<float> elapsedGameTime = TimeP4 - TimeP3;
	fElapsedGameTime = elapsedGameTime.count();
	cout << "You died.\n";
	cout << "Your kills: " << kill_count << endl;
	cout << "Your in-game time: " << fElapsedGameTime << " seconds.\n";
	cout << "Close window to exit.";
	char exit;
	while (1)
	{
		cin >> exit;
	}
	
	return 0;
}


void Controls(float fElapsedTime, wstring map, wchar_t* screen)
{
	//Handle Rotation
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
	{
		fPlayerA -= (3.0f) * fElapsedTime;
	}
	if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
	{
		fPlayerA += (3.0f) * fElapsedTime;
	}

	//Handle Forward and Backward
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
	{
		fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
		fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;

		//Collision Detection
		if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
		}
	}
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
	{
		fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
		fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;

		//Collision Detection
		if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
		}
	}

	//Handle Strafing
	if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
	{
		fPlayerX += sinf(fPlayerA - 90) * fSpeed * fElapsedTime;
		fPlayerY += cosf(fPlayerA - 90) * fSpeed * fElapsedTime;

		//Collision Detection
		if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
		{
			fPlayerX -= sinf(fPlayerA - 90) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA - 90) * fSpeed * fElapsedTime;
		}
	}
	if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
	{
		fPlayerX += sinf(fPlayerA + 90) * fSpeed * fElapsedTime;
		fPlayerY += cosf(fPlayerA + 90) * fSpeed * fElapsedTime;

		//Collision Detection
		if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
		{
			fPlayerX -= sinf(fPlayerA + 90) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA + 90) * fSpeed * fElapsedTime;
		}
	}

	//Handle Weapon Switch
	if (GetKeyState((unsigned short)'F') & 0x8000)
	{
		if ((tWeaponSwitch - tBegin) + chrono::milliseconds(200) <= chrono::system_clock::now() - tBegin)
		{
			if (WeaponInHand == WeaponInventory[0])
				WeaponInHand = WeaponInventory[1];
			else
				WeaponInHand = WeaponInventory[0];

			wname = WeaponInHand->get_name();
			damage = WeaponInHand->get_damage();
			capacity = WeaponInHand->get_capacity();
			reload = WeaponInHand->get_reload();

			tWeaponSwitch = chrono::system_clock::now();
		}
	}

	//Handle Shooting
	if ((tShoot - tBegin) + chrono::milliseconds(500) <= chrono::system_clock::now() - tBegin)
		ReadyShot = true;
	else
		ReadyShot = false;
	
	if (GetAsyncKeyState((unsigned short)' ') & 0x8000)
	{
		if (capacity > 0 && ReadyShot)
		{
			capacity = capacity - 1;
			
			float bullet = fPlayerA;
			float fDistanceTo = 0;
			bool bHitWall = false;
			bool bHitEnemy = false;
			bool bBoundary = false;

			float fEyeX = sinf(bullet); //Unit Vector for ray
			float fEyeY = cosf(bullet);

			while ((!bHitWall || !bHitEnemy) && fDistanceTo < fDepth)
			{
				fDistanceTo += 0.1f;

				HitX = (int)(fPlayerX + fEyeX * fDistanceTo); //works if walls are on integer boundries
				HitY = (int)(fPlayerY + fEyeY * fDistanceTo);

				//Test if ray is out of bounds
				if (HitX < 0 || HitX >= nMapWidth || HitY < 0 || HitY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceTo = fDepth;
				}
				else
				{
					//Ray is in bounds, but has hit wall
					if (map[HitY * nMapWidth + HitX] == '#')
					{
						bHitWall = true;
					}
					//Ray is in bounds, but has hit enemy
					else if (map[HitY * nMapWidth + HitX] == 'X')
					{
						bHitEnemy = true;
						//hit_enemy = 1;
						int id = FindHitEnemyID(HitX, HitY);
						Enemies[id]->hit(damage);
						if (Enemies[id]->alive() == false)
							kill_count++;
						break;

					}
				}
			}
			tShoot = chrono::system_clock::now();
			tAnimation = chrono::system_clock::now();
		}
	}

	//Handle Reload
	if (GetAsyncKeyState((unsigned short)'R') & 0x8000)
	{
		capacity = WeaponInHand->get_reload();
	}
}

void Environment(wchar_t* screen, wstring map)
{
	//Ray Casting
	for (int x = 0; x < nScreenWidth; x++)
	{
		// For each column, calculate the projected ray angle into the world
		float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

		float fDistanceTo = 0;
		bool bHitWall = false;
		bool bBoundary = false;

		float fEyeX = sinf(fRayAngle); //Unit Vector for ray
		float fEyeY = cosf(fRayAngle);

		while (!bHitWall && fDistanceTo < fDepth)
		{
			fDistanceTo += 0.1f;

			int nTestX = (int)(fPlayerX + fEyeX * fDistanceTo); //works if walls are on integer boundries
			int nTestY = (int)(fPlayerY + fEyeY * fDistanceTo);

			//Test if ray is out of bounds
			if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
			{
				bHitWall = true;
				fDistanceTo = fDepth;
			}
			else
			{
				//Ray is in bounds, but has hit wall
				if (map[nTestY * nMapWidth + nTestX] == '#')
				{
					bHitWall = true;

					vector <pair<float, float>> p;

					for (int tx = 0; tx < 2; tx++)
						for (int ty = 0; ty < 2; ty++)
						{
							float vy = (float)nTestY + ty - fPlayerY;
							float vx = (float)nTestX + tx - fPlayerX;
							float d = sqrt(vx*vx + vy*vy);
							float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
							p.push_back(make_pair(d, dot));
						}

					sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });

					float fBound = 0.01;
					if (acos(p.at(0).second) < fBound) bBoundary = true;
					if (acos(p.at(1).second) < fBound) bBoundary = true;
					if (acos(p.at(2).second) < fBound) bBoundary = true;
				}

			} //end else

		}

		//Calculate distance to ceiling and floor
		int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceTo);
		int nFloor = nScreenHeight - nCeiling;

		short nShadeWall = ' ';
		short nShadeFloor = ' ';

		if (fDistanceTo <= fDepth / 4.0f)		nShadeWall = 0x2588; //very close
		else if (fDistanceTo < fDepth / 3.0f)	nShadeWall = 0x2593;
		else if (fDistanceTo < fDepth / 2.0f)	nShadeWall = 0x2592;
		else if (fDistanceTo < fDepth)			nShadeWall = 0x2591;
		else									nShadeWall = ' ';	//too far to see

		if (bBoundary)							nShadeWall = ' ';	//blackout

		for (int y = 0; y < nScreenHeight; y++)
		{
			if (y < nCeiling) //ceiling
			{
				screen[y*nScreenWidth + x] = ' ';
			}
			else if (y > nCeiling && y <= nFloor) //wall
			{
				screen[y*nScreenWidth + x] = nShadeWall;
			}
			else //floor
			{
				float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / (float)(nScreenHeight / 2.0f));

				if (b < 0.25)		nShadeFloor = '#';
				else if (b < 0.5)	nShadeFloor = 'x';
				else if (b < 0.75)	nShadeFloor = '-';
				else if (b < 0.9)	nShadeFloor = '.';
				else				nShadeFloor = ' ';
				screen[y*nScreenWidth + x] = nShadeFloor;
			}
		}
	}

}

/*void MiniMap(wchar_t* screen)
{
	map = L""; //reset map
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#......####....#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#"; //7
	map += L"#...######.....#";
	map += L"#..............#";
	map += L"#........#######";
	map += L"#............H.#";
	map += L"#..............#";
	map += L"#############..#";
	map += L"#.........H...W#";
	map += L"################"; //15

	for (int nx = 0; nx < nMapWidth; nx++) //draw map to screen
		for (int ny = 0; ny < nMapWidth; ny++)
		{
			screen[(ny + 1)*nScreenWidth + nx] = map[ny * nMapWidth + (nMapWidth - nx - 1)];
		}
	
	if (items[10][14] == 0) //Healthpack
		screen[1805] = '.';
	if (items[13][11] == 0) //Healthpack
		screen[1442] = '.';
	if (items[14][14] == 0) //Weapon
		screen[1801] = '.';

	screen[((int)fPlayerY + 1) * nScreenWidth + (int)(nMapWidth - fPlayerX)] = 'P';

	if (victory == false)
		screen[(EnemyY + 1) * nScreenWidth + (nMapWidth - EnemyX)] = 'X';
	else
	{
		screen[(EnemyY + 1) * nScreenWidth + (nMapWidth - EnemyX)] = '.';
	}	
	
	for (int nx = 0; nx < nMapWidth; nx++)
		for (int ny = 0; ny < nMapWidth; ny++)
		{
			map[ny * nMapWidth + (nMapWidth - nx - 1)] = screen[(ny + 1)*nScreenWidth + nx];
		}
	

	
}*/

void TacticalMap(wchar_t* screen)
{
	int nTacticalMapWidth = 13;
	int nTacticalMapHeight = 9;

	wstring TacticalMap = L"";
	for (int i = 0; i <= nTacticalMapHeight; i++)
	{
		for (int j = 0; j < nTacticalMapWidth; j++)
			TacticalMap += L"*";
	}

	int nMapCenterRow = (nTacticalMapHeight / 2) + 1;
	int nFromCenter = (nTacticalMapHeight / 2);

	CalculateRows(screen, TacticalMap, nTacticalMapWidth, nMapCenterRow, 0);
	for (int i = 1; i <= nFromCenter; i++)
	{
		CalculateRows(screen, TacticalMap, nTacticalMapWidth, (nMapCenterRow + i), i);
		CalculateRows(screen, TacticalMap, nTacticalMapWidth, (nMapCenterRow - i), (-1 * i));
	}
	DirectionalArrow(TacticalMap, nTacticalMapWidth, nTacticalMapHeight);
	PrintTacticalMap(screen, TacticalMap, nTacticalMapWidth, nTacticalMapHeight);
}

void CalculateRows(wchar_t* screen, wstring &TacticalMap, int nTacticalMapWidth, int nRow, int nFromCenter)
{
	int currpos = (nMapWidth * (int)fPlayerY) + (int)fPlayerX;
	map[currpos] = 'P';

	if ((currpos + nFromCenter * nMapWidth) > 0 && (currpos + nFromCenter * nMapWidth) < nMapWidth * nMapHeight)
	{
		int pos = (nMapWidth * (int)fPlayerY) + (int)fPlayerX + nFromCenter * nMapWidth;
		int start = nMapWidth * (int)fPlayerY + nFromCenter * nMapWidth;
		int end = (nMapWidth * (int)fPlayerY) + nMapWidth - 1 + nFromCenter * nMapWidth;

		int val1 = pos - start;
		int val2 = end - pos;

		if (val1 > (nTacticalMapWidth / 2))
			for (int i = 1; i <= (nTacticalMapWidth / 2); i++)
				TacticalMap[((nTacticalMapWidth / 2) + (nRow * nTacticalMapWidth)) - i] = map[pos - i];
		else
			for (int i = 1; i <= val1; i++)
				TacticalMap[((nTacticalMapWidth / 2) + (nRow * nTacticalMapWidth)) - i] = map[pos - i];

		if (val2 > (nTacticalMapWidth / 2))
			for (int i = 1; i <= (nTacticalMapWidth / 2); i++)
				TacticalMap[((nTacticalMapWidth / 2) + (nRow * nTacticalMapWidth)) + i] = map[pos + i];
		else
			for (int i = 1; i <= val2; i++)
				TacticalMap[((nTacticalMapWidth / 2) + (nRow * nTacticalMapWidth)) + i] = map[pos + i];

		TacticalMap[(nTacticalMapWidth / 2) + (nRow * nTacticalMapWidth)] = map[pos];
	}
}

void DirectionalArrow(wstring &TacticalMap, int nTacticalMapWidth, int nTacticalMapHeight)
{
	int center = (nTacticalMapWidth) * (nTacticalMapHeight / 2 + 1) + (nTacticalMapWidth / 2);
	
	if (cosf(fPlayerA) >= 0.5)
		TacticalMap[center + nTacticalMapWidth] = 0x2193; //downwards arrow
	else if (cosf(fPlayerA) <= -0.5)
		TacticalMap[center - nTacticalMapWidth] = 0x2191; //upwards arrow
	else if (sinf(fPlayerA) >= 0.5)
		TacticalMap[center + 1] = 0x2190; //leftwards arrow
	else if (sinf(fPlayerA) <= -0.5)
		TacticalMap[center - 1] = 0x2192; //rightwards arrow
}

void PrintTacticalMap(wchar_t* screen, wstring &TacticalMap, int nTacticalMapWidth, int nTacticalMapHeight)
{
	for (int row = 1; row <= nTacticalMapHeight; row++)
	{
		for (int i = 0; i < nTacticalMapWidth; i++)
			screen[nScreenWidth * (7 + row) + (i + 0)] = TacticalMap[(nTacticalMapWidth * (row + 1) - 1) - i];
	}
}

void UserInterface(wchar_t* screen)
{
	wstring HealthBar = L"Health:";
	wstring WeaponName = L"Weapon:";
	wstring Ammo = L"Ammo:";
	wstring Kills = L"Kills:";

	for (int i = 1; i < 8; i++)
		for (int j = 0; j < 30; j++)
			screen[(nScreenWidth * i) + j] = 0x25A1;

	if (0 < health && health <= 10)
		HealthBar += L"*";
	if (10 < health && health <= 20)
		HealthBar += L"**";
	if (20 < health && health <= 30)
		HealthBar += L"***";
	if (30 < health && health <= 40)
		HealthBar += L"****";
	if (40 < health && health <= 50)
		HealthBar += L"*****";
	if (50 < health && health <= 60)
		HealthBar += L"******";
	if (60 < health && health <= 70)
		HealthBar += L"*******";
	if (70 < health && health <= 80)
		HealthBar += L"********";
	if (80 < health && health <= 90)
		HealthBar += L"*********";
	if (90 < health && health <= 100)
		HealthBar += L"**********";

	HealthBar += L" " + to_wstring(health);
	for (int i = 0; i < HealthBar.length(); i++)
		screen[(nScreenWidth * 1) + i] = HealthBar[i];

	WeaponName += wname;
	for (int i = 0; i < WeaponName.length(); i++)
		screen[(nScreenWidth * 2) + i] = WeaponName[i];

	Ammo = Ammo + to_wstring(capacity) + L" / " + to_wstring(reload);
	for (int i = 0; i < Ammo.length(); i++)
		screen[(nScreenWidth * 3) + i] = Ammo[i];

	Kills += to_wstring(kill_count);
	for (int i = 0; i < Kills.length(); i++)
		screen[(nScreenWidth * 4) + i] = Kills[i];

	TacticalMap(screen);

}

void WeaponArt(wchar_t* screen)
{
	
	if (wname == L"9mm Pistol") //ready pistol
	{
		short shadeHand = 0x2588;
		short shadeGun = 0x2592;
		screen[4730] = '+'; //bottom left hand
		screen[4750] = '+'; //bottom right hand
		screen[3770] = '+'; //top left hand
		screen[3790] = '+'; //top right hand
		screen[3773] = '+'; //bottom left gun
		screen[3787] = '+'; //bottom right gun
		screen[3053] = '+'; //top left gun
		screen[3067] = '+'; //top right gun
		
		for (int i = 4731; i < 4750; i++)
			screen[i] = '-';
		for (int i = 3771; i < 3790; i++)
			screen[i] = '-';
		for (int i = 3774; i < 3787; i++)
			screen[i] = '-';
		for (int i = 3054; i < 3067; i++)
			screen[i] = '-';
		

		for (int i = 3770 + 120; i < 4730; i = i + 120)
			screen[i] = '|';
		for (int i = 3790 + 120; i < 4750; i = i + 120)
			screen[i] = '|';
		for (int i = 3053 + 120; i < 3773; i = i + 120)
			screen[i] = '|';
		for (int i = 3067 + 120; i < 3787; i = i + 120)
			screen[i] = '|';
		

		for (int i = (4731 - 120); i < (4750 - 120); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 240); i < (4750 - 240); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 360); i < (4750 - 360); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 480); i < (4750 - 480); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 600); i < (4750 - 600); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 720); i < (4750 - 720); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 840); i < (4750 - 840); i++)
			screen[i] = shadeHand;

		for (int i = (3774 - 120); i < (3787 - 120); i++)
			screen[i] = shadeGun;
		for (int i = (3774 - 240); i < (3787 - 240); i++)
			screen[i] = shadeGun;
		for (int i = (3774 - 360); i < (3787 - 360); i++)
			screen[i] = shadeGun;
		for (int i = (3774 - 480); i < (3787 - 480); i++)
			screen[i] = shadeGun;
		for (int i = (3774 - 600); i < (3787 - 600); i++)
			screen[i] = shadeGun;
	}
	
	if (wname == L"9mm Pistol" && !ReadyShot && 
		(tAnimation - tBegin) + chrono::milliseconds(300) <= chrono::system_clock::now() - tBegin) //fired pistol
	{
		short shadeHand = 0x2588;
		short shadeGun = 0x2592;

		//Shade hand
		for (int i = (4731 - 120); i < (4750 - 120); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 240); i < (4750 - 240); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 360); i < (4750 - 360); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 480); i < (4750 - 480); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 600); i < (4750 - 600); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 720); i < (4750 - 720); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 840); i < (4750 - 840); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 960); i < (4750 - 960); i++)
			screen[i] = shadeHand;
		for (int i = (4731 - 1080); i < (4750 - 1080); i++)
			screen[i] = shadeHand;



		screen[4730] = '+'; //bottom left hand
		screen[4750] = '+'; //bottom right hand
		screen[3530] = '+'; //top left hand
		screen[3550] = '+'; //top right hand
		screen[4373] = '+'; //bottom left gun
		screen[4387] = '+'; //bottom right gun
		screen[1973] = '+'; //top left gun
		screen[1987] = '+'; //top right gun

		for (int i = (4731); i < (4750); i++)
			screen[i] = '-';
		for (int i = (3531); i < (3550); i++)
			screen[i] = '-';
		for (int i = (4374); i < (4387); i++)
			screen[i] = '-';
		for (int i = (1974); i < (1987); i++)
			screen[i] = '-';

		for (int i = (4730 - 120); i > 3530; i = i - 120)
			screen[i] = '|';
		for (int i = (4750 - 120); i > 3550; i = i - 120)
			screen[i] = '|';
		for (int i = (4373 - 120); i > 1973; i = i - 120)
			screen[i] = '|';
		for (int i = (4387 - 120); i > 1987; i = i - 120)
			screen[i] = '|';

		//Shade Gun
		for (int i = (4374 - 120); i < (4387 - 120); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 240); i < (4387 - 240); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 360); i < (4387 - 360); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 480); i < (4387 - 480); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 600); i < (4387 - 600); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 720); i < (4387 - 720); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 840); i < (4387 - 840); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 960); i < (4387 - 960); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1080); i < (4387 - 1080); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1200); i < (4387 - 1200); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1320); i < (4387 - 1320); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1440); i < (4387 - 1440); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1560); i < (4387 - 1560); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1680); i < (4387 - 1680); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1800); i < (4387 - 1800); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 1920); i < (4387 - 1920); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 2040); i < (4387 - 2040); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 2160); i < (4387 - 2160); i++)
			screen[i] = shadeGun;
		for (int i = (4374 - 2280); i < (4387 - 2280); i++)
			screen[i] = shadeGun;

		screen[4740 - 2520] = '-';
		screen[4739 - 2520] = '+';
		screen[4741 - 2520] = '+';
	}

		
}

void MoveEnemy(int id)
{
	int moveX = (rand() % 2);
	int moveY = (rand() % 2);
	int flip = (rand() % 2);

	if (flip == 0)
	{
		Enemies[id]->set_X(Enemies[id]->get_X() + moveX);
		Enemies[id]->set_Y(Enemies[id]->get_Y() + moveY);

		if (map[(nMapWidth * Enemies[id]->get_Y()) + Enemies[id]->get_X()] == '#') //check if in wall
		{
			Enemies[id]->set_X(Enemies[id]->get_X() - moveX);
			Enemies[id]->set_Y(Enemies[id]->get_Y() - moveY);
		}
	}
	else
	{
		Enemies[id]->set_X(Enemies[id]->get_X() - moveX);
		Enemies[id]->set_Y(Enemies[id]->get_Y() - moveY);

		if (map[(nMapWidth * Enemies[id]->get_Y()) + Enemies[id]->get_X()] == '#') //check if in wall
		{
			Enemies[id]->set_X(Enemies[id]->get_X() + moveX);
			Enemies[id]->set_Y(Enemies[id]->get_Y() + moveY);
		}
	}

	//boundary check
	if (Enemies[id]->get_X() < 2)
		Enemies[id]->set_X(2);
	else if (Enemies[id]->get_X() > (nMapWidth - 2))
		Enemies[id]->set_X(nMapWidth - 2);

	if (Enemies[id]->get_Y() < 2)
		Enemies[id]->set_Y(2);
	else if (Enemies[id]->get_Y() > (nMapHeight - 2))
		Enemies[id]->set_Y(nMapHeight - 2);

}

void EnemyMoveHandler(int id) 
{
	if (Enemies[id]->alive() == true)
	{
		if ((Enemies[id]->get_tEnemyMove() - tBegin) + chrono::seconds(1) <= chrono::system_clock::now() - tBegin)
		{
			MoveEnemy(id);
			Enemies[id]->set_tEnemyMove(chrono::system_clock::now());
		}

		int Enemycurrpos = (nMapWidth * Enemies[id]->get_Y()) + Enemies[id]->get_X();
		int Playercurrpos = (nMapWidth * (int)fPlayerY) + (int)fPlayerX;

		map[Enemycurrpos] = 'X';

		if (Enemycurrpos == Playercurrpos)
		{
			if ((Enemies[id]->get_tStrike() - tBegin) + chrono::seconds(1) <= chrono::system_clock::now() - tBegin)
			{
				health = health - Enemies[id]->strike();
				Enemies[id]->set_tStrike(chrono::system_clock::now());
			}
		}
	}
}

void SpawnEnemy(int id)
{
	int x_pos, y_pos;

	do //check if in wall
	{
		x_pos = rand() % nMapWidth;
		y_pos = rand() % nMapHeight;

	} while (map[(nMapWidth * y_pos) + x_pos] == '#');

	Enemies[id] = new Enemy(id);
	Enemies[id]->set_X((rand() % nMapWidth));
	Enemies[id]->set_Y((rand() % nMapHeight));
	Enemies[id]->set_tEnemyMove(chrono::system_clock::now());
	Enemies[id]->set_tStrike(chrono::system_clock::now());


	//check boundaries
	if (Enemies[id]->get_X() < 2)
		Enemies[id]->set_X(2);
	else if (Enemies[id]->get_X() > (nMapWidth - 2))
		Enemies[id]->set_X(nMapWidth - 2);

	if (Enemies[id]->get_Y() < 2)
		Enemies[id]->set_Y(2);
	else if (Enemies[id]->get_Y() > (nMapHeight - 2))
		Enemies[id]->set_Y(nMapHeight - 2);

	CurrentSpawned++;
	tNewSpawn = chrono::system_clock::now();
}

void LoadItems()
{
	int currpos = (nMapWidth * (int)fPlayerY) + (int)fPlayerX;
	int x = 0, y = 0;

	if (SearchItems(currpos, x, y))
	{
		PlayerPickup(itemTypes[x][y]->pickup(), itemTypes[x][y]);
		items[x][y] = 0;
	}
}

void PlayerPickup(int item_pickup, Item* item) //allows item to have appropriate effect based on type
{
	if (item_pickup == 1) //healthpack
	{
		health = 100;
	}
	else if (item_pickup == 2) //weapon
	{
		if (WeaponInventory[0]->get_name() == L"Null")
			WeaponInventory[0] = item;
		else
			WeaponInventory[1] = item;
	}

}

void ReloadMap()
{
	map = map_copy;

	for (int i = 0; i < nMapWidth; i++)
		for (int j = 0; j < nMapHeight; j++)
			if (items[i][j] == 0 && map[itemPosition[i][j]] != '#')
				map[itemPosition[i][j]] = '.';
}

void PlaceItem(int x, int y, Type type)
{
	itemPosition[x][y] = (nMapWidth * y) + x;

	if (type == Healthpack) //1
	{
		itemTypes[x][y] = Item::Create("HealthPack");
		items[x][y] = itemTypes[x][y]->pickup();
	}
	else if (type == Weapon) //2
	{
		itemTypes[x][y] = Item::Create("Weapon");
		itemTypes[x][y]->ChangeState(itemTypes[x][y], Item::Pistol);
		items[x][y] = itemTypes[x][y]->pickup();
	}
	else if (type == Special) //3
		items[x][y] = itemTypes[x][y]->pickup();
	else if (type == Ammo) //4
		items[x][y] = itemTypes[x][y]->pickup();

}

bool SearchItems(int currpos, int &x, int &y)
{
	for (int i = 0; i < nMapWidth; i++)
		for (int j = 0; j < nMapHeight; j++)
		{
			if ((currpos == itemPosition[i][j]) && (items[i][j] != 0))
			{
				x = i;
				y = j;
				return true;
			}
		}
	return false;
}

void PlaceAllItems()
{
	int mappos;

	for (int x = 0; x < nMapWidth; x++)
	{
		for (int y = 0; y < nMapHeight; y++)
		{
			mappos = (nMapWidth * y) + x;

			if (map[mappos] == 'H')
				PlaceItem(x, y, Healthpack);
			else if (map[mappos] == 'W')
				PlaceItem(x, y, Weapon);
		}
	}
}

int FindHitEnemyID(int x, int y)
{
	for (int id = 0; id < CurrentSpawned; id++)
		if (Enemies[id]->get_X() == x && Enemies[id]->get_Y() == y)
			return id;
}