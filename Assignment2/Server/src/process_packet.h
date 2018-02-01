#ifndef _PROCESS_PACKET_H
#define _PROCESS_PACKET_H
#pragma once


// Function declarations.
void ReceviedPacketProcess( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_EnterGame( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_Movement( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceivedPacketProcess_Collided( struct HNet::_ProcessSession *ToProcessSessoin );
void ReceviedPacketProcess_AsteroidCollided(struct HNet::_ProcessSession *ToProcessSessoin);
void ReceviedPacketProcess_NewMissile(struct HNet::_ProcessSession *ToProcessSessoin);

// Assignment 2
void ReceviedPacketProcess_RenderBoom(struct HNet::_ProcessSession *ToProcessSessoin);
void ReceviedPacketProcess_UpdateHP(struct HNet::_ProcessSession *ToProcessSessoin);
void ReceviedPacketProcess_Respawn(struct HNet::_ProcessSession *ToProcessSessoin);
#endif