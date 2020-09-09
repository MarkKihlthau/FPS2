#pragma once
#pragma once
#include "Weapon.h"

class SuperPistolState : public Weapon
{
public:
	SuperPistolState() {};
	~SuperPistolState() {};

	int pickup() override;
	wstring get_name() override;
	int get_damage() override;
	int get_capacity() override;
	int get_reload() override;

private:
	wstring name = L"Super Pistol";
	int damage = 500;
	int capacity = 999;
	int reload = 999;
};

int SuperPistolState::pickup()
{
	return 2;
}
wstring SuperPistolState::get_name()
{
	return name;
}
int SuperPistolState::get_damage()
{
	return damage;
}
int SuperPistolState::get_capacity()
{
	return capacity;
}
int SuperPistolState::get_reload()
{
	return reload;
}