#pragma once
#include "Item.h"
class Weapon : public Item
{
public:
	Weapon() {}
	~Weapon() {}

	virtual int pickup();
	virtual void reloadable();
	void set_weapon(string type);
	wstring get_name();
	int get_damage();
	int get_capacity();
	int get_reload();

	wstring name = L"Null";
	int damage = 0;
	int capacity = 0;
	int reload = 0;

};