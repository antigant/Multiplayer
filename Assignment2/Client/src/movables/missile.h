#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "hge.h"
#include "hgerect.h"
#include "movables.h"

class hgeSprite;

class Missile : public Movables
{
	private:
		int ownerid_;
		float damage_;
		int shipid_; // stores the id of the ship the missile had collide with

    public:
    	HTEXTURE tex_; //!< Handle to the sprite's texture
    	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

    public: // Constructors and Destructor.
        Missile( char* filename, float x, float y, float w, int shipid );
    	~Missile();

    public: // Getters and Setters.
        void set_ownerid( int ownerid ) { ownerid_ = ownerid; }
    	int get_ownerid() { return ownerid_; }

		void set_damage(float damage) { damage_ = damage; }
		float get_damage() { return damage_; }

		// Assignment 2
		void set_shipid(int shipid) { shipid_ = shipid; }
		int get_shipid() { return shipid_; }

    public: // Interface functions.
		virtual bool Update(float timedelta, float spritewidth, float spriteheight);
    	void Render();
};

#endif