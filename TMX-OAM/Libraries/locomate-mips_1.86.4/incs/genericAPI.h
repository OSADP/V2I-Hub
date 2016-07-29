#include "tool_def.h"

Time64 gen_getcurrentTime(void);
Time64 htonll(Time64);
int gen_readfile(const char* file, UINT8* buff, int buffsize);
unsigned int gen_htonl(unsigned int var);
unsigned short gen_htons(unsigned short var); 
unsigned int gen_ntohl(unsigned int var);
unsigned short genn_ntohs(unsigned short var);
void *gen_memcpy(void *to, const void *from, unsigned int n);
void log_output(const int level, const char* file, const int line, const char* fmt, ...);
int AsmConnect(int check, char *ip);
void AsmDisconnect(int txsocket_id,int rxsocket_id);
int AsmSend(char* buff, const int buffSize,int soid);
int AsmRecv(char* buff, const int buffSize,int soid);
int gpsConnect(void);
int gpsDisconnect(void);
int gpsSend(char* buff, const int buffSize);
int gpsRecv(char* buff, const int buffSize);
void msg_create_sign_msg(UINT8* buff, UINT8* buf,int* buffSize,int,UINT8,unsigned int *psid,int psidLen, UINT8 generationLocation,long generationLatitude_tx,long generationLongitude_tx);
void msg_create_verify_msg(const UINT8* signBuff, UINT8* buff, int* buffSize, UINT32 msgValidityDistance, UINT8 detectReplay, UINT8 generationLocationValidity,long local_latitude,long local_longitude);
void msg_create_enc_msg(UINT8* buff, UINT8* buf,int* buffSize,int);
void msg_create_dec_msg(const UINT8* encBuff, UINT8* buff, int* buffSize);
void msg_decode_dec_msg(UINT8* app_data,UINT8* decBuff,int* buffSize);
void msg_extract_Sign_OTA(UINT8* data,UINT8* signdata, int* recv_size);
void msg_extract_Enc_OTA(UINT8* data,UINT8* signdata, int* recv_size);
void msg_extract_Wsa_OTA(UINT8* data,UINT8* wsadata, UINT16* recv_size);
void msg_create_Wsa_OTA(UINT8* data,UINT8* recvdata,int datalength);
void msg_extract_Wsa_Verify_OTA(UINT8* data,UINT8* verifydata,UINT16* wsalength);
void msg_create_Sign_OTA(UINT8* data,UINT8* recvdata, int* recv_size,int datalength);
void msg_create_Enc_OTA(UINT8* data,UINT8* recvdata, int* recv_size,int datalength);
void msg_create_sign_wsa(UINT8* buff, UINT8* data, UINT16* buffSize,UINT16 length,UINT32 * ,UINT32 latitude,UINT32 longitude,UINT16 elevation);
void msg_create_verify_wsa(UINT8* signBuff, UINT8* buff, UINT16* buffSize,UINT32 * ,UINT32 latitude ,UINT32 longitude);
void msg_create_crl(UINT8* buff, int* buffSize);
void msg_create_cert_change(UINT8* buff, int* buffSize);
void msg_decode_sign_msg(UINT8* app_data,UINT8* signdata,int *recv_size,UINT8 *Algo,int send_size);
extern void msg_create_extract_data(const UINT8* signBuff, UINT8* buff, int* buffSize);
extern void msg_create_restart_msg(UINT8* buff, int* buffSize);
extern void msg_create_cert_info(UINT8* buff, int* buffSize);
#ifdef USE_LCM
#endif /* USE_LCM */
extern void msg_create_misbehavior_report(UINT8* buff, int* buffSize,UINT8* report_data,UINT32 report_data_length,long latitude,long longitude,UINT8* elevation);
extern void encode_length(UINT8 *addr ,SINT32 val , SINT32 *retIdx);
/* get value by length: cane be used for psid,legnth in security applications */
int getValbyLen(UINT8* addr,int *retIdx);
unsigned int  putPsidbyLen(UINT8* addr,unsigned int psid,int *retIdx);
unsigned int getPsidbyLen(UINT8* addr,int *retIdx);
#define DEFAULT_DEV_ADDR  "127.0.0.1" 
