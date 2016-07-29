#ifndef TOOL_DEF_H_
#define TOOL_DEF_H_

/***************************************************************************
 * 1. INCLUDES                                                             *
 ***************************************************************************/
#include "AsmDef.h"

/***************************************************************************
 * 2. DEFINES                                                              *
 ***************************************************************************/
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_DEBUG     2
#define LOG_LEVEL_INFO      3

#define ERROR(...)     log_output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...)     log_output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...)      log_output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)

//Network defines
#define IP_LEN      16
#define REMOTE_IP   "127.0.0.1"

/***************************************************************************
 * 3. DECLARATIONS                                                         *
 ***************************************************************************/

typedef enum {
	AsmOpen =0 ,
	AsmSign,
	AsmEncrypt,
	AsmVerify,
	AsmDecrypt
}AsmSecurityID;

typedef enum { 
    self = 0 , certificate_digest_with_ecdsap224,
    certificate_digest_with_ecdsap256,
    certificate, certificate_chain,
    certificate_digest_with_other_algorithm
}AsmSignerIdentifierType;


#define NO_SOCKET 0
#define TX_SOCKET 1
#define RX_SOCKET 2

#define WSA_QUEUE_DELAY 30
#endif /* TOOL_DEF_H_ */
