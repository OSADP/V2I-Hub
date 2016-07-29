/*
 * snmpClient.h
 *
 *  Created on: Aug 22, 2014
 *      Author: gibbsw
 */

#ifndef SIGNALCONTROLLER_H_
#define SIGNALCONTROLLER_H_

#include <pthread.h>
#include <boost/thread.hpp>

#include "utils/common.h"
#include "utils/spat.h"
#include "SignalControllerNTCIP.h"
#include "SpatMessage_r41.h"
#include "SPaTData.h"


class SignalController
{
	public:

		void Start(spat *message);
		void spat_load();
		void start_signalController();
		void spat_update(spat* msg);
		int getActionNumber();
		void setConfigs(char* ip, char* udpPort, char* snmpIP, char* snmpPort, const char* ptlmFile);
		void updatePtlmFile(const char* ptlmFile);
		int getIsConnected();
		SPaTData* getSpatData();

		pthread_mutex_t spat_message_mutex;
		boost::thread sigcon_thread_id;

	private:
		void *get_in_addr(struct sockaddr *);

		// Local IP address and UDP port for reception of SPAT data from the TSC.
		char* _localIp;
		char* _localUdpPort;

		char* _ptlmFile;

		spat* message;
		SPaTData sd;
		SignalControllerNTCIP sc;
		int counter;
		unsigned long normalstate;
		unsigned long crossstate;
		int EthernetIsConnected;


};

#endif /* SIGNALCONTROLLER_H_ */
