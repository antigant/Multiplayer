#include "screen_text.h"
#include <hge.h>

Screen_Text::Screen_Text()
{
}

Screen_Text::~Screen_Text()
{
	font_.release();
}

void Screen_Text::Render()
{
	font_.reset(new hgeFont("font1.fnt"));
	font_->SetScale(1.0f);
	font_->printf(get_x(), get_y(), HGETEXT_LEFT, "%s", text_);
}

void Screen_Text::set_text(char *text)
{
	strcpy(text_, text);
}
