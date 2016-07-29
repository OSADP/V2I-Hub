#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/syslog.h>
#include <sys/time.h>
#include <netinet/in.h> // for htons()
#include <arpa/inet.h> //for inet_addr()
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <netdb.h>
#include <sys/mman.h>
#include <endian.h>
#include <byteswap.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/bn.h"
#include "openssl/ecdsa.h"
#include "openssl/aes.h"
#include "openssl/ssl.h"
#include "queue.h"
#include <math.h>


/***************************************************************************
 * 2. DEFINES                                                              *
 ***************************************************************************/

#define STDIN 0
#define STDOUT 1
#define HASHSIZE 256
#define SUCCESS 0
#define FAILURE -1
#define TRUE 1
#define FALSE 0
#define HASH_ALGO_SHA224 1
#define HASH_ALGO_SHA256 2
#define HASH_ALGO_ECDSA 3
#define CIPHER_ALGO_BASE 200
#define CIPHER_ALGO_AES128_CCM (CIPHER_ALGO_BASE + 1)
#define CIPHER_ALGO_AES128_ECB (CIPHER_ALGO_BASE + 2)
#define CIPHER_ALGO_AES128_CBC (CIPHER_ALGO_BASE + 3)
#define SHA256_DIGEST_LENGTH 32
#define SHA224_DIGEST_LENGTH 28
#define ECDSA_DIGEST_LEN 20
#define AWSEC_DBG
#ifdef AWSEC_DBG
#define AWSec_Print printf
#else
#define AWSec_Print(fmt, args...)
#endif
#define SEC_LOOPBACK_PORT 50000
#define AWSEC_SCHEDULING_PRIORITY -19
#define ECDSA_NISTP_224 1
#define ECDSA_NISTP_256 2
#define NUMBER_OF_LEAP_MICROSECONDS_TODAY 34000000
#define NUMBER_OF_LEAP_SECONDS_TODAY 34
#define MAX_CIPHERTEXT_LENGTH 243
#define MAX_NUM_ENTRIES 288
#define DAY_SEC 86400 //number of seconds in a day(24hrs)
#define YEAR 1325376000
#define DIGESTLEN 8
#define NO_OF_PSID_PERMISSION 6
#define REFERENCE_TIME 1072915234
#define NUMSECS_PERHOUR 3600
#define BIGENDIAN       \
( {                 \
    long x = 0x00000001;    \
    !(*(char *)(&x));   \
})
# if __BYTE_ORDER != __LITTLE_ENDIAN
#  define htobe64(x) (x)
#endif
#define be64toh(x) bswap_64 (x)
#define ECIES_CURVE NID_X9_62_prime256v1
#define ECIES_CIPHER EVP_aes_128_ecb()
#define ECIES_HASHER EVP_sha256()
#define NAZ_EVP_MAX_IV_LENGTH 12
#define LCM_CONF_FILE "/tmp/usb/lcm/lcm.conf"
#define CRLPORT 4445
#define BUFSIZE 100
#define ASM_CONF_FILE "/tmp/security.conf"
#define INCR_ENTRY(x) x+=1
#define DECR_ENTRY(x) x-=1
#define IS_CERT_REVOKED 0x01
#define IS_CERT_VALID 0x02
#define IS_CERT_CHANGE_RECEIVED 0x04
/* log file related macro */
#define MB 1048576
#define LCM_LOG_SIZE 10*MB
#define LCM_LOG_DEBUG_SIZE 10*MB

