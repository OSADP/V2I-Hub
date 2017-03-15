/*
 * mx.h
 *
 *  Created on: Jan 13, 2016
 *      Author: BAUMGARDNER
 */

#ifndef SRC_MESSAGE_EXCHANGE_MX_H_
#define SRC_MESSAGE_EXCHANGE_MX_H_

#ifdef __CYGWIN__
/* No Bluez libraries for Cygwin */
#define MX_NO_BT
#endif

#include "mx/MxService.h"
#include "mx/MxServiceFactory.h"

#ifndef MX_NO_TCP
#include "mx/MxTcpService.h"
#endif

#ifndef MX_NO_UDP
#endif

#ifndef MX_NO_BT
#include "mx/MxBluetoothService.h"
#endif

#ifndef MX_NO_FILE
#include "mx/MxFileGlobService.h"
#endif

#endif /* SRC_MESSAGE_EXCHANGE_MX_H_ */
