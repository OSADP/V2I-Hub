#ifndef AsmDEF_H_
#define AsmDEF_H_
#ifdef  __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

/***************************************************************************
 * 1. INCLUDES                                                             *
 ***************************************************************************/

/***************************************************************************
 * 2. DEFINES                                                              *
 ***************************************************************************/
/* Basic definition */
#ifndef TRUE
#define TRUE                            1
#endif
#ifndef FALSE
#define FALSE                           0
#endif

/* Length of a digest as defined in 1609.2 */
#define DIGESTLEN                       8
#define CRL_DIGESTLEN                   10

/* Elliptic curve defines */
#define POINT_SIZE_ECDSA224             28
#define POINT_SIZE_ECDSA256             32
#define CURVE_ORDER_OCTETS_ECDSA224     28
#define CURVE_ORDER_OCTETS_ECDSA256     32

#define ENABLE_POINT_COMPRESION			1
#define UNCOMPRESSED_POINT				0
#define COMPRESSED_POINT				1

#define CAMP_1609DOT2_EXTENSIONS        1
//LCM
//#define USE_LCM 1

#ifdef CAMP_1609DOT2_EXTENSIONS
	/* Various CAMP specific lengths */
	#define CAMP_CERT_BATCH_ID_SIZE              8
	#define CAMP_ID_SIZE                         8
	#define CAMP_KEY_ID_SIZE                     8
	#define CAMP_HASH_SIZE                       10
	#define CAMP_KEY_SIZE                        16
	#define CAMP_SIZE_OF_S                       32
	#define CAMP_LINKAGE_ID_SIZE                 16
#endif

/* Point compression Defines*/
#ifdef ENABLE_POINT_COMPRESION
    #define SIZE_OF_ECC224_PUP_KEY 28
    #define SIZE_OF_ECC256_PUP_KEY 32
    #define PUP_KEY_Y &sign
    #define PUP_KEY_Y_SIZE_224 0
    #define PUP_KEY_Y_SIZE_256 0
#else
    #define SIZE_OF_ECC224_PUP_KEY sizeof(EscEcc224_PublicKeyT)
    #define SIZE_OF_ECC256_PUP_KEY sizeof(EscEcc256_PublicKeyT)
    #define PUP_KEY_Y public_key.y
    #define PUP_KEY_Y_SIZE_224 POINT_SIZE_ECDSA224
    #define PUP_KEY_Y_SIZE_256 POINT_SIZE_ECDSA256
#endif



/* Protocol Version */
#define PROTOCOL_VERSION                2
#define CERT_VERSION_AND_TYPE_EXPLICIT  2
#define CERT_VERSION_AND_TYPE_IMPLICIT  3

