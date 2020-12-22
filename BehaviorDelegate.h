#pragma once
#include "Map.h"
#include <string>

using namespace std;

class BehaviorDelegate
{
public:
	virtual void move(int &enemyX, int &enemyY, int playerX, int playerY) = 0;

protected:
	Map* m = m->getInstance();
	int nMapHeight = m->get_nMapHeight();
	int nMapWidth = m->get_nMapWidth();
	wstring map = m->get_map();
};

class AggressiveBehavior : public BehaviorDelegate
{
public:
	virtual void move(int &enemyX, int &enemyY, int playerX, int playerY) override
	{
		int moveX;
		int moveY;

		//move one space towards the player
		if (playerX > enemyX)
			moveX = 1;
		else if (playerX == enemyX)
			moveX = 0;
		else if (playerX < enemyX)
			moveX = -1;

		if (playerY > enemyY)
			moveY = 1;
		else if (playerY == enemyY)
			moveY = 0;
		else if (playerY < enemyY)
			moveY = -1;

		enemyX = (enemyX + moveX);
		enemyY = (enemyY + moveY);

		if (map[(nMapWidth * enemyY) + enemyX] == '#') //check if in wall
		{
			enemyX = (enemyX - moveX);
			enemyY = (enemyY - moveY);
		}

		//boundary check
		if (enemyX < 2)
			enemyX = 2;
		else if (enemyX >(nMapWidth - 2))
			enemyX = (nMapWidth - 2);

		if (enemyY < 2)
			enemyY = 2;
		else if (enemyY >(nMapHeight - 2))
			enemyY = (nMapHeight - 2);
	}
};

class ZombieBehavior : public BehaviorDelegate
{
public:
	virtual void move(int &enemyX, int &enemyY, int playerX, int playerY) override
	{
		int moveX = (rand() % 2);
		int moveY = (rand() % 2);
		int flip = (rand() % 2);

		if (flip == 0)
		{
			enemyX = (enemyX + moveX);
			enemyY = (enemyY + moveY);

			if (map[(nMapWidth * enemyY) + enemyX] == '#') //check if in wall
			{
				enemyX = (enemyX - moveX);
				enemyY = (enemyY - moveY);
			}
		}
		else
		{
			enemyX = (enemyX - moveX);
			enemyY = (enemyY - moveY);

			if (map[(nMapWidth * enemyY) + enemyX] == '#') //check if in wall
			{
				enemyX = (enemyX + moveX);
				enemyY = (enemyY + moveY);
			}
		}

		//boundary check
		if (enemyX < 2)
			enemyX = 2;
		else if (enemyX > (nMapWidth - 2))
			enemyX = (nMapWidth - 2);

		if (enemyY < 2)
			enemyY = 2;
		else if (enemyY > (nMapHeight - 2))
			enemyY = (nMapHeight - 2);
	}
};

class PassiveBehavior : public BehaviorDelegate
{
public:
	virtual void move(int &enemyX, int &enemyY, int playerX, int playerY) override
	{
		//do nothing
	}
};