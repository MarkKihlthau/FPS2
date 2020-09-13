#pragma once
#include "Weapon.h"
#include "SuperPistolState.h"

class PistolState : public Weapon
{
public:
	PistolState() {};
	~PistolState() {};

	void ChangeState(Item*&, Weapon_Type);

	int pickup() override;
	wstring get_name() override;
	int get_damage() override;
	int get_capacity() override;
	int get_reload() override;

private:
	wstring name = L"9mm Pistol";
	int damage = 50;
	int capacity = 16;
	int reload = 48;
};

void PistolState::ChangeState(Item*& state, Weapon_Type type)
{
	switch (type)
	{
	case (Pistol):
	{
		state = new PistolState;
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