/* Content Type */
#define CONTENT_TYPE_UNSECURED          0x00
#define CONTENT_TYPE_SIGNED             0x01
#define CONTENT_TYPE_ENCRYPTED          0x02
#define CONTENT_TYPE_CERT_REQUEST       0x03
#define CONTENT_TYPE_CERT_RESPONSE      0x04
#define CONTENT_TYPE_ANON_CERT_RESPONSE 0x05
#define CONTENT_TYPE_CERT_REQUEST_ERROR 0x06
#define CONTENT_TYPE_CRL_REQUEST        0x07
#define CONTENT_TYPE_CRL                0x08
#define CONTENT_TYPE_SIGNED_WSA         0x0b
#define CONTENT_TYPE_CERT_RESPONSE_ACK  0x0c
#ifdef CAMP_1609DOT2_EXTENSIONS
	#define CONTENT_TYPE_CRL_REQ                            236
	#define CONTENT_TYPE_CRL_REQ_ERROR                      237
	#define CONTENT_TYPE_MISBEHAVIOR_REPORT_REQ             238
	#define CONTENT_TYPE_MISBEHAVIOR_REPORT_ACK             239
	#define CONTENT_TYPE_CERT_BOOTSTRAP_REQ                 240
	#define CONTENT_TYPE_CERT_BOOTSTRAP_CFM                 241
	#define CONTENT_TYPE_CERT_BOOTSTRAP_ACK                 242
	#define CONTENT_TYPE_ANONYMOUS_CERT_REQUEST_REQ         243
	#define CONTENT_TYPE_ANONYMOUS_CERT_REQUEST_CFM         244
	#define CONTENT_TYPE_ANONYMOUS_CERT_REQUEST_STATUS_REQ  245
	#define CONTENT_TYPE_ANONYMOUS_CERT_REQUEST_STATUS_CFM  246
	#define CONTENT_TYPE_SIG_ENC_CERT                       247
	#define CONTENT_TYPE_CERTIFICATE_AND_PRIVATE_KEY_RECONSTRUCTION_VALUE 248
	#define CONTENT_TYPE_ANONYMOUS_CERT_RESPONSE_ACK        249
	#define CONTENT_TYPE_ANONYMOUS_CERT_DECRYPTION_KEY_REQ  250
	#define CONTENT_TYPE_ANONYMOUS_CERT_DECRYPTION_KEY_CFM  251
	#define CONTENT_TYPE_ANONYMOUS_CERT_DECRYPTION_KEY_ERR  252
	#define CONTENT_TYPE_ANONYMOUS_CERT_DECRYPTION_KEY_ACK  253
	#define CONTENT_TYPE_SYMMETRIC_ENCRYPTED                254
#endif

/* Signer Identifier Type */
#define SIGNER_SELF                     0
#define SIGNER_TYPE_CERT_DIGEST_224     1
#define SIGNER_TYPE_CERT_DIGEST_256     2
#define SIGNER_TYPE_CERT                3
#define SIGNER_TYPE_CERT_CHAIN          4

/* Signer type interface*/
#define SIGNER_INTERFACE_TYPE_CERT_DIGEST   0
#define SIGNER_INTERFACE_TYPE_CERT          1 
#define SIGNER_INTERFACE_TYPE_CERT_CHAIN    2

/* Message Format Type */
#define FLAG_USE_GENERATION_TIME        1
#define FLAG_EXPIRES         			2
#define FLAG_USE_LOCATION               3

/* Certifcate Flags  Type */
#define FLAG_USE_START_VALIDITY         0
#define FLAG_LIFETIME_IS_DURATION       1
#define FLAG_ENCRYPTION_KEY             2

/* PK Algorithm type */
#define ECDSA_NISTP224_WITH_SHA224      0
#define ECDSA_NISTP256_WITH_SHA256      1
#define ECDSA_NISTP256                  2

/*Status byte defines*/
#define X_COORDINATE_ONLY   0
#define SIZE_OF_STATUS      1
#define COMPRESSED_LSB_Y_0  2
#define COMPRESSED_LSB_Y_1  3
#define UNCOMPRESSED_Y      4

/*Extraction Defines*/
#define EXTRACTION_NOT_REQUIRED 1
#define EXTRACTION_REQUIRED 2
#define EXTRACTION_DONE 3

/* Symm Algorithm type */
#define SYMM_AES_128_CCM                0

/* AES-CCM ciphertext nonce length */
#define AESCCM_CIPHERTEXT_NONCE_LEN     12

/* AES-CCM ephemeral key length */
#define AESCCM_EPHEMERAL_KEY_LEN        64 //No longer used

/* AES-CCM symmetric key length */
#define AESCCM_SYMM_KEY_LEN             16

/* AES-CCM authentication tag length */
#define AESCCM_AUTH_TAG_LEN             20 //LW: tBits

/* Latitude & Longitude not available value */
#define LATITUDE_MAX_VALUE              900000000
#define LATITUDE_MIN_VALUE             -900000000
#define LATITUDE_NOT_AVAILABLE          900000001
#define LONGITUDE_MAX_VALUE             1800000000
#define LONGITUDE_MIN_VALUE            -1800000000
#define LONGITUDE_NOT_AVAILABLE         1800000001

/* CRL version */
#define CRL_VERSION                     1

