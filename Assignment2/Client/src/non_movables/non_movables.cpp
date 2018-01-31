#include "non_movables.h"

float time = 0.f;

NonMovables::NonMovables()
{
	ZeroMemory( &value_, sizeof( value_ ));
}

NonMovables::~NonMovables()
{
}

bool NonMovables::Update(float timedelta)
{
	time += timedelta;
	if (time > value_.render_time)
	{
		time = 0.f;
		render = false;
		return false;
	}
}