/* Request Command */
#define CMD_SIGN_POST                                       0x01
#define CMD_VERIFY_POST                                     0x02
#define CMD_ENC_POST                                        0x03
#define CMD_DEC_POST                                        0x04
#define CMD_SIGN_WSA_POST                                   0x05
#define CMD_VERIFY_WSA_POST                                 0x06
#define CMD_CRL_POST                                        0x07
#define CMD_CERT_CHG_POST                                   0x08
#define CMD_RESTART                                         0x09
#define CMD_MSG_DATA_EXT                                    0x0A
#define CMD_CERT_INFO                                       0x0B
#define CMD_MISBEHAVIOR_REPORT                              0x0C
#define CMD_IMPORT_CERTS                                    0x0D
#define CMD_SIGN_AND_ENCRYPT                                0x0E  
#define CMD_RELOAD_CERT         			    0x0F
/* Success Result */
#define CMD_OK_SIGN_POST                                    0x11
#define CMD_OK_VERIFY_POST                                  0x12
#define CMD_OK_ENC_POST                                     0x13
#define CMD_OK_DEC_POST                                     0x14
#define CMD_OK_SIGN_WSA_POST                                0x15
#define CMD_OK_VERIFY_WSA_POST                              0x16
#define CMD_OK_CRL_POST                                     0x17
#define CMD_OK_CERT_CHG_POST                                0x18
#define CMD_OK_Restart_POST                                 0x19
#define CMD_OK_MSG_DATA_EXT                                 0x1A
#define CMD_OK_VERIFY_POST_BUT_UNVERIFIED                   0x20
#define CMD_OK_ENC_POST_BUT_FAIL_ON_SOME_CERTS              0x21
#define CMD_OK_VERIFY_WSA_POST_BUT_UNVERIFIED               0x22
#define CMD_OK_CERT_CHG_POST_BUT_FAIL_ON_SOME_CERTS         0x23
#define CMD_OK_CERT_INFO                                    0x1B
#define CMD_OK_MISBEHAVIOR_REPORT                           0x1C
#ifdef USE_LCM
#define CMD_LCM_STATUS_RDY                                  0x0D //cycurV2X to OBE
#define CMD_LCM_STATUS_CERT_CHANGED                         0x1D //cycurV2X to OBE
#endif /* USE_LCM */
#define CMD_OK_RELOAD_CERT				    0X1E	

/* Error Result */
#define CMD_ERR_INVALID_INPUT                               0x30
#define CMD_ERR_NO_KEY_FOUND                                0x31
#define CMD_ERR_CERT_EXPIRED                                0x32
#define CMD_ERR_CERT_NOT_YETVALID                           0x5A
#define CMD_ERR_CERT_IN_CHAIN_EXPIRED                       0x33
#define CMD_ERR_CERT_IN_CHAIN_REVOKED                       0x34
#define CMD_ERR_CERT_NOT_FOUND                              0x35
#define CMD_ERR_INCORRECT_SIGNING_CERT_TYPE                 0x36
#define CMD_ERR_MESSAGE_EXPIRED_BASED_ON_GENERATION_TIME    0x37
#define CMD_ERR_MESSAGE_FUTURE_MESSAGE                      0x59 //Future message
#define CMD_ERR_MESSAGE_EXPIRED_BASED_ON_EXPIRY_TIME        0x38
#define CMD_ERR_MESSAGE_IS_REPLAY                           0x39
#define CMD_ERR_MESSAGE_OUT_OF_RANGE                        0x3A
#define CMD_ERR_COULD_NOT_CONSTRUCT_CHAIN                   0x3B
#define CMD_ERR_CHAIN_ENDED_AT_UNKNOWN_ROOT                 0x3C
#define CMD_ERR_INCORRECT_CA_CERT_TYPE                      0x3D
#define CMD_ERR_INCONSISTENT_CERT_SUBJECT_TYPE              0x3E
#define CMD_ERR_INCONSISTENT_PERMISSIONS                    0x3F
#define CMD_ERR_UNAVAILABLE_PERMISSIONS                     0x40    /* don't use any more */
#define CMD_ERR_INCONSISTENT_GEOGRAPHIC_SCOPE               0x41
#define CMD_ERR_UNAVAILABLE_GEOGRAPHIC_SCOPE                0x42    /* don't use any more */
#define CMD_ERR_REVOKED_CERT                                0x43
#define CMD_ERR_NO_UPDATE_CRL                               0x44
#define CMD_ERR_CERT_VERIFICATION_FAILED                    0x45
#define CMD_ERR_UNAUTHORIZED_PSID                           0x46
#define CMD_ERR_UNAUTHORIZED_GENERATION_LOCATION            0x47
#define CMD_ERR_MESSAGE_VERIFICATION_FAILED                 0x48
#define CMD_ERR_FAIL_ON_ALL_CERTS                           0x49
#define CMD_ERR_NO_DECRYPTION_CERT_FOUND                    0x4A
#define CMD_ERR_EXPIRED_DECRYPTION_CERT                     0x4B
#define CMD_ERR_REVOKED_DECRYPTION_CERT                     0x4C
#define CMD_ERR_COULD_NOT_DECRYPT_KEY                       0x4D
#define CMD_ERR_COULD_NOT_DECRYPT_MESSAGE                   0x4E
#define CMD_ERR_GEOGRAPHIC_SCOPE_ERROR                      0x4F
#define CMD_ERR_UNSUPPORTED_SIGNER_TYPE                     0x50
#define CMD_ERR_NOT_MOST_RECENT_WSA                         0x51
#define CMD_ERR_UNAUTHORIZED_PSID_AND_PRIORITY              0x52
#define CMD_ERR_INVALID_CRL                                 0x53
#define CMD_ERR_COULD_NOT_CHANGE_CERTS                      0x54
#define CMD_ERR_INVALID_ALGORITHM                           0x55
#define CMD_ERR_TIME_OUT                                    0x56
#define CMD_ERR_QUEUE_OVERFLOW                              0x57
#define CMD_ERR_INVALID_LEN                                 0x58
#define CMD_ERR_NO_LCM                                      0x5B
#define CMD_ERR_UNSECURED_WSA_PERMISSION                    0x5C
#define CMD_ERR_CERT_LOAD_FAILED                            0x5D
#define CMD_ERR_INVALID_PRIVATE_KEY_PROVIDED                0x5F
#define CMD_ERR_GENERAL                                     0xFF
#define CMD_CERTIFICATE_FOUND_NOT_VERIFIED                  0x60
#define CMD_ERR_KEY_MATERIAL_GENERATION_FAILED              0x61
#define CMD_ERR_INVALID_DATA_FORMAT                         0x62
#define CMD_ERR_INVALID_PACKET                              0x63

