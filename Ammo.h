#pragma once
#include "Weapon.h"
class Ammo : public Weapon
{
public:
	Ammo() {}
	~Ammo() {}

	wstring name = L"Null";
	int amount = 0;

	int pickup();
	void reloadable();
	void set_ammo(string type);
	wstring get_name();
	int get_amount();
};

int Ammo::pickup()
{
	return 4;
}

void Ammo::reloadable()
{
	if (Ammo::name == L"9mm" && Weapon::name == L"Pistol")
		capacity = capacity + amount;
	else if (Ammo::name == L"Battery" && Weapon::name == L"RailGun")
		capacity = capacity + amount;
}

void Ammo::set_ammo(string type)
{
	if (type == "9mm")
	{
		name = L"9mm";
		amount = 32;
	}
	else if (type == "Battery")
	{
		name = L"Battery";
		amount = 16;
	}
	else
	{
		//Error State.
	}
}

wstring Ammo::get_name()
{
	return name;
}

int Ammo::get_amount()
{
	return amount;
}