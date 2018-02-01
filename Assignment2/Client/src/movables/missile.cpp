#include "missile.h"
#include "ship.h"
#include <hge.h>
#include <hgeSprite.h>
#include <math.h>

#define MAX_TIME 3.0f

Missile::Missile(char* filename, float x, float y, float w, int shipid ) 
{
	HGE* hge = hgeCreate(HGE_VERSION);

	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 20));
	sprite_->SetHotSpot(20,10);

    set_object_type( MOVABLE_OBJECT_TYPE_MISSILE );

    set_x( x );
    set_y( y );
    set_w( w );
    set_server_xyw( x, y, w );
    set_client_xyw( x, y, w );
    set_ratio( 1.0f );

    set_velocity_x( 200.0f * cosf( get_w() ) );
    set_velocity_y( 200.0f * sinf( get_w() ) );
    set_server_velocity_x( get_velocity_x() );
    set_server_velocity_y( get_velocity_y() );

//    set_x( get_x() + ( get_velocity_x() * 0.5f ) );
//    set_y( get_y() + ( get_velocity_y() * 0.5f ) );

    set_ownerid( shipid );

	set_damage(0.f);
}

Missile::~Missile()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

bool Missile::Update(float timedelta, float spritewidth, float spriteheight)
{
	// Don't update if rendering on screen
	if (!get_render())
		return false;

	set_render_time(get_render_time() + timedelta);
	if (get_render_time() > MAX_TIME)
	{
		set_render(false);
		return false;
	}

	// Calculate the movement.
	HGE* hge = hgeCreate(HGE_VERSION);
	float pi = 3.141592654f * 2;

	float prev_x = get_x();
	float prev_y = get_y();
	float prev_w = get_w();

	// change the way angular velocity is updated (by ratio)
	set_server_w(get_server_w() + (get_angular_velocity() * timedelta));
	if (get_server_w() > pi)   set_server_w(get_server_w() - pi);
	if (get_server_w() < 0.0f) set_server_w(get_server_w() + pi);

	set_client_w(get_client_w() + (get_angular_velocity() * timedelta));
	if (get_client_w() > pi)   set_client_w(get_client_w() - pi);
	if (get_client_w() < 0.0f) set_client_w(get_client_w() + pi);

	set_w((get_ratio() * get_server_w()) + ((1 - get_ratio()) * get_client_w()));
	if (get_w() > pi)   set_w(get_w() - pi);
	if (get_w() < 0.0f) set_w(get_w() + pi);

	float screenwidth = static_cast<float>(hge->System_GetState(HGE_SCREENWIDTH));
	float screenheight = static_cast<float>(hge->System_GetState(HGE_SCREENHEIGHT));

	set_server_x(get_server_x() + (get_server_velocity_x() * timedelta));
	set_server_y(get_server_y() + (get_server_velocity_y() * timedelta));

	// change the way movement is updated by using ratio.
	if (get_server_x() < -spritewidth / 2)
		set_server_x(get_server_x() + (screenwidth + spritewidth));
	else if (get_server_x() > screenwidth + spritewidth / 2)
		set_server_x(get_server_x() - (screenwidth + spritewidth));

	if (get_server_y() < -spriteheight / 2)
		set_server_y(get_server_y() + (screenheight + spriteheight));
	else if (get_server_y() > screenheight + spriteheight / 2)
		set_server_y(get_server_y() - (screenheight + spriteheight));

	set_client_x(get_client_x() + (get_velocity_x() * timedelta));
	set_client_y(get_client_y() + (get_velocity_y() * timedelta));

	if (get_client_x() < -spritewidth / 2)
		set_client_x(get_client_x() + (screenwidth + spritewidth));
	else if (get_client_x() > screenwidth + spritewidth / 2)
		set_client_x(get_client_x() - (screenwidth + spritewidth));

	if (get_client_y() < -spriteheight / 2)
		set_client_y(get_client_y() + (screenheight + spriteheight));
	else if (get_client_y() > screenheight + spriteheight / 2)
		set_client_y(get_client_y() - (screenheight + spriteheight));

	if (((get_server_x() < -spritewidth / 2) && (get_client_x() > (screenwidth + (spritewidth / 2)))) ||
		((get_server_x() > screenwidth + (spritewidth / 2)) && (get_client_x() < -spritewidth / 2)))
	{
		set_x(get_server_x());
	}
	else
	{
		set_x((get_ratio() * get_server_x()) + ((1 - get_ratio()) * get_client_x()));
	}

	if (((get_server_y() < -spriteheight / 2) && (get_client_y() > (screenheight + (spriteheight / 2)))) ||
		((get_server_y() > screenheight + (spriteheight / 2)) && (get_client_y() < -spriteheight / 2)))
	{
		set_y(get_server_y());
	}
	else
	{
		set_y((get_ratio() * get_server_y()) + ((1 - get_ratio()) * get_client_y()));
	}

	if (get_ratio() < 1)
	{
		// interpolating ratio step
		set_ratio(get_ratio() + (timedelta * 4));
		if (get_ratio() > 1) set_ratio(1);
	}

	if (get_x() < (-spritewidth / 2))
	{
		set_x(get_x() + (screenwidth + spritewidth));
	}
	else if (get_x() > (screenwidth + (spritewidth / 2)))
	{
		set_x(get_x() - (screenwidth + spritewidth));
	}

	if (get_y() < (-spriteheight / 2))
	{
		set_y(get_y() + (screenheight + spriteheight));
	}
	else if (get_y() > (screenheight + (spriteheight / 2)))
	{
		set_y(get_y() - (screenheight + spriteheight));
	}

	if ((get_x() != prev_x) || (get_y() != prev_y)) return true; // something updated.

	return false; // nothing updated.
}

//bool Missile::Update(std::vector<Ship*> &shiplist, float timedelta)
//{
//	HGE* hge = hgeCreate(HGE_VERSION);
//	float pi = 3.141592654f*2;
//	float oldx, oldy;
//
//	w_ += angular_velocity * timedelta;
//	if (w_ > pi)
//		w_ -= pi;
//
//	if (w_ < 0.0f)
//		w_ += pi;
//
//	oldx = x_;
//	oldy = y_;
//	x_ += velocity_x_ * timedelta;
//	y_ += velocity_y_ * timedelta;
//
//	for (std::vector<Ship*>::iterator thisship = shiplist.begin();
//		thisship != shiplist.end(); thisship++)
//	{
//		if( HasCollided( (*(*thisship)) ) )
//		{
//			return true;
//		}
//	}
//
//	
//	float screenwidth = static_cast<float>(hge->System_GetState(HGE_SCREENWIDTH));
//	float screenheight = static_cast<float>(hge->System_GetState(HGE_SCREENHEIGHT));
//	float spritewidth = sprite_->GetWidth();
//	float spriteheight = sprite_->GetHeight();
//	if (x_ < -spritewidth/2)
//		x_ += screenwidth + spritewidth;
//	else if (x_ > screenwidth + spritewidth/2)
//		x_ -= screenwidth + spritewidth;
//
//	if (y_ < -spriteheight/2)
//		y_ += screenheight + spriteheight;
//	else if (y_ > screenheight + spriteheight/2)
//		y_ -= screenheight + spriteheight;
//
//	return false;
//}

void Missile::Render()
{
	sprite_->RenderEx( get_x(), get_y(), get_w() );
}

//bool Missile::HasCollided( Ship &ship )
//{
//	sprite_->GetBoundingBox( x_, y_, &collidebox);
//
//	return collidebox.Intersect( ship.GetBoundingBox() );
//}