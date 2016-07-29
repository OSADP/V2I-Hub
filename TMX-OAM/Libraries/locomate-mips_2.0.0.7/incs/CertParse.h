#ifndef _CERTPARSE_H_
#define _CERTPARSE_H_

#include <stdint.h>

#define MAX_CIPHERTEXT_LENGTH 243
#define MAX_NUM_ENTRIES 288
#define DAY_SEC 86400 //number of seconds in a day(24hrs)
#define YEAR 1325376000
#define DIGESTLEN 8
#define REFERENCE_TIME 1072915234
#define NUMSECS_PERHOUR 3600

//typedef uint32_t Time32;

typedef uint16_t  CertificateDuration;

//typedef enum{self=0,certificate_digest_with_ecdsap224=1,certificate_digest_with_ecdsap256=2,certificate=3,certificate_chain=4,certificate_digest_with_other_algorithm=5,}SignerIdentifierType;

typedef enum { message_anonymous=0, message_identified_not_localized =1,message_identified_localized =2,message_csr = 3,wsa =4,wsa_csr =5,message_ca=6, wsa_ca =7, crl_signer=8,message_ra =9,root_ca=255} SubjectType;

typedef enum{ use_start_validity =0,lifetime_is_duration=1,encryption_key=2}CertificateContentFlags;

typedef enum{message_anonymous_flags=0,message_identified_not_localized_flags=1,message_identified_localized_flags=2,message_csr_flags=3,wsa_flags =4,wsa_csr_flags=5,message_ca_flags=6,wsa_ca_flags=7,crl_signer_flags =8,message_ra_flags =9}SubjectTypeFlags;

typedef enum {from_issuer=0, specified=1} ArrayType;

typedef enum { ecdsa_nistp224_with_sha224 =0,  ecdsa_nistp256_with_sha_256 =1, ecies_nistp256 =2} PKAlgorithm;

typedef enum { x_coordinate_only =0,compressed_lsb_y_0 =1,compressed_lsb_y_1 =2, uncompressed =3} EccPublicKeyType;

typedef enum {unsecured=0, Signed=1, encrypted=2, certificate_request=3, certificate_response=4, anonymous_certificate_response=5, certificate_request_error=6, crl_request=7, crl=8, signed_partial_payload=9, signed_external_payload=10, signed_wsa=11, certificate_response_acknowledgment=12, crl_req =236, crl_req_error =237, misbehavior_report_req =238, misbehavior_report_ack =239, cert_bootstrap_req =240, cert_bootstrap_cfm =241, cert_bootstrap_ack =242, anonymous_cert_request_req =243, anonymous_cert_request_cfm =244, anonymous_cert_request_status_req =245, anonymous_cert_request_status_cfm =246, sig_enc_cert =247, certificate_and_private_key_reconstruction_value =248, anonymous_cert_response_ack =249, anonymous_cert_decryption_key_req =250, anonymous_cert_decryption_key_cfm =251, anonymous_cert_decryption_key_error =252, anonymous_cert_decryption_key_ack =253,symmetric_encrypted=254}ContentType;

typedef enum {from_issuer_region =0, circle =1, rectangle =2, polygon =3, none =4} RegionType;

typedef struct {
    uint8_t certInfo[256];
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
    uint16_t expiration;
    uint16_t start_validity_or_lft;
    uint16_t crl_series;
    uint16_t verification_key;
    uint16_t encryption_key;
    uint16_t sig_RV;
} Certificate;
#endif
