#include "Weapon.h"
#include "PistolState.h"
#include "SuperPistolState.h"

void Weapon::ChangeState(Item*& state, Weapon_Type type)
{
	switch (type)
	{
		case (Pistol):
		{
			state = new PistolState;
		}break;
		case (RailGun):
		{
			//blank
		}break;
		case (Super_Pistol):
		{
			state = new SuperPistolState;
		}break;
		default:
		{
			state = new PistolState;
		}
	}
}

int Weapon::pickup()
{
	return 2;
}

void Weapon::set_weapon(string type)
{
	if (type == "Pistol")
	{
		name = L"Pistol";
		damage = 500;
		capacity = 16;
		reload = 32;
	}
	else if (type == "RailGun")
	{
		name = L"RailGun";
		damage = 12;
		capacity = 8;
		reload = 16;
	}
	else
	{
		//Error state.
	}
}

wstring Weapon::get_name()
{
	return name;
}
int Weapon::get_damage()
{
	return damage;
}
int Weapon::get_capacity()
{
	return capacity;
}
int Weapon::get_reload()
{
	return reload;
}