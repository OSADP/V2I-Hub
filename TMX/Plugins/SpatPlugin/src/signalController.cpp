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

#include "tmx/tmx.h"
#include "tmx/IvpPlugin.h"
#include "tmx/messages/IvpJ2735.h"

#include "signalController.h"
#include "utils/common.h"
#include "utils/spat.h"

void SignalController::Start(spat *msg)
{
	spat_initialize(msg);
	message = msg;
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

void SignalController::setConfigs(char* localIp, char* localUdpPort, char* tscIp, char* tscRemoteSnmpPort, const char* ptlmFile)
{
	_localIp = strdup(localIp);
	_localUdpPort = strdup(localUdpPort);
	_ptlmFile = strdup(ptlmFile);

	sc.setConfigs(tscIp, tscRemoteSnmpPort);
}

void SignalController::updatePtlmFile(const char* ptlmFile)
{
	_ptlmFile = strdup(ptlmFile);
	sd.updatePtlmFile(_ptlmFile);
}

void SignalController::start_signalController()
{
#ifndef __CYGWIN__
	prctl(PR_SET_NAME, "SpatGenSC", 0, 0, 0);
#endif

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	int maxDataSize = 1000;

    int sockfd, numbytes;
    char buf[maxDataSize];
    struct addrinfo hints, *servinfo;
    int rv;
    struct timeval tv;
    int on = 1;

	int returnVal = sd.initializeSpat(&sc, _ptlmFile);
	if (!returnVal)
	{
		printf("Initialize Spat Failed:%d", returnVal);
		return;
	}

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
				printf("Signal Controller UDP Client Connected to %s:%s\n",_localIp, _localUdpPort);
				freeaddrinfo(servinfo); // all done with this structure
//				client_socket = sockfd;
				// Receive Packets and process until disconnected
				while(EthernetIsConnected) {
					numbytes = recv(sockfd, buf, maxDataSize-1, 0);
					if ((numbytes == -1) || (numbytes == 0)){
						if(numbytes == 0)
							printf("Signal Controller Timed out\n");
						else
							printf("Signal Controller Client closed\n");
						EthernetIsConnected = 0;
					}
					else {
//						printf("Received Data bytes:%d\n", numbytes);

						pthread_mutex_lock(&spat_message_mutex);
						sd.buildSpat((unsigned char *) &buf, numbytes);
						pthread_mutex_unlock(&spat_message_mutex);

//						for (i=0; i<numbytes; i++) { printf("%02x ", (unsigned char) buf[i]); }
//						printf("\n\n");
					}
				}
			}
			printf("Sleeping\n");
			sleep(3);
	    }
	}
}

int SignalController::getIsConnected()
{
	return EthernetIsConnected;
}

int SignalController::getActionNumber()
{
	return sd.actionNumber;
}

void SignalController::spat_update(spat* msg)
{
	sd.convertPtlmToSpat(msg);
}
SPaTData* SignalController::getSpatData()
{
	return &sd;
}
