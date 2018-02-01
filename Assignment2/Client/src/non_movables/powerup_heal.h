#ifndef	_POWER_UP_HEAL
#define _POWER_UP_HEAL

#include "hge.h"
#include "non_movables.h"

class hgeSprite;

class Powerup_Heal : public NonMovables
{
public:
	HTEXTURE tex_; 
	std::auto_ptr<hgeSprite> sprite_;

	int powerupid_;
	int shipid_;
	float healamt_;

public:
	Powerup_Heal(char *filename, float x, float y, int powerupid, float healamt);
	~Powerup_Heal();

public:
	void set_powerupid(int powerupid) { powerupid_ = powerupid; }
	int get_powerupid() { return powerupid_; }

	void set_shipid(int shipid) { shipid_ = shipid; }
	int get_shipid() { return shipid_; }

	void set_healamt(float healamt) { healamt_ = healamt; }
	float get_healamt() { return healamt_; }

public:
	void Render(); // Interface functions
};

#endif