/* CRL type */
#define CRL_TYPE_ID_ONLY                0
#define CRL_TYPE_ID_AND_EXPIRY          1
#ifdef CAMP_1609DOT2_EXTENSIONS
	#define CRL_TYPE_ANONYMOUS_ENTRY        2
#endif

/* Subject type */
#define SUBJECT_TYPE_MSG_ANONYMOUS                    0
#define SUBJECT_TYPE_MSG_IDENTIFIED_NOT_LOCALIZED     1
#define SUBJECT_TYPE_MSG_IDENTIFIED_LOCALIZED         2
#define SUBJECT_TYPE_MSG_CSR                          3
#define SUBJECT_TYPE_WSA                              4
#define SUBJECT_TYPE_WSA_CSR                          5
#define SUBJECT_TYPE_MSG_CA                           6
#define SUBJECT_TYPE_WSA_CA                           7
#define SUBJECT_TYPE_CRL_SIGNER                       8
#define SUBJECT_TYPE_MSG_RA                           9
#define SUBJECT_TYPE_ROOT_CA                        255

/* Region type */
#define REGION_TYPE_FROM_ISSUER         0
#define REGION_TYPE_CIRCLE              1
#define REGION_TYPE_RECTANGLE           2
#define REGION_TYPE_POLYGON             3
#define REGION_TYPE_NONE                4

/* Array type */
#define ARRAY_TYPE_FROM_ISSUER          0
#define ARRAY_TYPE_SPECIFIED            1

/* Misbehavior Report Category */
#ifdef CAMP_1609DOT2_EXTENSIONS
	#define MISBEHAVIOR_REPORT_CATEGORY_CASUAL_REPORT           0
	#define MISBEHAVIOR_REPORT_CATEGORY_ALERT_RELATED_REPORT    1
	#define MISBEHAVIOR_REPORT_CATEGORY_SUSPICOUS_MESSAGE       2
#endif

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
//#ifdef USE_LCM
#define CMD_LCM_STATUS_RDY                                  0x0D //cycurV2X to OBE
#define CMD_LCM_STATUS_CERT_CHANGED                         0x1D //cycurV2X to OBE
//#endif /* USE_LCM */

/* Error Result */
#define CMD_ERR_INVALID_INPUT                               0x30
#define CMD_ERR_NO_KEY_FOUND                                0x31
#define CMD_ERR_CERT_EXPIRED                                0x32
#define CMD_ERR_CERT_NOT_YETVALID                           0x5A //<-5a current largest erro
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
#define CMD_ERR_GENERAL                                     0xFF
#define CMD_CERTIFICATE_FOUND_NOT_VERIFIED                  0x60

#define CERTIFICATE                                         0x01
#define CERTID8                                             0x02
#define CERTID10                                            0x03

#ifdef CAMP_1609DOT2_EXTENSIONS
#define MISBEHAVIOR_TYPE_CASUAL 0x00
#define MISBEHAVIOR_TYPE_ALERT  0x01
#define MISBEHAVIOR_TYPE_SUSPICIOUS 0x02
#endif

#define NUMBER_OF_LEAP_MICROSECONDS_TODAY 34000000
#define NUMBER_OF_LEAP_SECONDS_TODAY 34

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

/* 1609 data types */
typedef UINT32              Time32;
typedef UINT64              Time64;

#pragma pack(1)


//needed for cert container decryption
static const UINT8 key2[AESCCM_SYMM_KEY_LEN] =
	{0x86, 0x54, 0xee, 0xfd, 0xd4, 0x8a, 0xec, 0x6e,
	 0xa1, 0x41, 0xb8, 0xa1, 0x66, 0xec, 0x75, 0xb9};

typedef struct
{
    Time64  time;
    UINT8 confidence;
} Time64WithConfidence;

typedef struct
{
    SINT32  latitude;
    SINT32  longitude;
} TwoDLocation;

typedef struct
{
    TwoDLocation  center;
    UINT16  radius;
} CircularRegion;

typedef struct
{
    TwoDLocation  upper_left;
    TwoDLocation  lower_right;
} RectangularRegion;

