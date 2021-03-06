#include "ClientNetwork.h"
#include "..\application.h"
#include "packets\packets_s2c.h"
#include "process_packet.h"
#include "send_packet.h"
#include <string>
#include <thread>


// NetLib Step 2. Client network object to use network library.
HNet::_ClientNetwork NetObj;

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif


namespace Net {
    bool InitNetwork( void )
    {
        return NetObj.InitNet( HNet::APPTYPE_CLIENT, HNet::PROTOCOL_TCP, "127.0.0.1", 3456 );
    }

    //-------------------------------------------------------------------------
    void ProcessPacket( Application *thisapp )
    {
        // Check any message from network! You can try to make this message processing with another thread but becareful with synchronization.
        // NetLib Step 6. Prepare the pointer of _ProcessSession and buffer structure of _PacketMessage.
        //         _ProcessSession pointer will give you the session information if there is any network communication from any client.
        //         _PacketMessage is for fetch the each of the actual data inside the packet buffer.
        struct HNet::_ProcessSession *ToProcessSessoin;
        // NetLib Step 7. Message Loop.
        //         Check any message from server and process.
        while( nullptr != ( ToProcessSessoin = NetObj.GetProcessList()->GetFirstSession() ) )
        { // Something recevied from network.
            int PacketID;

            ToProcessSessoin->PacketMessage >> PacketID;
            switch( thisapp->GetGameState() )
            {
                case GAMESTATE_INITIALIZING:
                {
                    switch( PacketID )
                    {
                        case PACKET_ID_S2C_WELCOMEMESSAGE:
                            WelcomeMessage( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_ENTERGAMEOK:
                            thisapp->SetGameState( GAMESTATE_INPLAY );
                            break;

						case PACKET_ID_S2C_SERVERFULL:
							thisapp->GetServerFull()->set_text("Server is currently full, press esc to continue");
							thisapp->GetServerFull()->set_x(100.f);
							thisapp->GetServerFull()->set_y(250.f);
							thisapp->SetGameState( GAMESTATE_SERVERFULL );
							break;
                    }
                }
                break;

                case GAMESTATE_INPLAY:
                {
                    switch( PacketID )
                    {
                        case PACKET_ID_S2C_ENEMYSHIP:
                        case PACKET_ID_S2C_NEWENEMYSHIP:
                            NewEnemyShip( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_NEWASTEROID:
                            NewAstreroid( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_DISCONNECTENEMYSHIP:
                            DisconnectEnemyShip( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_MOVEMENT:
                            ShipMovement( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_COLLIDED:
                            ShipCollided( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_ASTEROIDMOVEMENT:
                            AsteroidMovement( thisapp, ToProcessSessoin );
                            break;

						case PACKET_ID_S2C_ASTEROIDCOLLIDED:
							AsteroidCollided(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWMISSILE:
							NewMissile(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_RENDERBOOM:
							RenderBoom(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_UPDATEHP:
							UpdateHP(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_RESPAWN:
							Respawn(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWPOWERUP_HEAL:
							NewPowerup_Heal(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_HEAL:
							Heal(thisapp, ToProcessSessoin);
							break;
                    }

                }
                break;
            }

            // Step 8. After finish the process with packet, You should delete the session message.
            //         If you forget to delete, it will cause memory leak!
            NetObj.GetProcessList()->DeleteFirstSession();
			//PacketID = PACKET_ID_S2C_EMPTY;
        }
    }

    //-------------------------------------------------------------------------
    void WelcomeMessage( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_WelcomeMessage PacketData;
        ToProcessSessoin->PacketMessage >> PacketData;
        thisapp->GetMyShip()->SetShipID( PacketData.ShipID );
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_WELCOMEMESSAGE. ShipID:%d", PacketData.ShipID );
#endif
        // Send my spaceship info to server for synchronization.
        Net::send_packet_enter_game( thisapp->GetMyShip() );
    }

    //-------------------------------------------------------------------------
    void NewEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        //struct PKT_S2C_EnemyShip EnemyshipPacketData;
        struct PKT_S2C_NewEnemyShip NewEnemyShipPacketData;

        ToProcessSessoin->PacketMessage >> NewEnemyShipPacketData;

        std::string EnemyShipName = "Enemy" + std::to_string( NewEnemyShipPacketData.ShipID );
        Ship *EnemyShip = new Ship( NewEnemyShipPacketData.ShipType, EnemyShipName, NewEnemyShipPacketData.x, NewEnemyShipPacketData.y );

        EnemyShip->SetShipID( NewEnemyShipPacketData.ShipID );
        EnemyShip->SetShipName( EnemyShipName );
        EnemyShip->SetShipType( NewEnemyShipPacketData.ShipType );
        EnemyShip->set_x( NewEnemyShipPacketData.x );
        EnemyShip->set_y( NewEnemyShipPacketData.y );
        EnemyShip->set_w( NewEnemyShipPacketData.w );
        EnemyShip->set_velocity_x( NewEnemyShipPacketData.velocity_x );
        EnemyShip->set_velocity_y( NewEnemyShipPacketData.velocity_y );
        EnemyShip->set_angular_velocity( NewEnemyShipPacketData.angular_velocity );

        thisapp->GetEnemyShipList()->push_back( EnemyShip );
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_ENEMYSHIP or PACKET_ID_S2C_NEWENEMYSHIP. ShipID:%d", EnemyShip->GetShipID() );
        log( "\n        - ShipName:%s, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f",
             EnemyShip->GetShipName().c_str(), EnemyShip->GetShipType(), EnemyShip->get_x(), EnemyShip->get_y(), EnemyShip->get_w() );
        log( "\nNow I have: " );
        for( auto Enemy : *( thisapp->GetEnemyShipList() ) )
        {
            log( "%d ", Enemy->GetShipID() );
        }
#endif
    }

    //-------------------------------------------------------------------------
    void NewAstreroid( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_NewAsteroid AsteroidData;
        ToProcessSessoin->PacketMessage >> AsteroidData;

#ifdef _DEBUG
        log( "\nReceived PACKET_ID_S2C_NEWASTEROID. ID:%d, x:%0.0f, y:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
             AsteroidData.AsteroidID, AsteroidData.x, AsteroidData.y, AsteroidData.velocity_x, AsteroidData.velocity_y, AsteroidData.angular_velocity );
#endif

        Asteroid *asteroid = new Asteroid( "asteroid.png", AsteroidData.x, AsteroidData.y, AsteroidData.angular_velocity );
        asteroid->id_ = AsteroidData.AsteroidID;
        asteroid->set_velocity_x( AsteroidData.velocity_x );
        asteroid->set_velocity_y( AsteroidData.velocity_y );
        thisapp->GetAsteroidList()->push_back( asteroid );
    }

    //-------------------------------------------------------------------------
    void DisconnectEnemyShip( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_EnemyShipDisconnect EnemyShipData;
        ToProcessSessoin->PacketMessage >> EnemyShipData;

        for( unsigned int i = 0; i < thisapp->GetEnemyShipList()->size(); ++i )
        {
            if( thisapp->GetEnemyShipList()->at( i )->GetShipID() == EnemyShipData.ShipID )
            {
                thisapp->GetEnemyShipList()->erase( thisapp->GetEnemyShipList()->begin() + i );
				// If there is an existing missile with the ShipID, delete it
				for (unsigned int i = 0; i < thisapp->GetEnemyMissileList()->size(); ++i)
				{
					if (thisapp->GetEnemyMissileList()->at(i)->get_ownerid() == EnemyShipData.ShipID)
					{
						thisapp->GetEnemyMissileList()->erase(thisapp->GetEnemyMissileList()->begin() + i);
						break;
					}
				}
                break;
            }
        }
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_DISCONNECT. ShipID: %d", EnemyShipData.ShipID );
        log( "\nNow I have: " );
        for( auto Enemy : *( thisapp->GetEnemyShipList() ) )
        {
            log( "%d ", Enemy->GetShipID() );
        }
#endif
    }

    //-------------------------------------------------------------------------
    void ShipMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_Movement MovementData;
        ToProcessSessoin->PacketMessage >> MovementData;
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_MOVEMENT. ShipID:%d, x:%0.0f, y:%0.0f, w:%0.0f, svx:%0.2f, svy:%0.2f, av:%0.0f",
             MovementData.ShipID, MovementData.server_x, MovementData.server_y, MovementData.server_w,
             MovementData.velocity_x, MovementData.velocity_y, MovementData.angular_velocity );
#endif

        Ship *MoveShip;
        if( thisapp->GetMyShip()->GetShipID() == MovementData.ShipID )
        { // Update my ship.
//            MoveShip = thisapp->GetMyShip();
            return;
        }
        else
        { // Update enemyship.
            MoveShip = thisapp->FindEnemyShip( MovementData.ShipID );
        }
        MoveShip->set_server_x( MovementData.server_x );
        MoveShip->set_server_y( MovementData.server_y );
        MoveShip->set_server_w( MovementData.server_w );
        MoveShip->set_server_velocity_x( MovementData.velocity_x );
        MoveShip->set_server_velocity_y( MovementData.velocity_y );
        MoveShip->set_angular_velocity( MovementData.angular_velocity );
        MoveShip->do_interpolate_update();
    }

    //-------------------------------------------------------------------------
    void ShipCollided( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_Collided CollidedData;
        ToProcessSessoin->PacketMessage >> CollidedData;

        Ship *CollidedShip;
        if( thisapp->GetMyShip()->GetShipID() == CollidedData.ShipID )
        { // Update my ship.
            CollidedShip = thisapp->GetMyShip();
        }
        else
        {
            CollidedShip = thisapp->FindEnemyShip( CollidedData.ShipID );
        }
        CollidedShip->set_server_x( CollidedData.server_x );
        CollidedShip->set_server_y( CollidedData.server_y );
        CollidedShip->set_server_w( CollidedData.server_w );
        CollidedShip->set_velocity_x( CollidedData.velocity_x );
        CollidedShip->set_velocity_y( CollidedData.velocity_y );
        CollidedShip->set_angular_velocity( CollidedData.angular_velocity );
    }

    //-------------------------------------------------------------------------
    void AsteroidMovement( Application *thisapp, struct HNet::_ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_AsteroidMovement AsteroidMovementData;
        ToProcessSessoin->PacketMessage >> AsteroidMovementData;

        Asteroid *MovingAsteroid = nullptr;
        for( auto itr_asteroid : *( thisapp->GetAsteroidList() ) )
        {
            if( itr_asteroid->get_id() == AsteroidMovementData.AsteroidID )
            {
                MovingAsteroid = itr_asteroid;
            }
        }
        if( nullptr == MovingAsteroid )
        { // ID error. I cannot find this asteroid.
            return;
        }

        // Now, let's update the data.
        MovingAsteroid->set_server_x( AsteroidMovementData.server_x );
        MovingAsteroid->set_server_y( AsteroidMovementData.server_y );
        MovingAsteroid->set_server_velocity_x( AsteroidMovementData.velocity_x );
        MovingAsteroid->set_server_velocity_y( AsteroidMovementData.velocity_y );
        MovingAsteroid->set_angular_velocity( AsteroidMovementData.angular_velocity );
        MovingAsteroid->do_interpolate_update();
    }

	void AsteroidCollided(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession)
	{
		struct PKT_S2C_AsteroidCollided AsteroidCollidedData;
		ToProcessSession->PacketMessage >> AsteroidCollidedData;

		Asteroid *CollidedAsteroid = NULL;
		for (auto itr_asteroid : *(thisapp->GetAsteroidList()))
		{
			if (itr_asteroid->get_id() == AsteroidCollidedData.AsteroidID)
			{
				CollidedAsteroid = itr_asteroid;
			}
		}
		if (NULL == CollidedAsteroid)
		{	// ID error. Cannot find this asteroid
			return;
		}

		// Now, let's update the data.
		CollidedAsteroid->set_server_x(AsteroidCollidedData.server_x);
		CollidedAsteroid->set_server_y(AsteroidCollidedData.server_y);
		CollidedAsteroid->set_server_velocity_x(AsteroidCollidedData.velocity_x);
		CollidedAsteroid->set_server_velocity_y(AsteroidCollidedData.velocity_y);
		CollidedAsteroid->set_angular_velocity(AsteroidCollidedData.angular_velocity);
		CollidedAsteroid->do_interpolate_update();
	}

	void NewMissile(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession)
	{
		struct PKT_S2C_NewMissile NewMissileData;
		ToProcessSession->PacketMessage >> NewMissileData;

		// If the missile is my own missile, then ignore the packet.
		if (thisapp->GetMyShip()->GetShipID() == NewMissileData.OwnerShipID) return;

		// If there is exsiting missile with the ShipID, delete it first
		for (unsigned int i = 0; i < thisapp->GetEnemyMissileList()->size(); ++i)
		{
			if (thisapp->GetEnemyMissileList()->at(i)->get_ownerid() == NewMissileData.OwnerShipID)
			{
				thisapp->GetEnemyMissileList()->erase(thisapp->GetEnemyMissileList()->begin() + i);
				break;
			}
		}

		Missile *missile = new Missile("missile.png", NewMissileData.x, NewMissileData.y, NewMissileData.w, NewMissileData.OwnerShipID);
		missile->set_velocity_x(NewMissileData.velocity_x);
		missile->set_velocity_y(NewMissileData.velocity_y);
		missile->set_server_velocity_x(NewMissileData.velocity_x);
		missile->set_server_velocity_y(NewMissileData.velocity_y);
		thisapp->GetEnemyMissileList()->push_back(missile);
	}

	//Assignment 2
	void RenderBoom(Application *thisapp, struct HNet::_ProcessSession *ToProcessSession)
	{
		struct PKT_S2C_RenderBoom NewRenderBoomData;
		ToProcessSession->PacketMessage >> NewRenderBoomData;

		// If it's me then ignore
		if (thisapp->GetMyShip()->GetShipID() != NewRenderBoomData.OwnerShipID) return;

		// Need to create a explosion sprite class here and a vector to store all the enemy missile explosion
		Boom *boom = new Boom("boom.png", NewRenderBoomData.x, NewRenderBoomData.y, NewRenderBoomData.OwnerShipID);
		boom->set_render(true);
		boom->set_render_time(0.5f); // render for 5s
		thisapp->GetEnemyBooms()->push_back(boom);
	}

	void UpdateHP(Application * thisapp, HNet::_ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_UpdateHP UpdateHPData;
		ToProcessSession->PacketMessage >> UpdateHPData;

		// if its not me then update enemy's ship else update my ship
		if (thisapp->GetMyShip()->GetShipID() != UpdateHPData.ShipID)
		{
			thisapp->FindEnemyShip(UpdateHPData.ShipID)->Add_HP(-UpdateHPData.damage);
			if (thisapp->FindEnemyShip(UpdateHPData.ShipID)->Get_HP() <= 0.f)
			{
				thisapp->FindEnemyShip(UpdateHPData.ShipID)->set_render(false);
				thisapp->FindEnemyShip(UpdateHPData.ShipID)->set_dead(true);
			}
		}
		else
		{
			// Update the ship hp
			thisapp->GetMyShip()->Add_HP(-UpdateHPData.damage);
			if (thisapp->GetMyShip()->Get_HP() <= 0.f)
			{
				thisapp->GetMyShip()->set_render(false);
				thisapp->GetMyShip()->set_dead(true);
			}
		}

		//// Safety check, if missile didnt collide with me then return (very unlikely)
		//if (thisapp->GetMyShip()->GetShipID() != UpdateHPData.ShipID) return;

		//// Update the ship hp
		//thisapp->GetMyShip()->Add_HP(-UpdateHPData.damage);
		//if (thisapp->GetMyShip()->Get_HP() <= 0.f)
		//{
		//	thisapp->GetMyShip()->set_render(false);
		//	thisapp->GetMyShip()->set_dead(true);
		//}
	}
	void Respawn(Application * thisapp, HNet::_ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_Respawn RespawnData;
		ToProcessSession->PacketMessage >> RespawnData;

		// Safty check, if it's me then return
		if (thisapp->GetMyShip()->GetShipID() == RespawnData.OwnerShipID) return;

		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_x(RespawnData.x);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_y(RespawnData.y);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_w(RespawnData.w);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_velocity_x(RespawnData.velocity_x);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_velocity_y(RespawnData.velocity_y);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_angular_velocity(RespawnData.angular_velocity);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_render(true);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->set_dead(false);
		thisapp->FindEnemyShip(RespawnData.OwnerShipID)->reset_hp();
	}
	void NewPowerup_Heal(Application * thisapp, HNet::_ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_NewPowerupHeal PowerupData;
		ToProcessSession->PacketMessage >> PowerupData;

		Powerup_Heal *powerup = new Powerup_Heal("powerup_heal.png", PowerupData.x, PowerupData.y, PowerupData.PowerupID, PowerupData.heal_);
		powerup->set_render_time(20.0f);
		thisapp->GetPowerup_HealList()->push_back(powerup);
	}
	void Heal(Application * thisapp, HNet::_ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_Heal HealData;
		ToProcessSession->PacketMessage >> HealData;

		// If it's not me then update enemy's hp, else update myself
		if (thisapp->GetMyShip()->GetShipID() != HealData.ShipID)
		{
			thisapp->FindEnemyShip(HealData.ShipID)->Add_HP(HealData.heal_);
			thisapp->FindPowerup_Heal(HealData.PowerupID)->set_render(false);
		}
		else
			thisapp->GetMyShip()->Add_HP(HealData.heal_);
	}
}

