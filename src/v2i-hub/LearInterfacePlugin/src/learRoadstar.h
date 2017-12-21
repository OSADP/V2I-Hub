/**************************************************************
 * Copyright (c) 2016-2017 Lear Corporation. All rights reserved.
 * Proprietary and Confidential Material.
 **************************************************************/

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <sys/types.h> 
#include <unistd.h>   
#include <math.h>
#include "dot3/lib16093.h"


#define GPS_PORT 5938
#define GPS_IP "224.0.0.1"
#define CAN_PORT 5939
#define CAN_IP "224.0.0.1"
#define SER_LEN 100
#define MICRO_SEC 1000

typedef enum
{
    AsmOpen = 1,
    AsmSign,
    AsmEncrypt,
    AsmVerify,
    AsmDecrypt,
    AsmNone
} AsmSecurityID;

struct confData
{
    uint32_t channelNo;
    int32_t txInterval;
    int8_t txPower;
    uint8_t dataRate;
    uint8_t dataPath;
    uint8_t noOfPkt;
    uint8_t security;
    uint8_t payLoadLen;
};

typedef struct  _appData
{
    int txCount;
    int txError;
    int txDrop;
    int rxCount;
    int32_t asmContext;
    uint32_t psid;
    libWmeContext wmeCtx;
    libWsmContext wsmCtx;
    struct confData cData;
} appData;

#endif
