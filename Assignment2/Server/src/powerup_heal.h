#ifndef _POWER_UP_HEAL_H
#define _POWER_UP_HEAL_H

#include "powerup.h"

class Powerup_Heal : public _Powerup
{
private:
	float healamt_;

public:
	Powerup_Heal(float healamt, int ID = -100, float x = 0.0f, float y = 0.0f)
	{
		ID_ = ID;
		x_ = x;
		y_ = y;
		healamt_ = healamt;
	}
	~Powerup_Heal() { ; }

	void set_healamt(float healamt) { healamt_ = healamt; }
	float get_healamt(void) { return healamt_; }
};

#endif