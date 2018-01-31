#ifndef _PACKETS_C2S_H
#define _PACKETS_C2S_H

enum _PACKET_ID_CLIENT_TO_SERVER {
    PACKET_ID_C2S_EMPTY = 0,            // Start of the list. Do not remove this.

    PACKET_ID_C2S_ENTERGAME,
    PACKET_ID_C2S_MOVEMENT,
    PACKET_ID_C2S_COLLIDED,
	PACKET_ID_C2S_ASTEROIDCOLLIDED,
	PACKET_ID_C2S_NEWMISSILE,

	/************ Assignment2 *************/
	// Use to render boom for t seconds
	PACKET_ID_C2S_RENDERBOOM,
	PACKET_ID_C2S_UPDATEHP,

    PACKET_ID_C2S_END                   // This is the indicator for the end of the list. Do not put any packet ID after this.

};


/**
* This is the list of data structures for each packet.
* ** IMPORTANT ** All the structures have to be fixed size.
*/
#pragma pack(push, 1) // Pack the memory alignment. 
struct PKT_C2S_EnterGame {
    int ShipID;
    int ShipType;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_Movement {
    int ShipID;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_Collided {
    int ShipID;
    float x;
    float y;
    float w;
    float velocity_x;
    float velocity_y;
    float angular_velocity;
};

struct PKT_C2S_AsteroidCollided
{
	int AsteroidID;
	float x;
	float y;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_C2S_NewMissile
{
	int OwnerShipID;
	float x;
	float y;
	float w;
	float velocity_x;
	float velocity_y;
	float angular_velocity;
};

struct PKT_C2S_RenderBoom
{
	int OwnerShipID;
	float x;
	float y;
	bool render_boom;
};

struct PKT_C2S_UpdateHP
{
	// ID of the ship that missile collides with
	int ShipID; 
	float damage;
};

struct PKT_C2S_DeleteMissile
{
	// Not yet implemented
};

struct PKT_C2S_UpdateMissile
{
	// Not yet implemented
};

#pragma pack(pop) // Return back to default memory alignment.


#endif