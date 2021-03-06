#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "hge.h"

#include "globals.h"
#include "net\process_packet.h"
#include "net\send_packet.h"
#include "Application.h"
#include "movables\ship.h"
#include "movables\asteroid.h"

#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#include <io.h>
#endif

#ifdef _DEBUG
void log( char *szFormat, ... )
{
    char Buff[1024];
    char Msg[1024];
    va_list arg;

    va_start( arg, szFormat );
    _vsnprintf_s( Msg, 1024, szFormat, arg );
    va_end( arg );

    sprintf_s( Buff, 1024, "%s", Msg );
    _write( 1, Buff, strlen( Buff ) );
}
#endif

#define ABS(n) ( (n < 0) ? (-n) : (n) )

/** 
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/
Application::Application() : 
	hge_(hgeCreate(HGE_VERSION))
	, myMissile(NULL)
	, keydown_enter(false)
{
    SetGameState( GAMESTATE_NONE );
}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/
Application::~Application() throw()
{
	Shutdown();
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/
bool Application::Init()
{
	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "SpaceShooter");
	hge_->System_SetState(HGE_LOGFILE, "SpaceShooter.log");
    hge_->System_SetState(HGE_DONTSUSPEND, true );
	
    if( false == hge_->System_Initiate() )
    {
        return false;
    }
    
    srand( (unsigned int)time(NULL) );

    // Initialize and prepare the game data & systems.
    // Initialize my own spaceship.
    int shiptype = ( rand() % 4 ) + 1;
    float startx = (float)(( rand() % 600 ) + 100);
    float starty = (float)(( rand() % 400 ) + 100);
    float startw = 0.0f;
    myship_ = new Ship( shiptype, "MyShip", startx, starty, startw );
	dead_message.set_text("Press R to respawn...");
	dead_message.set_x(300.f);
	dead_message.set_y(275.f);

    // Initialize asteroids.
    //asteroids_.push_back( new Asteroid( "asteroid.png", 100, 100, 1 ) );
    //asteroids_.push_back( new Asteroid( "asteroid.png", 700, 500, 1.5 ) );

    // Initialize the network with Network Library.
    if( !(Net::InitNetwork()) )
    {
        return false;
    }
    SetGameState( GAMESTATE_INITIALIZING );

    return true;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
    float timedelta = hge_->Timer_GetDelta();

    // Process the packet received from server.
    Net::ProcessPacket( this );

    if( GAMESTATE_INPLAY == GetGameState() )
    {

        // Check key inputs and process the movements of spaceship.
        if( hge_->Input_GetKeyState( HGEK_ESCAPE ) )
            return true;

        myship_->set_angular_velocity( 0.0f );
        if( hge_->Input_GetKeyState( HGEK_LEFT ) )
        {
            myship_->set_angular_velocity( myship_->get_angular_velocity() - DEFAULT_ANGULAR_VELOCITY );
        }
        if( hge_->Input_GetKeyState( HGEK_RIGHT ) )
        {
            myship_->set_angular_velocity( myship_->get_angular_velocity() + DEFAULT_ANGULAR_VELOCITY );
        }
        if( hge_->Input_GetKeyState( HGEK_UP ) )
        {
            myship_->Accelerate( DEFAULT_ACCELERATION, timedelta );
        }
        if( hge_->Input_GetKeyState( HGEK_DOWN ) )
        {
            myship_->Accelerate( -DEFAULT_ACCELERATION, timedelta );
        }
        if( hge_->Input_GetKeyState( HGEK_0 ) )
        {
            myship_->stop_moving();
        }
		if (hge_->Input_GetKeyState(HGEK_ENTER))
		{
			if (!keydown_enter && myship_->get_render())
			{
				CreateMissile(myship_->get_x(), myship_->get_y(), myship_->get_w(), myship_->GetShipID());
				keydown_enter = true;
			}
		}
		else
			keydown_enter = false;

		if (hge_->Input_GetKeyState(HGEK_R))
		{	// Respawn player
			if (myship_->get_dead()) // if dead then random position on world, then send to server
			{
				float x = (float)((rand() % 600) + 100);
				float y = (float)((rand() % 400) + 100);
				float w = 0.0f;

				// Set new position and make the ship stop moving
				myship_->set_x(x);
				myship_->set_y(y);
				myship_->set_w(w);
				myship_->stop_moving();

				// To make it render on screen and not letting the player triggering this func agn
				myship_->set_render(true);
				myship_->set_dead(false);
				myship_->reset_hp();

				// Send client's ship data to other connecting clients
				Respawn(myship_);
			}
		}

        // Update my space ship.
        bool AmIMoving = false;
        if( true == ( AmIMoving = myship_->Update( timedelta, myship_->sprite_->GetWidth(), myship_->sprite_->GetHeight() ) ) )
        {
            for( auto itr_asteroid : asteroids_ )
            {
                if( true == CheckCollision( myship_, itr_asteroid, timedelta ) )
                {
                    myship_->set_collided_with_me( true );
                }
            }
        }

        // Update asteroids.
        for( AsteroidList::iterator itr_asteroid = asteroids_.begin(); itr_asteroid != asteroids_.end(); ++itr_asteroid )
        {
            if( true == ( *itr_asteroid )->Update( timedelta, ( *itr_asteroid )->sprite_->GetWidth(), ( *itr_asteroid )->sprite_->GetHeight() ) )
            {
                // Collision check with other asteroids.
                for( AsteroidList::iterator next_asteroid = ( itr_asteroid + 1 ); next_asteroid != asteroids_.end(); ++next_asteroid )
                {
                    CheckCollision( ( *itr_asteroid ), ( *next_asteroid ), timedelta );
                }

                // Collision check with my ship.
                if( false == AmIMoving )
                {
                    if( false == myship_->get_collided_with_me() )
                        CheckCollision( ( *itr_asteroid ), myship_, timedelta );
                    else
                        myship_->set_collided_with_me( false );
                }
            }
        }

        // Update enemy ships.
        for( ShipList::iterator itr_enemyship = enemyships_.begin(); itr_enemyship != enemyships_.end(); ++itr_enemyship )
        {
            ( *itr_enemyship )->Update( timedelta, ( *itr_enemyship )->sprite_->GetWidth(), ( *itr_enemyship )->sprite_->GetHeight() );
        }


		// Update my missile.
		if (myMissile)
		{
			if (!myMissile->get_render())
			{
				delete myMissile;
				myMissile = NULL;
			}

			else if (myMissile->Update(timedelta, myMissile->sprite_->GetWidth(), myMissile->sprite_->GetHeight()))
			{
				// Collision check with asteroids
				for (auto itr_asteroids : asteroids_)
				{
					if (HasCollided(myMissile, itr_asteroids))
					{
						// render boom!
						CreateBoom(myMissile->get_x(), myMissile->get_y(), myMissile->get_ownerid());
						delete myMissile;
						myMissile = NULL;
						break;
					}
				}

				// Collision check with enemyships
				if (myMissile)
				{
					for (auto itr_enemyship : enemyships_)
					{
						if (!itr_enemyship->get_render())
							continue;
						if (HasCollided(myMissile, itr_enemyship))
						{
							CreateBoom(myMissile->get_x(), myMissile->get_y(), myMissile->get_ownerid());
							// Due damage to enemy ship
							myMissile->set_shipid(itr_enemyship->GetShipID());
							// functions that update hp
							UpdateHP(myMissile);
							//UpdateHP(itr_enemyship, myMissile->get_damage());

							delete myMissile;
							myMissile = NULL;
							break;
						}
					}				
				}
			}
		}

		// Update all enemy missiles
		bool missile_collided = false;
		for (unsigned int i = 0; i < enemymissiles_.size(); ++i)
		{
			if (!enemymissiles_[i]->get_render())
			{
				enemymissiles_.erase(enemymissiles_.begin() + i);
				continue;
			}

			else if (enemymissiles_[i]->Update(timedelta, enemymissiles_[i]->sprite_->GetWidth(), enemymissiles_[i]->sprite_->GetHeight()))
			{
					for (auto itr_asteroids : asteroids_)
					{
						if (HasCollided(enemymissiles_[i], itr_asteroids))
						{
							missile_collided = true;
							// render boom!
							CreateBoom(enemymissiles_[i]->get_x(), enemymissiles_[i]->get_y(), enemymissiles_[i]->get_ownerid());
							enemymissiles_.erase(enemymissiles_.begin() + i);
							break;
						}
					}

					if (true == missile_collided) continue;
					//if (i >= enemymissiles_.size())
					//	continue;

					if (enemymissiles_[i])
					{
						if (HasCollided(myship_, enemymissiles_[i]))
						{
							missile_collided = true;
							// render boom!
							CreateBoom(enemymissiles_[i]->get_x(), enemymissiles_[i]->get_y(), enemymissiles_[i]->get_ownerid());
							enemymissiles_.erase(enemymissiles_.begin() + i);
							// render boom!
							break;
						}
					}

					if (true == missile_collided) continue;

					if (enemymissiles_[i])
					{
						for (auto itr_enemyship : enemyships_)
						{
							if (!itr_enemyship->get_render())
								continue;
							if (itr_enemyship->GetShipID() == enemymissiles_[i]->get_ownerid())
								continue;
							if (HasCollided(enemymissiles_[i], itr_enemyship))
							{
								missile_collided = true;
								CreateBoom(enemymissiles_[i]->get_x(), enemymissiles_[i]->get_y(), enemymissiles_[i]->get_ownerid());
								enemymissiles_.erase(enemymissiles_.begin() + i);

								// Due damage to enemy ship
								//myMissile->set_shipid(itr_enemyship->GetShipID());
								//// functions that update hp
								//UpdateHP(myMissile);
								//UpdateHP(itr_enemyship, myMissile->get_damage());

								//delete myMissile;
								//myMissile = NULL;
								break;
							}
						}
					}
			}
		}

		/**
		* Assignment 2
		*/
		// Update power ups
		bool powerup_collided = false;
		for (int i = 0; i < powerups_.size(); ++i)
		{
			if (!powerups_[i]->get_render())
			{
				delete powerups_[i];
				powerups_[i] = NULL;
				powerups_.erase(powerups_.begin() + i);
			}

			// Check powerup against my ship
			else if (powerups_[i]->Update(timedelta))
			{
				if (HasCollided(powerups_[i], myship_))
				{	// Send to all, update player's health
					powerups_[i]->set_shipid(myship_->GetShipID());
					Heal(powerups_[i]);

					delete powerups_[i];
					powerups_[i] = NULL;
					powerups_.erase(powerups_.begin() + i);
				}
			}
			else
			{	// Times up, not rendering anymore
				delete powerups_[i];
				powerups_[i] = NULL;
				powerups_.erase(powerups_.begin() + i);
			}
		}

		// Update booms
		for (int i = 0; i < enemybooms_.size(); ++i)
		{	// Enemy
			if (!enemybooms_[i]->Update(timedelta))
			{
				delete enemybooms_[i];
				enemybooms_[i] = NULL;
				enemybooms_.erase(enemybooms_.begin() + i);
			}
		}

		for (int i = 0; i < mybooms_.size(); ++i)
		{	// Client's
			if (!mybooms_[i]->Update(timedelta))
			{
				delete mybooms_[i];
				mybooms_[i] = NULL;
				mybooms_.erase(mybooms_.begin() + i);
			}
		}

        // Now send the update to server. But not every frame.
        if( !myship_->compare_xyw() ) Net::send_packet_myship_movement( myship_ );
    }

	else if (GAMESTATE_SERVERFULL == GetGameState())
	{
		// Check key inputs and process the movements of spaceship.
		if (hge_->Input_GetKeyState(HGEK_ESCAPE))
			return true;
	}

	return false;
}