/*LOG-MODE'S*/
#define ASM 2
#define LCM 1
/*LOG-LEVELS*/
#define LOG_DISABLE  0    /*DO NOT LOG AT ALL*/
#define LOG_CRITICAL 1
#define LOG_WARN     2    /*not error but may indicate a problem */
#define LOG_INF      3    /*prints helpful in debugs*/
#define LOG_DEBUG    4    /*log buffer provided*/



/***************************************************************************
 * 3. DECLARATIONS                                                         *
 ***************************************************************************/

/* Standard data types */
typedef unsigned char       BOOL;
typedef unsigned char       UINT8;
typedef signed char         SINT8;
typedef unsigned short      UINT16;
typedef signed short        SINT16;
typedef unsigned long       UINT32;
typedef signed long         SINT32;
typedef unsigned long long  UINT64;
typedef signed long long    SINT64;
typedef void                VOID;
typedef int                 INT32;

/* 1609 data types */
typedef UINT32              Time32;
typedef UINT64              Time64;
typedef char secure_t;
char lcmlogfilename[30];


typedef struct 
{
  uint32_t WSApsid[NO_OF_PSID_PERMISSION];
  uint32_t WSApriority[NO_OF_PSID_PERMISSION];
  uint32_t WSAsspString[NO_OF_PSID_PERMISSION];
} WSApsidPermission;

struct SelfCertData
{
  char pCertInfo[512];
  char pPubKey[33];
  char pPrivKey[32];
  int i;
  int j;
  LIST_ENTRY(SelfCertData) certList;
};

ATH_LIST_HEAD(selfcert, SelfCertData) selfCertHead;
typedef struct ECKeyData_s {
  unsigned char pPrivKey[32];
  unsigned char pSignR[32];
  unsigned char pSignS[32];
  unsigned char pPublicKey[1024];
  UINT32 uiPubKeyLen;
} ECKeyData_t;

typedef struct {

        struct {
                uint64_t key;
                uint64_t mac;
                uint64_t orig;
                uint64_t body;
        } length;

} secure_head_t;

typedef struct {
    int32_t latitude;
    int32_t longitude;
} TwoDLocation;
typedef struct
{
    Time64  time;
    uint8_t confidence;
} Time64WithConfidence;

typedef struct
{
    SINT32  latitude;
    SINT32  longitude;
    UINT8   elevation[2];
} ThreeDLocation;

#pragma pack(1)