typedef struct
{
    SINT32  latitude;
    SINT32  longitude;
    UINT8   elevation[2];
} ThreeDLocation;

typedef struct
{
    UINT64  psid;
    UINT8   maxPriority;
} PsidPriority;

typedef UINT8   CertId10[10];

typedef struct
{
    CertId10    id;
    Time32      expiry;
} IdAndDate;

typedef struct
{
	UINT32	i;
	UINT8 linkage_value_1[CAMP_LINKAGE_ID_SIZE];
	UINT8 linkage_value_2[CAMP_LINKAGE_ID_SIZE];
	UINT32 max_i;
} AnonymousEntry;



//**************************************************************************//
// AsmMsg_Sign message structure
//   Request:
//     - AsmMsg_Sign_Msg_Req1_t
//     - AsmMsg_Sign_Msg_Req2_t
//     - AsmMsg_Sign_Msg_Req3_t
//   Response:
//     - AsmMsg_Sign_Msg_Res_t
//
//**************************************************************************//
/***** AsmMsg_Sign request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT8   signed_message_type;
    UINT32  application_data_length;
    UINT8   application_data[1];    // start point
} AsmMsg_Sign_Msg_Req1_t;

typedef struct
{
    UINT8   psid[1]; // start point
} AsmMsg_Sign_Msg_Req2_t;

typedef struct
{

    BOOL    use_generation_time;
    BOOL    use_expiry_time;
    BOOL    use_generation_location;
	BOOL    sign_with_fast_verification;
    Time64  expiry_time;
    SINT32  generation_location_latitude;
    SINT32  generation_location_longitude;
    UINT8   generation_location_elevation[2];
    UINT8   signer_identifier_type;
    SINT32  signer_identifier_cert_chain_length;
} AsmMsg_Sign_Msg_Req3_t;


/***** AsmMsg_Sign response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT32  signed_message_length;
    UINT8   signed_message_data[1];     // start point
} AsmMsg_Sign_Msg_Res_t;

//**************************************************************************//
// AsmMsg_Verify message structure
//   Request:
//     - AsmMsg_Verify_Msg_Req1_t
//     - AsmMsg_Verify_Msg_Req2_t
//   Response:
//     - AsmMsg_Verify_Msg_Res_t
//
//**************************************************************************//
/***** AsmMsg_Verify request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  signed_message_length;
    UINT8   signed_message_data[1];     // start point
} AsmMsg_Verify_Msg_Req1_t;

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
    //UINT32  generation_location_horizontal_confidence_multiplier;   // x1000
    //UINT32  generation_location_elevation_confidence_multiplier;   // x1000
    SINT32  local_location_latitude;
    SINT32  local_location_longitude;
    UINT32  overdue_CRL_tolerance;
} AsmMsg_Verify_Msg_Req2_t;

/***** AsmMsg_Verify response structure *****/

typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT32  unsigned_data_length;
    UINT8   unsigned_data[1];    // start point
} AsmMsg_Verify_Msg_Res1_t;

typedef struct
{
    Time64WithConfidence generation_time;
    Time64  expiry_time;
    ThreeDLocation generation_location;
    UINT32  last_CRL ;
    UINT32  most_overdue_CRL;
}  AamMsg_Verify_Msg_Res2_t;

