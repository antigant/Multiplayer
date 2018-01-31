#ifndef _SEND_PACKET_H
#define _SEND_PACKET_H
#pragma once

#include "..\Shares\timer\timer.h"
#include "..\movables\ship.h"
#include "ClientNetwork.h"
#include "packets\packets_c2s.h"
#include "..\movables\asteroid.h"
#include "../movables/missile.h"

// Function prototypes.
namespace Net
{
    void send_packet_enter_game( Ship *myship );
    void send_packet_myship_movement( Ship *myship );
    void send_packet_collided( Ship *myship );
	void send_packet_asteroid_collided(Asteroid *asteroid);
	void send_packet_new_missile(Missile *newMissile);

	// Assignment 2
	void send_packet_render_boom(int OwnershipID, float x, float y, bool render_boom);
}


#endif