typedef struct
{
    BOOL    perform_cryptographic_verification;
    BOOL    detect_replay;
    BOOL    require_generation_time;    // check validity based on generation time
    UINT64  message_validity_period;
    Time64WithConfidence generation_time;
    UINT32  generation_time_confidence_multiplier;     // x1000
    BOOL    use_expiry_time;    // check validity based on expiry time
    Time64  expiry_time;
    BOOL    require_generation_location;
    UINT32  message_validity_distance;
    ThreeDLocation generation_location;
    SINT32  local_location_latitude;
    SINT32  local_location_longitude;
    UINT32  overdue_CRL_tolerance;
} AWSecMsg_Verify_Msg_Req2_t;

typedef struct
{
    BOOL sign_with_fast_verification;
    UINT64  lifetime;
    SINT32  generation_location_latitude;
    SINT32  generation_location_longitude;
    UINT8   generation_location_elevation[2];
    UINT16  number_of_permissions;
    UINT8   permissions[1];     // start point
}  AWSecMsg_Sign_WSA_Req2_t;

typedef struct
{
    BOOL    perform_cryptographic_verification;
    BOOL    detect_replay;
    BOOL    use_message_validity_period;
    UINT64  message_validity_period;
    UINT32  generation_time_confidence_multiplier;     // x1000
    BOOL    use_message_validity_distance;
    UINT32  message_validity_distance;
    SINT32  local_location_latitude;
    SINT32  local_location_longitude;
    UINT32  overdue_CRL_tolerance;
    BOOL    perform_permissions_check;
    UINT16  number_of_permissions;
    UINT8   permissions[1];     // start point
} AWSecMsg_Verify_WSA_Req2_t;

typedef struct  
{                   
    UINT8   command;
    UINT32  application_data_length;
    UINT8   application_data[1];    // start point
} AWSecMsg_To_Be_Sign_and_Encrypted;

#pragma pack(0)

typedef struct{
    uint32_t certReqTime;
    uint32_t endReqTime; //super batch end time
    uint32_t DkReqTime;
    uint32_t cfmOffset;
    uint32_t cfmOffset_next; //Next status_cfm
    uint32_t batchOffset;
    int32_t SLClen; //length of short_livid_certs<var>
    int32_t SLClen_next; //Next status_cfm
    int32_t SMlen; //length of SignedData<var>
    int32_t fallBackLen; //length of fallBackcerts
    Time32 nextCrl;
}LCM_store;


typedef struct {
    uint8_t certInfo[512];
    int16_t certLen;
    uint8_t version_and_type;
    uint8_t subject_type;
    uint8_t cf;
    uint8_t signature_alg;
    uint16_t signer_id;
    int16_t ad_len; //additional_data length
    uint16_t ad;//additional_data
    int16_t scopelen;
    uint16_t scope;
    uint16_t psid_array_permLength;
    uint16_t psid_array_permOffset;
    uint16_t expiration;
    uint16_t start_validity_or_lft;
    uint16_t crl_series;
    uint16_t verification_key;
    uint16_t geographic_length;  
    uint16_t geographic_region;
    uint16_t encryption_key;
    uint16_t sig_RV;
} Certificate;


typedef uint16_t  CertificateDuration;
typedef struct{
    uint8_t certCheckFlag;
    Time32 expiration;
    CertificateDuration lifetime;
    Certificate slotCert;
    Time32 lastRecvCrl;
    Time32 nextRecvCrl;
    uint8_t hash[8];
    uint8_t privSignKey[32];
    uint8_t pubSignKey[33];
    uint8_t privEncKey[32];
}certStore;

certStore CACertStore, RACertStore, CSRCertStore;
struct CertInfo
{
  uint32_t hashIndex;
  char SignerID[8];
  char CertHash[8];
  char pPubKey[33];
  TAILQ_ENTRY(CertInfo) si_list;
  LIST_ENTRY(CertInfo) si_hash;
};

struct Cert_hash_table
{
  TAILQ_HEAD(st_cert, CertInfo) st_cert;
  ATH_LIST_HEAD(st_hash, CertInfo) st_hash[HASHSIZE];
};

struct Cert_hash_table TrustCert;

struct tempCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(tempCertInfo) temp_list;
};

ATH_LIST_HEAD(tempListHead, tempCertInfo) tempListHead;

struct issuerCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(issuerCertInfo) issuer_list;
};

ATH_LIST_HEAD(issuerListHead, issuerCertInfo) issuerListHead;

struct WSAissuerCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(WSAissuerCertInfo) WSAissuer_list;
};

