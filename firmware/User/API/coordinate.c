#include "coordinate.h"
#include "includes.h"

const char axis_id[TOTAL_AXIS]={'X','Y','Z','E'};

COORDINATE coordinate={{0.0,0.0,0.0,0.0},3000};

static bool relative_xyz = false;
static bool relative_e = false;

bool xyzGetRelative(void)
{
	return relative_xyz;
}
void xyzSetRelative(bool mode)
{
	relative_xyz = mode;
}
bool eGetRelative(void)
{
	return relative_e;
}
void eSetRelative(bool mode)
{
	relative_e = mode;
}


void coordinateSetAxis(AXIS axis,float position)
{
    bool r = (axis==axis_id[E]) ? relative_e : relative_xyz;
	if(r==false)
    {
        coordinate.axis[axis] = position;
    }
	else
    {
		coordinate.axis[axis] += position;
    }
}

void coordinateSetFeedRate(u32 feedrate)
{
	coordinate.feedrate=feedrate;
}

float coordinateGetAxis(AXIS axis)
{
	return coordinate.axis[axis];
}

u32 coordinateGetFeedRate(void)
{
	return coordinate.feedrate;
}

