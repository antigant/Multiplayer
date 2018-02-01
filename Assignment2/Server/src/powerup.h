#ifndef _POWER_UP_H
#define _POWER_UP_H

class _Powerup
{
protected:
	int ID_;
	float x_;
	float y_;

public:
	_Powerup(int ID = -100, float x = 0.0f, float y = 0.0f);
	~_Powerup();

public:
	void set_ID(int ID) { ID_ = ID; }
	void set_x(float x) { x_ = x; }
	void set_y(float y) { y_ = y; }

	int get_ID(void) { return ID_; }
	float get_x(void) { return x_; }
	float get_y(void) { return y_; }
};

#endif