#ifndef _POWER_UP_GENERATOR_H_
#define _POWER_UP_GENERATOR_H_

// Every MAX_TIME, spawn one power up
#define MAX_TIME 10.0f

class Powerup_Generator
{
private:
	Powerup_Generator()
	{
		rand_ = 0;
		ID_ = 0;
		time_ = 0.0f;
	};
	static Powerup_Generator *instance;

	int rand_;
	int ID_;
	float time_;

public:
	static Powerup_Generator *GetInstance()
	{
		if (!instance)
			instance = new Powerup_Generator();
		return instance;
	}
	~Powerup_Generator();

	bool Update(float timedelta);
	
	int get_rand(void) { return rand_; }
	int get_ID(void) { return ++ID_; }
};

#endif