#ifndef __CAN_GDS_H
#define __CAN_GDS_H

typedef struct  GDSData {
    
	long exteriorLights;
        long ratefront;
        long raterear; 
	unsigned char eventFlag;       
        /* if required more parameters can be included */
}can_GDSData_t;

int get_can_data(void *,char *can_interface);

#endif

