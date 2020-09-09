#pragma once
#include "Item.h"
class Weapon : public Item
{
public:
	Weapon() {}
	virtual ~Weapon() {}

	void ChangeState(Item*&, Weapon_Type);

	virtual int pickup();
	void set_weapon(string type);
	virtual wstring get_name();
	virtual int get_damage();
	virtual int get_capacity();
	virtual int get_reload();

protected:
	wstring name = L"Null";
	int damage = 0;
	int capacity = 0;
	int reload = 0;
};