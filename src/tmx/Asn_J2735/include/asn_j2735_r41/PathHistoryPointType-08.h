/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../J2735_R41_Source_mod.ASN"
 * 	`asn1c -gen-PER -fcompound-names -fincludes-quoted`
 */

#ifndef	_PathHistoryPointType_08_H_
#define	_PathHistoryPointType_08_H_


#include "asn_application.h"

/* Including external dependencies */
#include "OCTET_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PathHistoryPointType-08 */
typedef OCTET_STRING_t	 PathHistoryPointType_08_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PathHistoryPointType_08;
asn_struct_free_f PathHistoryPointType_08_free;
asn_struct_print_f PathHistoryPointType_08_print;
asn_constr_check_f PathHistoryPointType_08_constraint;
ber_type_decoder_f PathHistoryPointType_08_decode_ber;
der_type_encoder_f PathHistoryPointType_08_encode_der;
xer_type_decoder_f PathHistoryPointType_08_decode_xer;
xer_type_encoder_f PathHistoryPointType_08_encode_xer;
per_type_decoder_f PathHistoryPointType_08_decode_uper;
per_type_encoder_f PathHistoryPointType_08_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _PathHistoryPointType_08_H_ */
#include "asn_internal.h"
