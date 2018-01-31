#include "boom.h"
#include <hge.h>
#include <hgesprite.h>
#include <math.h>

Boom::Boom(char * filename, float x, float y, int ownerid_)
{
	HGE *hge = hgeCreate(HGE_VERSION);

	tex_ = hge->Texture_Load(filename);
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 40, 40));
	sprite_->SetHotSpot(20, 20);

	set_object_type( NON_MOVABLE_OBJECT_BOOM );

	set_x(x);
	set_y(y);
	
	set_ownerid(ownerid_);
}

Boom::~Boom()
{
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();
}

void Boom::Render()
{
	if (!get_render())
		return;
	sprite_->Render(get_x(), get_y());
}