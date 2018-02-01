#include "powerup_heal.h"
#include <hge.h>
#include <hgesprite.h>
#include <math.h>

Powerup_Heal::Powerup_Heal(char * filename, float x, float y, int powerupid, float healamt)
{
	HGE *hge = hgeCreate(HGE_VERSION);

	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0.0f, 0.0f, 30.0f, 30.0f));
	sprite_->SetHotSpot(15.0f, 15.0f);

	set_object_type(NON_MOVABLE_OBJECT_POWERUP_HEAL);

	set_x(x);
	set_y(y);

	set_powerupid(powerupid);
	set_healamt(healamt);
}

Powerup_Heal::~Powerup_Heal()
{
	HGE *hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

void Powerup_Heal::Render()
{
	if (!get_render())
		return;
	sprite_->Render(get_x(), get_y());
}