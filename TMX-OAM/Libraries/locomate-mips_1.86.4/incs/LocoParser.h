#ifndef _LOCOPARSER_H_
#define _LOCOPARSER_H_


typedef struct{
    uint16_t TimeToContact;
    float LaneWidth;
    char filename[255]; 
    char msg1[100];
    char msg2[100];
    char msg3[100];
    char Display_Type[100];
    char SpatDisplay[30];
}LocomateOptions;

typedef struct{
    uint32_t mod_depl;
    uint16_t temp_id;
    char mount_time[50];
    uint16_t veh_type;
    uint16_t veh_length;
    uint16_t veh_width;
}LocomateVehicleSpecs;

#endif
       
