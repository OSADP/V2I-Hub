/*

* Copyright (c) 2005-2007 Arada Syatems, Inc. All rights reserved.

* Proprietary and Confidential Material.

*

*/

#ifndef __GPS_STAT_L_
#define __GPS_STAT_L_
#define	uint_8		unsigned __int8

#ifndef WIN32
#include<stdint.h>
#endif

#include<sys/types.h>

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

typedef struct  {
		uint32_t		actual_time;		//no. of sec from jan 1, 1970 00:00:00
        uint32_t        time;
        uint64_t        local_tod;
        uint64_t        local_tsf;
        long     	    latitude;
        char            latdir;
        long            longitude;
        char            longdir;
        uint16_t        altitude;
        char            altunit;
        uint16_t        course;
        uint16_t        speed;
        uint64_t        climb;
        uint64_t        tee;
        uint64_t        hee;
        uint64_t        vee;
        uint64_t        cee;
        uint64_t        see;
        uint64_t        clee;
        uint64_t        hdop;
        uint64_t        vdop;
        uint8_t         numsats;
        uint8_t         fix;
        uint64_t        tow;
        uint32_t        date;
       	uint64_t        epx;
        uint64_t        epy;
        uint64_t        epv;
}__attribute__ ((packed)) GPSSAEData;


int get_gps_contents();
void get_gps_status(GPSData *, char *);
void set_gpsmode_notWSMP();
int gpsc_connect(char *);
int gpsc_close_sock();
#endif