//**************************************************************************//
// AsmMsg_Enc structures
//   Encrypt a Message
//     Request:
//       - AsmMsg_Enc_Req1_t
//       - AsmMsg_Enc_Req2_t
//     Response:
//       - AsmMsg_Enc_Msg_Res1_t
//       - AsmMsg_Enc_Msg_Res2_t
//
//**************************************************************************//
/***** Encrypt a Message Request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  application_data_length;
    UINT8   application_data_type;
    UINT8   application_data[1];    // start point
} AsmMsg_Enc_Req1_t;

typedef struct
{
    UINT16  recipient_number;
    UINT8   recipient_certs[1];   // start point
} AsmMsg_Enc_Req2_t;

/***** Encrypt a Message Response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT32  encrypted_message_length;
    UINT8   encrypted_message_data[1];  // start point
} AsmMsg_Enc_Msg_Res1_t;

typedef struct
{
    UINT16  failed_recipient_number;
    UINT8   failed_recipient_certs[1];   // start point
} AsmMsg_Enc_Msg_Res2_t;

//**************************************************************************//
// AsmMsg_Dec structures
//   Decrypting a Message
//     Request:
//       - AsmMsg_Dec_Msg_Req1_t
//       - AsmMsg_Dec_Msg_Req2_t
//     Response:
//       - AsmMsg_Dec_Msg_Res_t
//
//**************************************************************************//
/***** Decrypting a Message Request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  encrypted_message_length;
    UINT8   encrypted_message_data[1];    // start point
} AsmMsg_Dec_Msg_Req1_t;

typedef struct
{
    BOOL    allow_no_longer_valid_cert;
} AsmMsg_Dec_Msg_Req2_t;

/***** Decrypting a Message Response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT8   content_type;
    UINT32  application_data_length;
    UINT8   application_data[1];    // start point
} AsmMsg_Dec_Msg_Res_t;

//**************************************************************************//
// AsmMsg_Sign_WSA message structure
//   Request:
//     - AsmMsg_Sign_WSA_Req1_t
//     - AsmMsg_Sign_WSA_Req2_t
//   Response:
//     - AsmMsg_Sign_WSA_Res_t
//
//**************************************************************************//
/***** AsmMsg_Sign_WSA request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  application_data_length;
    UINT8   application_data[1];    // start point
} AsmMsg_Sign_WSA_Req1_t;

typedef struct
{
	BOOL sign_with_fast_verification;
    UINT64  lifetime;
    SINT32  generation_location_latitude;
    SINT32  generation_location_longitude;
    UINT8   generation_location_elevation[2];
    UINT16  number_of_permissions;
    UINT8   permissions[1];     // start point
}  AsmMsg_Sign_WSA_Req2_t;

/***** AsmMsg_Sign_WSA response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT32  signed_wsa_with_chain_length;
    UINT8   signed_wsa_with_chain_data[1];     // start point
} AsmMsg_Sign_WSA_Res_t;

//**************************************************************************//
// AsmMsg_Verify_WSA message structure
//   Request:
//     - AsmMsg_Verify_WSA_Req1_t
//     - AsmMsg_Verify_WSA_Req2_t
//   Response:
//     - AsmMsg_Verify_WSA_Res1_t
//     - AsmMsg_Verify_WSA_Res2_t
//
//**************************************************************************//
/***** AsmMsg_Verify_WSA request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  signed_wsa_length;
    UINT8   signed_wsa_data[1];     // start point
} AsmMsg_Verify_WSA_Req1_t;

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
} AsmMsg_Verify_WSA_Req2_t;

/***** AsmMsg_Verify_WSA response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT32  unsigned_wsa_length;
    UINT8   unsigned_wsa_data[1];    // start point
} AsmMsg_Verify_WSA_Res1_t;

typedef struct
{
    Time64WithConfidence generation_time;
    Time64  expiry_time;
    ThreeDLocation generation_location;
    SINT32  most_overdue_CRL;
}  AsmMsg_Verify_WSA_Res2_t;

//**************************************************************************//
// AsmMsg_CRL structures
//   Request:
//     - AsmMsg_CRL_Req_t
//   Response:
//     - AsmMsg_CRL_Res_t
//**************************************************************************//
/***** CRL Request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  crl_length;
    UINT8   crl_data[1];     // start point
} AsmMsg_CRL_Req_t;


/***** CRL Response structure *****/
typedef struct
{
    UINT8  command;
    UINT32 handle;
} AsmMsg_CRL_Res_t;

/***** Restart Request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
} AsmMsg_Restart_Req_t;

/***** Restart Response structure *****/
typedef struct
{
    UINT8  command;
    UINT32 handle;
} AsmMsg_Restart_Res_t;


