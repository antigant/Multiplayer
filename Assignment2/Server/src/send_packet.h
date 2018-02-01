#ifndef _SENDPACKET_H
#define _SENDPACKET_H

#include "ServerNetwork.h"
#include "packets\packets_s2c.h"
#include "asteroid.h"
#include "Ship.h"
#include "powerup_heal.h"

///////////////////////////////////////////////////////////////////////////////
// Global function declarations.
void SendPacketProcess_NewAccept( const int SessionID );
void SendPacketProcess_NewAccept_SendWelcomeMessage( const int SessionID );
void SendPacketProcess_EnemyShipDisconnect( const int SessionID );
void SendPacketProcess_AsteroidMovement( _Asteroid *asteroid );
void SendPacketProcess_AsteroidCollided(_Asteroid *asteroid);

// Assignment 2
// Add new functions to disconnect player if server have more than 3
void SendPacketProcess_ServerFull(const int SessionID);
void SendPacketProcess_ServerFull_Message(const int SessionID);

void SendPacketProcess_NewPowerupHeal(Powerup_Heal *powerup);

#endif