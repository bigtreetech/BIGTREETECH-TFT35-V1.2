#ifndef _CONFIGRATION_H_
#define _CONFIGRATION_H_


enum
{
	 ENGLISH =0,
	 CHINESE,
	 RUSSIAN,
	 JAPANESE,
     ARMENIAN,
	 
	 TOTEL_LANGUAGE,	
};


#define V1_0 0
#define V1_1 1
#define V1_2 2


#define HARDWARE_VERSION      V1_2


#define NOZZLE_MINTEMP 0
#define BED_MINTEMP 0

#define NOZZLE_MAXTEMP 350
#define BED_MAXTEMP 150

#define DEFAULT_NOZZLE_TEMP	0
#define DEFAULT_BED_TEMP	0

#define PREVENT_COLD_EXTRUSION
#define EXTRUDE_MINTEMP 170


#define EXTRUDE_SLOW_SPEED	60
#define EXTRUDE_NORMAL_SPEED	600
#define EXTRUDE_FAST_SPEED	EXTRUDE_NORMAL_SPEED*2






#endif