//**************************************************************************//
// Data Extraction Request structure
//   Request:
//     - AsmMsg_DataExtraction_Req1_t
//     - AsmMsg_DataExtraction_Req2_t
//   Response:
//     - AsmMsg_DataExtraction_Res1_t
//     - AsmMsg_DataExtraction_Res2_t
//     - AsmMsg_DataExtraction_Res3_t
//
//**************************************************************************//
/***** Data Extraction Request structures *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT32  message_length;
    UINT8   message_data[1];    // start point
} AsmMsg_DataExtraction_Req1_t;

typedef struct
{
    BOOL    allow_no_longer_valid_cert;
} AsmMsg_DataExtraction_Req2_t;

/***** Data Extraction Response structures *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
	UINT8   message_type;
	UINT8   inner_message_type;
	UINT32	message_data_length;
	UINT8   message_data[1];

} AsmMsg_DataExtraction_Res1_t;

typedef struct
{
	UINT32	signed_message_data_length;
	UINT8   signed_message_data[1];
}AsmMsg_DataExtraction_Res2_t;

typedef struct
{
	UINT64  PSID;
	BOOL    use_generation_time;
	Time64WithConfidence generation_time;
	BOOL    use_expiry_time;
	Time64  expiry_time;
	BOOL    use_generation_location;
	ThreeDLocation generation_location;
	UINT8   signer_info_type;
	UINT32  signer_info_data_length;
	UINT8   signer_info_data[1];
} AsmMsg_DataExtraction_Res3_t;

//**************************************************************************//
// AsmMsg_CertChg structures
//   Request:
//     - AsmMsg_CertChg_Req_t
//   Response:
//     - AsmMsg_CertChg_Res_t
//**************************************************************************//
/***** Certificate change request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
} AsmMsg_CertChg_Req_t;

/***** Certificate change response structure *****/
typedef struct
{
    UINT8  command;
    UINT32 handle;
} AsmMsg_CertChg_Res_t;

//**************************************************************************//
// AsmMsg_CertInfo structures
//   Request:
//     - AsmMsg_Cert_Info_Req_t
//   Response:
//     - AsmMsg_Cert_Info_Res_t
//**************************************************************************//
/***** AsmMsg_Cert_Info_Req_t request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT8   identifier_type;
    UINT32  identifier_length;
    UINT8   identifier[1];    // start point
} AsmMsg_Cert_Info_Req1_t;

typedef struct
{
    UINT8   command;
    UINT32  handle;
    UINT8   digest[DIGESTLEN];
    Time32  lastCRLTime;
    Time32  nextCRLTime;
    UINT32   public_key_length;    // start point
    UINT8   public_key[1];
} AsmMsg_Cert_Info_Res1_t;

typedef struct
{
    UINT32  certificate_length;
    UINT8  certificate[1];    // start point
} AsmMsg_Cert_Info_Res2_t;

typedef struct
{
    UINT32  scope_length;
    UINT8   scope[1];    // start point
} AsmMsg_Cert_Info_Res3_t;

typedef struct
{
    UINT32  geograhic_data_length;
    UINT8   geograhic_data[1];    // start point
} AsmMsg_Cert_Info_Res4_t;

//**************************************************************************//
// AsmMsg_Misbehavior message structure
//   Request:
//     - AsmMsg_Misbehavior_Rep_Msg_Req1_t
//   Response:
//     - AsmMsg_Misbehavior_Rep_Msg_Res_t
//
//**************************************************************************//
/***** AsmMsg_Sign request structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
    Time64  actual_time;
    UINT8   type;
    SINT32  generation_location_latitude;
    SINT32  generation_location_longitude;
    UINT8   generation_location_elevation[2];
    UINT32  misbehavior_report_length;
    UINT8   misbehavior_report_data[1];    // start point
} AsmMsg_Misbehavior_Rep_Msg_Req1_t;

//**************************************************************************//
// Error response
//**************************************************************************//
typedef struct
{
    UINT8   result;
    UINT32  handle;
} AsmMsg_Misbehavior_Rep_Msg_Res_t;

