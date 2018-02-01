#ifndef	_NON_MOVABLES_H_
#define _NON_MOVABLES_H_

#include "hge.h"
#include "hgesprite.h"
#include "hgefont.h"
#include <vector>

/**
* The Non-Moveables class represents a super class for any non-moveables object
* such as explosive (boom) which will stay at the spot for t secs
*/

enum NON_MOVABLE_OBJECT_TYPE {
	NON_MOVABLE_OBJECT_TYPE_EMPTY = 0,

	NON_MOVABLE_OBJECT_BOOM,

	NON_MOVABLE_OBJECT_TYPE_MAX
};

class NonMovables
{
private:
	int object_type_;
	struct _values {
		// These variables are for local rendering
		float x_; //!< The x-coordinate
		float y_; //!< The y-coordinate
		float render_time; //!< Time it will be rendered onto the screen
	} value_;

	bool render;

public:
	NonMovables();
	~NonMovables();

public:
	int  get_object_type( void               ) { return object_type_;              }
	void set_object_type( int object_type    ) { this->object_type_ = object_type; }
																				   
	float get_x( void                        ) { return value_.x_;                 }
	void  set_x( float x                     ) { value_.x_ = x;                    }
																				   
	float get_y( void                        ) { return value_.y_;                 }
	void  set_y( float y                     ) { value_.y_ = y;                    }
																				   
	float get_render_time( void              ) { return value_.render_time;        }
	void  set_render_time( float render_time ) { value_.render_time = render_time; }

	bool  get_render( void                   ) { return render;                    }
	void  set_render( bool render            ) { this->render = render;            }

public:
	virtual bool Update( float timedelta );
};

#endif