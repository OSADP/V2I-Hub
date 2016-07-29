/*
 * ApplicationMessageTypes.h
 *
 *  Created on: Jun 7, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_APPLICATIONDATAMESSAGETYPES_H_
#define INCLUDE_APPLICATIONDATAMESSAGETYPES_H_

namespace appdatamessage {

//Indicates the type of data included in the Application Data Message payload json.
enum DataCodeId
{
	NOEVENTID = 0,
	Epcw=1,
};

enum StageTypes {
	Stage_None = 0, Detected = 1, Entering = 2, InCenter = 3, Exiting = 4

};
}

#endif /* INCLUDE_APPLICATIONDATAMESSAGETYPES_H_ */
