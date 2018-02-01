#ifndef	_SCREEN_TEXT_H_
#define _SCREEN_TEXT_H_

#include "hge.h"
#include "non_movables.h"
#include <string>
class hgeFont;

/**
* Render text on screen
*/

class Screen_Text : public NonMovables
{
private:
	char text_[100];
	//std::string text_;

public: 
	std::auto_ptr<hgeFont> font_;

public:
	Screen_Text();
	~Screen_Text();

public:
	void Render();

	void set_text(char *text);
};

#endif