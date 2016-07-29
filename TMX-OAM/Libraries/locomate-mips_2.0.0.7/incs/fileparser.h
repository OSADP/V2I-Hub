#ifndef _FILEPARSER_H_
#define _FILEPARSER_H_


typedef struct{
    uint8_t MessageType_rse;
    uint8_t priority_rse;
    uint8_t SecurityType_rse;
    uint16_t payload_size;
    uint8_t payload[512];
    char actfile[255];
    uint32_t app_psid_rse;
    uint32_t pktdelaymsecs_rse;
    uint32_t bcastintrvl_rse;
    uint32_t start_utctime_sec;
    uint32_t stop_utctime_sec;
    int ChannelAcess_rse;
    int txChan_rse;
}ActiveMsg;

#endif
