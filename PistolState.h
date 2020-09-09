#pragma once
#include "Weapon.h"

class PistolState : public Weapon
{
public:
	PistolState() {};
	~PistolState() {};

	int pickup() override;
	wstring get_name() override;
	int get_damage() override;
	int get_capacity() override;
	int get_reload() override;

private:
	wstring name = L"9mm Pistol";
	int damage = 50;
	int capacity = 48;
	int reload = 16;
};

int PistolState::pickup()
{
	return 2;
}
wstring PistolState::get_name()
{
	return name;
}
int PistolState::get_damage()
{
	return damage;
}
int PistolState::get_capacity()
{
	return capacity;
}
int PistolState::get_reload()
{
	return reload;
}