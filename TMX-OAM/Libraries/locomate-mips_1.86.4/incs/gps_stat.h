/*

* Copyright (c) 2005-2007 Arada Syatems, Inc. All rights reserved.

* Proprietary and Confidential Material.

*

*/

#ifndef __GPS_STAT_L_
#define __GPS_STAT_L_
#define	uint_8		unsigned __int8

#include<stdint.h>

typedef enum {
	LOCAL_GPS,
	TX_GPS,
	RX_GPS,
}GPS_PACKET;

#define GPS_STRSIZE             30

typedef struct  {
	double		actual_time;		//no. of sec from jan 1, 1970 00:00:00
        double          time;
        double          local_tod;
        uint64_t        local_tsf;
        double          latitude;
        char            latdir;
        double          longitude;
        char            longdir;
        double          altitude;
        char            altunit;
        double          course;
        double          speed;
        double          climb;
        double          tee;
        double          hee;
        double          vee;
        double          cee;
        double          see;
        double          clee;
        double          hdop;
        double          vdop;
        uint8_t         numsats;
        uint8_t          fix;
        double          tow;
        int             date;
        double          epx;
        double          epy;
        double          epv;

}__attribute__ ((packed)) GPSData;

int get_gps_contents();
void set_gpsmode_notWSMP();
int create_connect_sock();
int close_sock();
#endif
