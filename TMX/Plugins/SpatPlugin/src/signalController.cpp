/*
 ============================================================================
 Name        : snmpClient.cpp
 Author      : William Gibbs
 Version     :
 Copyright   : Battelle
 Description : Query Signal Controller and populate the SPaT message
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include <netinet/in.h>
#ifndef __CYGWIN__
#include <sys/prctl.h>
#endif
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <PluginLog.h>

#include "signalController.h"

#include "NTCIP1202.h"

using namespace tmx::messages;
using namespace tmx::utils;
using namespace std;

void SignalController::Start()
{
	// Create mutex for the Spat message
	pthread_mutex_init(&spat_message_mutex, NULL);
    // launch update thread
    sigcon_thread_id = boost::thread(&SignalController::start_signalController, this);
    // test code
    counter = 0;
    normalstate = 0x01;
    crossstate = 0x04;
}

// get sockaddr, IPv4 or IPv6:
void *SignalController::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void SignalController::setConfigs(char* localIp, char* localUdpPort, char* tscIp, char* tscRemoteSnmpPort, const char* ptlmFile, char * intersectionName, int intersectionId)
{
	_localIp = strdup(localIp);
	_localUdpPort = strdup(localUdpPort);
	_intersectionId = intersectionId;
	_intersectionName = strdup(intersectionName);

	sc.setConfigs(tscIp, tscRemoteSnmpPort);
}

void SignalController::start_signalController()
{
#ifndef __CYGWIN__
	prctl(PR_SET_NAME, "SpatGenSC", 0, 0, 0);
#endif

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	int maxDataSize = 1000;

	Ntcip1202* ntcip1202 = new Ntcip1202();
	_spat = new SPAT();

    int sockfd, numbytes;
    char buf[maxDataSize];
    struct addrinfo hints, *servinfo;
    int rv;
    struct timeval tv;
    int on = 1;

	while (1)
	{

		counter++;
		if(counter % 60)
		{

		}
		// Create UDP Socket
	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_protocol = IPPROTO_UDP;
	    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;

	    EthernetIsConnected = 0;
	    IsReceiving = 0;

	    while (1) {
	    	printf("Top of While Loop\n");
			if ((rv = getaddrinfo(_localIp, _localUdpPort, &hints, &servinfo)) != 0) {
	    		printf("Getaddrinfo Failed %s %s Exiting thread!!!\n", _localIp, _localUdpPort);
	    		return;
			}
			printf("Getting Socket\n");
			if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,	servinfo->ai_protocol)) == -1) {
				printf("Get Socket Failed %s %s Exiting thread!!!\n", _localIp, _localUdpPort);
				return;
			}

			rv = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

			// Set the socket to time out on reads if no data comes in during the timeout value then the socket will close and
			// then try to re-open during the normal execution
			// Wait up to 10 seconds.
		    tv.tv_sec = 10;
		    tv.tv_usec = 0;
		    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

			if (bind(sockfd,servinfo->ai_addr,servinfo->ai_addrlen)==-1) {
				printf("Could not bind to Socket %s:%s Exiting thread!!!\n", _localIp, _localUdpPort);
				return;
			}

			if (servinfo == NULL) {
				printf("Could not connect\n");
				EthernetIsConnected = 0;
			}
			else {
				printf("Connected\n");
				EthernetIsConnected = 1;
			}

			if (EthernetIsConnected) {
				//printf("Signal Controller UDP Client Connected to %s:%s\n",_localIp, _localUdpPort);
				freeaddrinfo(servinfo); // all done with this structure
//				client_socket = sockfd;
				// Receive Packets and process until disconnected
				while(EthernetIsConnected) {
					//printf("Signal Controller ethernet connected, reading data\n");
					numbytes = recv(sockfd, buf, maxDataSize-1, 0);
					//printf("Signal Controller read %d bytes\n", numbytes);
					//TODO - Check the start byte for 0xcd, then check for len of 245.
					//TODO - store in temp space if less than 245, send only from 0xcd (byte 0) to byte 245 to new processing function
					if ((numbytes == -1) || (numbytes == 0)){
						if(numbytes == 0)
							printf("Signal Controller Timed out\n");
						else
							printf("Signal Controller Client closed\n");
						EthernetIsConnected = 0;
						IsReceiving = 0;
					}
					else {

						IsReceiving = 1;
						pthread_mutex_lock(&spat_message_mutex);

						//printf("Signal Controller calling ntcip1202 copyBytesIntoNtcip1202");
						ntcip1202->copyBytesIntoNtcip1202(buf, numbytes);

						//printf("Signal Controller calling ntcip1202 ToJ2735r41SPAT");
						ntcip1202->ToJ2735r41SPAT(_spat, _intersectionName, _intersectionId);

						//printf("Signal Controller calling _spatMessage set_j2735_data\n");
						_spatMessage.set_j2735_data(_spat);

						pthread_mutex_unlock(&spat_message_mutex);
						PLOG(logDEBUG) << _spatMessage;
					}
				}
			}
			//printf("Sleeping\n");
			sleep(3);
	    }
	}
}

void SignalController::getEncodedSpat(SpatEncodedMessage* spatEncodedMsg)
{
	pthread_mutex_lock(&spat_message_mutex);

	//printf("Signal Controller getEncodedSpat\n");
	spatEncodedMsg->initialize(_spatMessage);

	pthread_mutex_unlock(&spat_message_mutex);

}

int SignalController::getIsConnected()
{
	return EthernetIsConnected && IsReceiving;
}

int SignalController::getActionNumber()
{
	return 1;//sd.actionNumber;
}

