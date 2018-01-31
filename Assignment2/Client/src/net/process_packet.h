#ifndef _PROCESSPACKET_H
#define _PROCESSPACKET_H
#pragma once

#include "ClientNetwork.h"

// Function declarerations.
namespace Net {
    bool InitNetwork( void );
    void ProcessPacket( Application *thisapp );

    void WelcomeMessage( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void NewEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void NewAstreroid( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void DisconnectEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void ShipMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void ShipCollided( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
    void AsteroidMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
	void AsteroidCollided(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
	void NewMissile(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);

	//Assignment 2
	void RenderBoom(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
	void UpdateHP(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession);
}



#endif