ATH_LIST_HEAD(WSAissuerListHead, WSAissuerCertInfo) WSAissuerListHead;

struct SLCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(SLCertInfo) SL_list;
};

ATH_LIST_HEAD(SLListHead, SLCertInfo) SLListHead;

struct TIMCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(TIMCertInfo) TIM_list;
};

ATH_LIST_HEAD(TIMListHead, TIMCertInfo) TIMListHead;

struct SPATCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(SPATCertInfo) SPAT_list;
};

ATH_LIST_HEAD(SPATListHead, SPATCertInfo) SPATListHead;

struct GIDCertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(GIDCertInfo) GID_list;
};

ATH_LIST_HEAD(GIDListHead, GIDCertInfo) GIDListHead;

struct ENC1CertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(ENC1CertInfo) ENC1_list;
};

ATH_LIST_HEAD(ENC1ListHead, ENC1CertInfo) ENC1ListHead;

struct WSACertInfo
{
  Time32 processTime;
  certStore pCertInfo;
  LIST_ENTRY(WSACertInfo) WSA_list;
};

ATH_LIST_HEAD(WSAListHead, WSACertInfo) WSAListHead;

struct SelfCertInfo
{
  uint32_t hashIndex;
  char SignerID[8];
  char SelfCertHash[8];
  char pPrivKey[32];
  char pPubKey[33];
  TAILQ_ENTRY(SelfCertInfo) si_list;
  LIST_ENTRY(SelfCertInfo) si_hash;
};

struct SelfCert_table
{
  TAILQ_HEAD(st_selfcert, SelfCertInfo) st_selfcert;
  ATH_LIST_HEAD (self_certs, SelfCertInfo) self_certs[HASHSIZE];
};

struct SelfCert_table SelfCert;

/**********GENERATION LOCATION COMPUTATION RELATED***********/
typedef struct{
    ThreeDLocation generation;
    SINT32 local_latitude;
    SINT32 local_longitude;//use variable or use htonl() call after computation for location
    double distance;
}location;

struct ThreadInfo
{
  int socketFd;
  unsigned long threadId;
  LIST_ENTRY(ThreadInfo) ThreadList;
};

ATH_LIST_HEAD(ThreadListHead, ThreadInfo) ThreadListHead;
#ifdef CRLMANAGE
struct certIdInfo
{
  uint8_t signerId[3][8]; 
};
#endif

typedef enum
	{
	self=0,
	certificate_digest_with_ecdsap224=1,
	certificate_digest_with_ecdsap256=2,
	certificate=3,
	certificate_chain=4,
	certificate_digest_with_other_algorithm=5,
	}SignerIdentifierType;

typedef enum 
	{ 
	message_anonymous=0, 
	message_identified_not_localized =1,
	message_identified_localized =2,
	message_csr = 3,
	wsa =4,
	wsa_csr =5,
	message_ca=6, 
	wsa_ca =7, 
	crl_signer=8,
	message_ra =9,
	root_ca=255
	} SubjectType;

typedef enum
        {
        GID=0,
        TIM=1,
        SPAT=2,
        WSA=3,
        ENC1=4,
        ENC2=5,
        SLIVED=6,
        FBACK=7,
        }certType;
        
        
typedef enum
	{ 
	use_start_validity =0,
	lifetime_is_duration=1,
	encryption_key=2
	}CertificateContentFlags;

typedef enum
	{
	message_anonymous_flags=0,
	message_identified_not_localized_flags=1,
	message_identified_localized_flags=2,
	message_csr_flags=3,
	wsa_flags =4,
	wsa_csr_flags=5,
	message_ca_flags=6,
	wsa_ca_flags=7,
	crl_signer_flags =8,
	message_ra_flags =9
	}SubjectTypeFlags;

typedef enum 
	{
	from_issuer=0, 
	specified=1
	} ArrayType;

typedef enum 
	{ 
	ecdsa_nistp224_with_sha224 =0,  
	ecdsa_nistp256_with_sha_256 =1, 
	ecies_nistp256 =2
	} PKAlgorithm;

typedef enum 
	{ 
	x_coordinate_only =0,
	compressed_lsb_y_0 =1,
	compressed_lsb_y_1 =2, 
	uncompressed =3
	} EccPublicKeyType;

