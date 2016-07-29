#include "common.h"

#ifndef SPAT_H_
#define SPAT_H_
/*************************************************************************************
*
*  spat.h   : SPAT Message Header File
*
*************************************************************************************/
/* Constants */
#define spat_maxlanes 20
#define spat_maxmovements 20

/* SPAT Enumerations */
enum spat_intersection_status {manual=1, stopped=2, flash=4, preempt=8, priority=16};
enum spat_pedestrian {unavailable=0, none=1, oneormore=2};

/* SPAT Structures */
struct spat_data
    {
    unsigned long state;
    unsigned int  mintime;
    unsigned int  maxtime;
    unsigned long yellowstate;
    unsigned int  yellowtime;
    unsigned char pedestrian;
    unsigned char count;
    };  
struct spat_movement 
    {
    unsigned char type;
    unsigned char lane[spat_maxlanes];
    spat_data data;
    };  
struct spat 
    {
    unsigned long intersectionid;
    unsigned char intersectionstatus;
    unsigned long timestampseconds;
    unsigned char timestamptenths;
    spat_movement movement[spat_maxmovements];
    bblob payload;
    };
    
/* SPAT Function Prototypes */   
int spat_initialize(spat*);
int spat_encode(spat*, unsigned char);
int spat_decode(spat*);
void spat_clear(spat*);

#endif /* SPAT_H_ */