/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{
	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);

	if (GAMESTATE_INPLAY == GetGameState())
	{
		if (myship_->get_dead())
			dead_message.Render();

		// Render my space ship.
		myship_->Render();

		// Render enemy ships.
		for (auto enemyship : enemyships_) enemyship->Render();

		// Render asteroids.
		for (auto asteroid : asteroids_) asteroid->Render();

		// Render enemy missile
		for (auto enemymissile : enemymissiles_) enemymissile->Render();

		// Render my missile
		if (myMissile) myMissile->Render();

		//std::cout << powerups_.size() << std::endl;
		// Render powerups
		for (auto powerups : powerups_) powerups->Render();

		// Render booms effect
		for (auto booms : enemybooms_) booms->Render();

		for (auto booms : mybooms_) booms->Render();
	}

	else if (GAMESTATE_SERVERFULL == GetGameState())
	{
		// Render text on screen saying server is full
		server_full.Render();
	}

	hge_->Gfx_EndScene();
}

Ship * Application::FindEnemyShip( int ShipID )
{
    for( auto enemyship : enemyships_ )
    {
        if( ShipID == enemyship->GetShipID() )
        {
            return enemyship;
        }
    }

    return nullptr;
}

Powerup_Heal *Application::FindPowerup_Heal(int powerupid)
{
	for (auto powerup : powerups_)
	{
		if (powerupid == powerup->get_powerupid())
			return powerup;
	}
	return nullptr;
}

