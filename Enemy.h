#pragma once
#include <stdlib.h>
#include <chrono>
#include "BehaviorDelegate.h"
using namespace std;

class Enemy
{
public:
	Enemy(int id)
	{
		id = id;
		health = 100;
		base_damage = 5;
		living = true;
	}

	void hit(int bullet)
	{
		health = health - bullet;
	}

	void set_X(int x)
	{
		X_position = x;
	}

	void set_Y(int y)
	{
		Y_position = y;
	}

	void set_tStrike(chrono::time_point<chrono::system_clock> tp)
	{
		tStrike = tp;
	}

	void set_tEnemyMove(chrono::time_point<chrono::system_clock> tp)
	{
		tEnemyMove = tp;
	}

	int strike()
	{
		return base_damage;
	}

	bool alive()
	{
		if (health >= 0)
			return (living = true);
		else
			return (living = false);
	}

	int get_health()
	{
		return health;
	}

	int get_id()
	{
		return id;
	}

	int get_X()
	{
		return X_position;
	}

	int get_Y()
	{
		return Y_position;
	}

	chrono::time_point<chrono::system_clock> get_tStrike()
	{
		return tStrike;
	}

	chrono::time_point<chrono::system_clock> get_tEnemyMove()
	{
		return tEnemyMove;
	}

	void move(int playerX, int playerY)
	{
		_b->move(X_position, Y_position, playerX, playerY);
	}

private:
	int id;
	int health;
	int base_damage;
	int X_position;
	int Y_position;
	bool living;
	chrono::time_point<chrono::system_clock> tStrike;
	chrono::time_point<chrono::system_clock> tEnemyMove;

	BehaviorDelegate* _b = new AggressiveBehavior(); //switch between AggressiveBehavior, ZombieBehavior, or PassiveBehavior
};