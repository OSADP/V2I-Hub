#ifndef SPAT_MESSAGE_R41_H
#define SPAT_MESSAGE_R41_H

#include <stdio.h>
#include <string>
#include <stdexcept>
#include <asn_j2735_r41/SPAT.h>
#include <asn_j2735_r41/UPERframe.h>

#include "utils/common.h"
#include "SPaTData.h"


class SpatMessage_r41 {
public:
	SpatMessage_r41();
	~SpatMessage_r41();

	unsigned char encoded[4000];
	unsigned int encodedBytes;
	unsigned char derEncoded[4000];
	unsigned int derEncodedBytes;

	bool		createUPERframe_DERencoded_msg();
	int			createSpat();
	uint16_t 	createIntersectionStatus(unsigned short intStatus);
	uint16_t 	createEventState(unsigned short evStatus);
	void		setSpatData(SPaTData* sd);

private:
	void		printUINT16(uint16_t num);
	long		getAdjustedTime(unsigned int offset);
	SPAT 		spatMessage;
	SPaTData 	*spatData;
	UPERframe 	*frame;

	int 		iterations;
};

#endif
