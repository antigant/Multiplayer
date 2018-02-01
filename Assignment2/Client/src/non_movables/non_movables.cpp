#include "non_movables.h"

NonMovables::NonMovables()
	: render(true)
	, time_(0.0f)
{
	ZeroMemory( &value_, sizeof( value_ ));
}

NonMovables::~NonMovables()
{
}

bool NonMovables::Update(float timedelta)
{
	time_ += timedelta;
	if (time_ > value_.render_time)
	{
		render = false;
		return false;
	}
}