typedef enum 
	{
	unsecured=0, 
	Signed=1, 
	encrypted=2, 
	certificate_request=3, 
	certificate_response=4, 
	anonymous_certificate_response=5, 
	certificate_request_error=6, 
	crl_request=7, 
	crl=8, 
	signed_partial_payload=9, 
	signed_external_payload=10, 
	signed_wsa=11
	, certificate_response_acknowledgment=12, 
	crl_req =236, 
	crl_req_error =237, 
	misbehavior_report_req =238, 
	misbehavior_report_ack =239, 
	cert_bootstrap_req =240, 
	cert_bootstrap_cfm =241, 
	cert_bootstrap_ack =242, 
	anonymous_cert_request_req =243, 
	anonymous_cert_request_cfm =244, 
	anonymous_cert_request_status_req =245, 
	anonymous_cert_request_status_cfm =246, 
	sig_enc_cert =247, 
	certificate_and_private_key_reconstruction_value =248, 
	anonymous_cert_response_ack =249, 
	anonymous_cert_decryption_key_req =250, 
	anonymous_cert_decryption_key_cfm =251, 
	anonymous_cert_decryption_key_error =252, 
	anonymous_cert_decryption_key_ack =253,
	symmetric_encrypted=254
	}ContentType;

typedef enum 
	{
	from_issuer_region =0, 
	circle =1, 
	rectangle =2, 
	polygon =3, 
	none =4
	} RegionType;

typedef enum 
	{
	id_only=0, 
	id_and_expiry=1, 
	anonymous_entry=2
	} CrlType;

typedef struct KeyInfo_s {
  UINT8 pBootStrapKeyPriv[32];
  UINT8 pBootStrapKeyPub[33];
  UINT8 pStaticKeyPriv[32];
  UINT8 pStaticKeyPub[33];
  UINT8 pSignKeyPriv[32];
  UINT8 pSignKeyPub[33];
  UINT8 pEncKeyPriv[32];
  UINT8 pEncKeyPub[33];
  UINT8 pAESSignKey[16];
  UINT8 pAESEncKey[16];
  UINT8 pSignKeyPriv_Next[32];
  UINT8 pEncKeyPriv_Next[32];
  UINT8 pAESSignKey_Next[16];
  UINT8 pAESEncKey_Next[16];
  UINT8 pAESSignNonce[12];
  UINT8 pAESEncNonce[12];
/*  UINT8 CACertInfo[512];
  UINT32 CACertPubOffset;
  UINT32 CACertEncPubOffset;
  UINT8 RACertInfo[512];
  UINT32 RACertPubOffset;
  UINT32 RACertEncPubOffset;
  UINT8 ImplicitCertInfo[512];
  UINT32 ICCertPubOffset;
  UINT32 ICCertEncPubOffset;*/
  UINT8 pCACertID[8];
  UINT8 DigestInfo[32];
  UINT8 recon_priv[32];
  UINT8 pAESKeyStatusReq[16];
  UINT8 pAESKeyDecryptKeyReq[16];
  UINT8 pStatusReqDigestInfo[32];
  UINT8 pStatusRespDigestInfo[32];
  UINT8 pDecryptDigestInfo[32];
  UINT8 pMisBehaveDigestInfo[32];
  UINT8 pCrlDigestInfo[32];
  UINT8 pDecryptAESKey[16];
  UINT8 pCertBatchID[8];

} KeyInfo_t;
KeyInfo_t SaveKeys;