/** 
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/
void Application::Shutdown()
{
	hge_->System_Shutdown();
	hge_->Release();
}

/** 
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
	}
}

template <typename T1, typename T2>
bool Application::HasCollided( T1 &object, T2 &movable )
{
    hgeRect object_collidebox;
    hgeRect movable_Collidebox;

    object->sprite_->GetBoundingBox( object->get_x(), object->get_y(), &object_collidebox );
    movable->sprite_->GetBoundingBox( movable->get_x(), movable->get_y(), &movable_Collidebox );
    return object_collidebox.Intersect( &movable_Collidebox );
}

template <typename Mov, typename Tgt>
bool Application::CheckCollision( Mov &moving_object, Tgt &other, float timedelta )
{
    if( HasCollided( moving_object, other ) )
    {
        other->set_velocity_x( moving_object->get_velocity_x() );
        other->set_velocity_y( moving_object->get_velocity_y() );
        other->set_server_velocity_x( moving_object->get_server_velocity_x() );
        other->set_server_velocity_y( moving_object->get_server_velocity_y() );
        other->Update( timedelta, other->sprite_->GetWidth(), other->sprite_->GetHeight() );

        moving_object->set_velocity_x( moving_object->get_velocity_x() );
        moving_object->set_velocity_y( -moving_object->get_velocity_y() );
        moving_object->set_server_velocity_x( -moving_object->get_server_velocity_x() );
        moving_object->set_server_velocity_y( -moving_object->get_server_velocity_y() );
        moving_object->restore_xy();
        moving_object->Update( timedelta, moving_object->sprite_->GetWidth(), moving_object->sprite_->GetHeight() );

        if( (void *)moving_object == (void *)myship_ )
        { // If I collided with others, need to send the message to the server.
            Net::send_packet_collided( (Ship *)moving_object );
        }
		else if (MOVABLE_OBJECT_TYPE_ASTEROID == moving_object->get_object_type())
		{
			Net::send_packet_asteroid_collided((Asteroid *)moving_object);
		}

		if (MOVABLE_OBJECT_TYPE_ASTEROID == other->get_object_type())
		{
			Net::send_packet_asteroid_collided((Asteroid *)other);
		}
        return true;
    }

    return false;
}

void Application::CreateMissile(float x, float y, float w, int id)
{
	if (myMissile)
	{	// delete existing missile
		delete myMissile;
		myMissile = NULL;
	}

	// add a new missile based on the following parameter coordinates
	myMissile = new Missile("missile.png", x, y, w, id);
	myMissile->set_damage(10.f);

	// send new missile information to the server
	Net::send_packet_new_missile(myMissile);
}

void Application::CreateBoom(float x, float y, int id)
{
	// add a new boom based on the parameters
	Boom *boom = new Boom("boom.png", x, y, id);
	boom->set_render(true);
	boom->set_render_time(0.5f);

	mybooms_.push_back(boom);
	// send new boom information to the server
	Net::send_packet_render_boom(boom);
}

// This functions update HP on other client's screen
void Application::UpdateHP(Missile *missile)
{
	Net::send_packet_update_hp(missile);
}

// This function update HP on this client's screen
void Application::UpdateHP(Ship *ship, float damage)
{
	ship->Add_HP(-damage);
	if (ship->Get_HP() <= 0.f)
	{
		ship->set_render(false);
		ship->set_dead(true);
	}
}

void Application::Respawn(Ship *ship)
{
	Net::send_packet_respawn_ship(ship);
}

void Application::Heal(Powerup_Heal *powerup)
{
	Net::send_packet_powerup_heal(powerup);
}