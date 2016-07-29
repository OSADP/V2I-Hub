#ifndef __PATHHIST_DEFS_H__
#define  __PATHHIST_DEFS_H__

#define RADIUS_OF_EARTH_AT_PRIME_MERIDIAN 6376115 
#define PH_CHORD_LENGTH_THRESHOLD 310 
#define PH_MIN_DELTA_HEADING 0.0
#define PH_ALLOWABLE_ERROR 0.80 
#define PH_PER_POINT_SIZE 8 
#define PH_MAX_DISTANCE 300.0
#define PH_MAX_POINTS 23
#define PH_MAX_ROC 0x7FFFF

struct gps_datapoint_php4 {
    double lat;
    double lon;
    double elev;
    double time;
    double distPrev;
    double course;
};

typedef struct gps_datapoint_php4 ElemType;
#include "cb.h"

#endif