struct lcm_conf_options
{
    char RA_ADDRESS[25];
    int RA_PORT;
    int PSID;
    int Storage_Space;
    int Bootstrap_Request_Timeout;
    char Device_Specific_ID[64];
    uint16_t Batch_Duration_Units;
    uint16_t Batch_Duration_Value;
    uint16_t Superbatch_Duration_units;
    uint16_t Superbatch_Duration;
    int Certificate_Request_Status_Inquiry_Interval;
    int Certificate_Request_Confirmation_Timeout;
    int Decryption_Key_Request_Interval;
    int Maximum_Certificate_Storage_Time;
    int Request_Certificates_Time;
    int Request_Decryption_Key_Time;
    int Default_Timeout;
    char LCM_NAME[4];
    int Connection_Retry_Interval;
    int LCMLogEnable;
    char LogFileDirectory[256];
    char LogUseSimpleName[256];
    uint32_t totalLogFlag;
    uint8_t LogEnableAdditionalInfo;
    uint8_t Log_Bootstrap_Request;
    uint8_t Log_Bootstrap_Confirm;
    uint8_t Log_Bootstrap_Ack;
    uint8_t Log_CertRequest_Req;
    uint8_t Log_CertRequest_Confirm;
    uint8_t Log_CertStatus_Req;
    uint8_t Log_CertStatus_Confirm;
    uint8_t Log_CertStatus_Confirm_Data;
    uint8_t Log_CertResponse_Ack;
    uint8_t Log_DecryptKey_Request;
    uint8_t Log_DecryptKey_Confirm;
    uint8_t Log_DecryptKey_Ack;
    uint8_t Log_SignEncrypt_Input;
    uint8_t Log_SignEncrypt_Before_Encrypt;
    uint8_t Log_SignEncrypt_After_Encrypt;
    uint8_t Log_Imported_File;
    int DecryptKey_Retry;
    char Lcm_DebugFile[255];
    uint8_t crlEnable;
};


struct asm_conf_options
{
    int SM_SOCKET_TYPE;
    int SM_PORT_NUMBER;
    char SM_SOCKET_PATH[30];
    int MSG_TOLERANCE_TIME;
    int CERT_CHECK_PERIOD;
    int MSG_QUEUE_TIME;
    uint16_t MSG_FRAG_TIME_OUT;
    uint16_t USE_CRL_FILE;
    char CRL_FILE_NAME[30];
    uint16_t LOG_OUTPUT_STREAM;
    int LOG_FILE_SIZE;
    char LOG_FILE_NAME[30];
    uint16_t SET_SYSTEM_TIME;
    int LOCAL_TIME_CONFIDENCE;
    int LOCAL_LOCATION_CONFIDENCES;
    uint16_t MAX_CERT_CHAIN_LENGTH;
    char KEY_CONF_DIR[256];
    uint16_t GEOGRAPHIC_CONSISTENCY_CHECK;
    uint32_t DEVICE_MODE;
    int RELOAD_PERIOD;
    int FPGA_DELAY_TIME1;
    int FPGA_DELAY_TIME2;
    int MSG_BUFFER_SIZE;
    int MSG_QUEUE_SIZE;
    int PROC_THREAD_COUNT;
    int FPGA_QUEUE_SIZE;
    uint16_t MSG_FRESHNESS_CHECK;
    uint16_t MSG_REPLAY_CHECK;
    uint16_t MSG_LOCATION_CHECK;
    uint16_t TCP_MAX_PEND_CONNECTION;
};


typedef enum {
    BOOTSTRAP_REQ=0,
    BOOTSTRAP_CFM,
    BOOTSTRAP_ACK,
    ANONCERTREQ_REQ,
    ANONCERTREQ_CFM,
    ANONCERTREQ_STS_REQ,
    ANONCERTREQ_STS_CFM,
    ANONCERTRESP_ACK,
    DECRYPTIONKEY_REQ,
    DECRYPTIONKEY_CFM,
    DECRYPTIONKEY_ACK,
    MISBEHAVIOURREPORT_REQ,
    MISBEHAVIOURREPORT_ACK,
    CRL_REQ,
    CRL_CFM
}Stages;



typedef enum { 
    csr_cert_verification_failure=0, 
    csr_cert_expired,
    csr_cert_revoked, 
    csr_cert_unauthorized,
    request_denied, 
    csr_cert_unknown ,
    canonical_identity_unknown ,
    certificate_response_not_ready ,
    cert_set_start_time_in_past ,
    cert_set_start_time_too_far_in_future ,
    cert_set_end_time_too_close_to_start_time,
    cert_set_end_time_too_far_from_start_time,
    requested_smaller_than_minimum_batch_duration,
    requested_larger_than_maximum_batch_duration,
    requested_past_decryption_keys,
    requested_far_future_decryption_keys,
    invalid_signature,
    invalid_request_hash,
    invalid_response_encryption_key,
    invalid_status,
    invalid_algorithm,
    current_time_in_past,
    current_time_in_future,
} CertificateRequestErrorCode;

typedef enum {
    success=0, 
    first_request, 
    got_certs_not_ready,
    timeout, 
    err_decrypt
} CertificateProcessingStatus;


