#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include "stm32f10x.h"
#include "stdbool.h"


typedef enum
{
	X=0,
	Y,
	Z,
	E,
	TOTAL_AXIS
}AXIS;


typedef struct
{
	float axis[TOTAL_AXIS];
	u32   feedrate;
}COORDINATE;

extern const char axis_id[TOTAL_AXIS];


bool  xyzGetRelative(void);
void  xyzSetRelative(bool mode);
bool  eGetRelative(void);
void  eSetRelative(bool mode);
float coordinateGetAxis(AXIS axis);
void  coordinateSetAxis(AXIS axis, float position);
u32   coordinateGetFeedRate(void);
void  coordinateSetFeedRate(u32 feedrate);


#endif