//**************************************************************************//
// Error response
//**************************************************************************//
typedef struct
{
    UINT8   result;
    UINT32  handle;
} AsmMsg_Error_Res_t;

//**************************************************************************//
// Failed verification response
//**************************************************************************//
typedef struct
{
    UINT8   result;
    UINT32  handle;
    UINT32  unsigned_data_length;
    UINT8   unsigned_data[1];    // start point
} AsmMsg_Error_VerifyFail_Res_t;

//#ifdef USE_LCM
//**************************************************************************//
// AsmMsg_LCM_Status structures
//   Response 1:
//     - AsmMsg_LCM_Status_Rdy_t
//   Response 2:
//     - AsmMsg_LCM_Status_Changed_t
//**************************************************************************//

/***** LCM Status Certificate change response structure *****/
typedef struct
{
    UINT8   command;
    UINT32  handle;
} AsmMsg_LCM_Status_Rdy_t;

/***** LCM Status Certificate change response structure *****/
typedef struct
{
    UINT8  command;
    UINT32 handle;
    UINT8  digest[DIGESTLEN];
    Time32 expiration;
    Time32 duration;
    UINT32 certificate_length;
    UINT8  certificate[1]; //start point
} AsmMsg_LCM_Status_Changed_t;

//#endif /* USE_LCM */

//needed for cert container decryption
static const UINT8 key5[AESCCM_SYMM_KEY_LEN]  =
	{0x33, 0x6c, 0x1d, 0x43, 0x6c, 0xfa, 0xbf, 0x8c,
     0xe9, 0x44, 0x7c, 0x1b, 0x12, 0x31, 0xb5, 0x20};

#define ECDSA_256 0
#define ECDSA_224 1

typedef struct
{
    UINT8 psid[1];
    UINT8 length_ssp;
}permissions;

typedef struct
{
    UINT8 length_subject_name;
    UINT8 subj_name[1];
}subject_name;

typedef struct
{
    subject_name sname;
    UINT8 type;
    UINT8 length_permission_field;
    permissions perm;    
}scope;

typedef struct
{
    UINT8 algorithm;
    UINT8 public_key[1];
}public_keys;

typedef struct {
    UINT8 rsignature[32];
    UINT8 ssignature[32];
} signature;

typedef struct
{
    UINT8  CertVersion;
    UINT8  SubType;
    UINT8  SignerId[8];
    scope  scope_ntlocalized;
    UINT32 Expiration;
    UINT32 crl_series;
    UINT8  length_public_key;
    public_keys key;
    signature signat;
}Asm_certificate;
    
typedef struct 
{
    UINT8 ctlength[2];
    UINT8 ct[1];
}cert_chain;

typedef struct
{
    UINT8 ProVersion;
    UINT8 ContType;
    UINT8 CertType;
    union {
	UINT8 cert[1];
	UINT8 digest[1];
	cert_chain ctchain;
    }cert_type;
}AsmMsg_Extraction_Signed_Msg1;

typedef struct 
{
    UINT8 mflength;
    UINT8 mf;
    UINT8 psid[1];
}AsmMsg_Extraction_Signed_Msg2;

typedef struct
{
    UINT8 app_length[4];
    UINT8  app_data[1];
}AsmMsg_Extraction_Signed_Msg3;

typedef struct {
    Time64  time;
    UINT8 confidence;
} genaration_time;

typedef struct {
    SINT32  genaration_location_latitude;
    SINT32  genaration_location_longitude;
    UINT8   genaration_location_elevation[2];
    UINT8   genaration_location_confidences;
} genaration_location;

typedef struct
{
    genaration_time gen_time; 
    Time64  expiry_time;
    genaration_location gen_loc;
    signature sign;
}AsmMsg_Extraction_Signed_Msg4;

typedef struct {
    UINT8 mflength;
    UINT8 mf;
    UINT32 unsigned_datalength;
    UINT8 unsigned_data[1];
}AsmMsg_Unsigned_Wsa;
    
 
#pragma pack(0)

#ifdef  __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#endif /* AsmSMDEF_H_ */
