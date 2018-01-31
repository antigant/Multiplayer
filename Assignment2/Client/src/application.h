#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "movables\ship.h"
#include "movables\Asteroid.h"
#include "movables\missile.h"
#include <vector>

enum _GAMESTATE {
    GAMESTATE_NONE = 0,
    GAMESTATE_INITIALIZING,
    GAMESTATE_INPLAY,
    GAMESTATE_QUITGAME
};

class HGE;

//! The default angular velocity of the ship when it is in motion
static const float DEFAULT_ANGULAR_VELOCITY = 3.0f; 
//! The default acceleration of the ship when powered
static const float DEFAULT_ACCELERATION = 50.0f;

/**
* The application class is the main body of the program. It will
* create an instance of the graphics engine and execute the
* Update/Render cycle.
*
*/
class Application
{
    private:
        int gamestate_;
    	HGE* hge_; //!< Instance of the internal graphics engine

        Ship *myship_; //!< My own space ship
        typedef std::vector<Ship*> ShipList;  //!< A list of ships
        ShipList enemyships_; //!< List of all the enemy ships in the universe

        typedef std::vector<Asteroid *> AsteroidList; //!< A list of asteroids
        AsteroidList asteroids_; //!< List of all the asteroids in the universe

		typedef std::vector<Missile*> MissileList;
		MissileList enemymissiles_;

		Missile *myMissile;
		bool have_missile;
		bool keydown_enter;

		// Add relevant variables for explosion, typedef std::vector<Boom*> BoomList; etc..

    	bool Init();
    	static bool Loop();
    	void Shutdown();

    public:
    	Application();
    	~Application() throw();

    	void Start();
    	bool Update();
    	void Render();

        template <typename T1, typename T2>
        bool HasCollided( T1 &object, T2 &movable );

        template <typename Mov, typename Tgt>
        bool CheckCollision( Mov &object, Tgt &movable, float timedelta );

		void CreateMissile(float x, float y, float w, int id);

    public:
        void SetGameState( int gamestate ) { gamestate_ = gamestate; }
        int  GetGameState( void          ) { return gamestate_; }

    public:
        Ship *GetMyShip( void ) { return myship_; }
        ShipList *GetEnemyShipList( void ) { return &enemyships_; }
        Ship *FindEnemyShip( int ShipID );
        AsteroidList *GetAsteroidList( void ) { return &asteroids_; }
		MissileList *GetEnemyMissileList() { return &enemymissiles_; }
};

#endif
