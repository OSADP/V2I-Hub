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

#include <tmx/j2735_messages/SpatMessage.hpp>
#include "SignalControllerNTCIP.h"


class SignalController
{
	public:
		~SignalController();

		void Start(string signalGroupMappingJson);
		void spat_load();
		void start_signalController();
		int getActionNumber();
		void setConfigs(string ip, string udpPort, string snmpIP, string snmpPort, string ptlmFile, string intersectionName, int intersectionId);
		void updatePtlmFile(const char* ptlmFile);
		int getIsConnected();

		//int getDerEncodedSpat(unsigned char* derEncodedBuffer);

		void getEncodedSpat(tmx::messages::SpatEncodedMessage* spatEncodedMsg, std::string currentPedLanes = "");

		pthread_mutex_t spat_message_mutex;
		boost::thread sigcon_thread_id;

	private:
		void *get_in_addr(struct sockaddr *);

		// Local IP address and UDP port for reception of SPAT dSPaTDataata from the TSC.
		char* _localIp;
		char* _localUdpPort;
		char* _intersectionName;
		int _intersectionId;

		string _signalGroupMappingJson;
		tmx::messages::SpatMessage *_spatMessage{NULL};

		SignalControllerNTCIP sc;
		int counter;
		unsigned long normalstate;
		unsigned long crossstate;
		int EthernetIsConnected;
		int IsReceiving;
};

#endif /* SIGNALCONTROLLER_H_ */
