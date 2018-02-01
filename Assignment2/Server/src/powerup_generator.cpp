#include "powerup_generator.h"

Powerup_Generator *Powerup_Generator::instance = 0;

Powerup_Generator::~Powerup_Generator()
{
}

bool Powerup_Generator::Update(float timedelta)
{
	time_ += timedelta;
	if (time_ > MAX_TIME)
	{	// Spawn random power up
		time_ -= MAX_TIME;
		// randomize rand (for random power up)
		return true;
	}
	return false;
}