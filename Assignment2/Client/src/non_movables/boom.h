#ifndef _BOOM_H
#define _BOOM_H

#include "hge.h"
#include "non_movables.h"

class hgeSprite;

class Boom : public NonMovables
{
public:
	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the boom

	int ownerid_;

public: // Constructors and Destructor
	Boom(char* filename, float x, float y, int ownerid_);
	~Boom();

public:
	void set_ownerid(int ownerid) { ownerid_ = ownerid; }
	int get_ownerid() { return ownerid_; }

public: // Interface functions.
	void Render();
};

#endif