typedef enum {
    casual_report=0,
    alert_related_report,
    suspicious_message
} MisbehaviorReportCategory;

typedef struct {
uint32_t i;
uint8_t linkage_value_1[16];
uint8_t linkage_value_2[16];
uint32_t max_i;
} AnonymousEntry;


enum { 
    crl_verification_failure=0,
    crl_anonymous_cert_invalid,
    crl_anonymous_cert_revoked,
    crl_request_denied,
    crl_invalid_crl_series,
    crl_invalid_crl_serial,
    crl_invalid_ca_id
} CrlRequestErrorCode;

typedef struct {
uint8_t private_key_sign[32]; // private key for signing
uint8_t private_key_dec[32]; // private key for decryption
uint8_t encoded_certificate[MAX_CIPHERTEXT_LENGTH-16-32-32]; // encoded 1609.2 Certificate
} PrivateKeyAndCertificate;

typedef struct {
uint8_t nonce[12]; // 12-byte nonce used to AES-CCM encrypt this entry
uint8_t ciphertext_length; // number of bytes in the ciphertext
uint8_t ciphertext[MAX_CIPHERTEXT_LENGTH]; // ciphertext
//uint8_t padding[256-12-1-ciphertext_length]; // arbitrary padding bytes
} Entry;

EC_POINT * PrivateKeyReconstruct(UINT8 *pHash, UINT8 *pPrivKeyReconBuf, UINT8 *pEphemeralPrivKey, UINT8 *pPrivateKey, EC_GROUP *pGroup);
EC_POINT *PublicKeyReconstruct(UINT8 *pHash, UINT8 *pPubKeyRecon, UINT8 *pCAPubKeyRecon, UINT8 *pStaticPubKey, EC_GROUP *pGroup);
EC_GROUP * CreateECCompGroup(UINT32 GroupType);
UINT32 HexStr2ByteStr(UINT8 *str, UINT8 *buf, UINT32 buflen);
UINT32 ByteStr2HexStr(UINT8 *str, UINT8 *buf, UINT32 inBufLen);
UINT32 VerifySignedData(UINT8 *pPubKey, UINT8 *pSignData, UINT8 *pDigest, UINT32 uiDigestLen);
UINT32 AWSecEncrypt(UINT32 cipherAlgo, UINT8 * pCipherKey, UINT8 * pIVNonce, UINT32 uiNonceLen, UINT8 * pBuf, UINT32 uiBufLen, UINT8 *pCipherText, UINT32 *uiCipherTextLen, UINT32 uiTagLen);
UINT32 AWSecDecrypt(UINT32 cipherAlgo, UINT8 * pCipherKey, UINT8 * pIVNonce, UINT32 uiNonceLen, UINT8 *pCipherText, UINT32 uiCipherTextLen, UINT8 *pBuf, UINT32 *uiBufLen, UINT32 uiTagLen);
UINT8 * AWSecHash(UINT32 hashAlgo, UINT8 * pBuf, UINT32 uiBufLen, UINT8 *pDigest, UINT32 *uiDigestLen, UINT32 uiReqLen);
UINT32 GetSignedData(UINT8 *pPrivKey, UINT8 *pData, UINT32 uiDataLen, UINT8 *pDigest, UINT32 uiDigestLen, UINT8 *pSignDataBuf);
UINT8* itoa(UINT32 i,  UINT8 b[]);

static inline uint64_t swap_64( uint64_t x1)
{
    unsigned long long x = (x1);
    if(!BIGENDIAN)
        return (((x & 0xFFULL) << 56| (x & 0xFF00000000000000ULL) >> 56| \
        (x & 0xFF00ULL) << 40| (x & 0xFF000000000000ULL) >> 40| \
        (x & 0xFF0000ULL) <<24| (x & 0xFF0000000000ULL) >> 24| \
        (x & 0xFF000000ULL) <<8 | (x & 0xFF00000000ULL) >>8 ));
    else
        return x;

}

static inline uint32_t swap_32( uint32_t x) 
{
    if(!BIGENDIAN)
        return (((x & 0xff)<<24)|((x>>24) & 0xff)|((x & 0x0000ff00)<<8)|((x & 0x00ff0000) >>8)) ;
    else
        return x;
}



static inline uint16_t swap_16(uint16_t x)
{
    if(!BIGENDIAN)
        return ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8));
    else
        return x;
}
