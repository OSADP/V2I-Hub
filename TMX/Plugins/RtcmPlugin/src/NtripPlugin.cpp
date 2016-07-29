/*
 ============================================================================
 Name        : NtripPlugin.c
 Author      : Joshua Branch
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <time.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>

#include <iomanip>
#include "tmx/tmx.h"
#include "tmx/IvpPlugin.h"
#include "tmx/messages/IvpJ2735.h"
#include "tmx/messages/IvpRtcm.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include "Base64.h"
#include <math.h>

#include <asn_j2735_r41/RTCM-Corrections.h>

using namespace std;


struct MountPoint {
	std::string mountPointName;
	IvpRtcmVersion version;

	MountPoint()
	{
		version = IvpRtcmVersion_Unknown;
	}
};

int _socket_fd;
int _connected;
int _ggaSent;

IvpPlugin *gPlugin;


MountPoint gMountPoint;


pthread_mutex_t gLock = PTHREAD_MUTEX_INITIALIZER;

uint64_t getSystemMillis()
{
	timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);

	return (uint64_t)time.tv_sec * 1000 + (uint64_t)time.tv_nsec / 1000000;
}

std::vector<std::string> tokenize(std::string str, std::string sep, bool removeEmpty)
{
	std::vector<std::string> results;

	unsigned int startPosition = 0;

	unsigned int i;
	for(i = 0; i < str.length(); i++)
	{
		if (sep.find(str.at(i)) != std::string::npos)
		{
			results.push_back(str.substr(startPosition, i - startPosition));
			startPosition = i + 1;
		}
	}
	if (i != startPosition)
		results.push_back(str.substr(startPosition, i - startPosition));

	if (removeEmpty)
	{
		for(std::vector<std::string>::iterator itr = results.begin(); itr != results.end() && results.size() > 0; itr++)
		{
			if (itr->empty())
			{
				results.erase(itr);
				itr = results.begin();
			}
		}
	}

	return results;
}

void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	std::cout << "State Change: " << state << std::endl;

	if (state == IvpPluginState_registered)
	{

	}
	else
	{

	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	std::cerr << "Error: " << err.error << "/" << err.sysErrNo << " Level: "
			<< err.level << std::endl;
}

void onMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	assert(msg != NULL);

	pthread_mutex_lock(&gLock);

	if (msg->payload != NULL && msg->payload->type == cJSON_String)
	{

		if(_connected && !_ggaSent)
		{
			_ggaSent = 1;

			string dataToSend = msg->payload->valuestring;

			dataToSend = dataToSend + "\r\n";

			cout<<"SEND ***"<<endl<<dataToSend<<endl;

			send(_socket_fd, dataToSend.c_str(), dataToSend.size(), 0);
		}
	}

	pthread_mutex_unlock(&gLock);
}

void onConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	std::cout << "Config Changed | " << key << " : " << value << std::endl;
}

void ntripStreamCallback(char *ptr, size_t nmemb)
{
	static char buf[5000];
	static unsigned int bufPosition = 0;

	for(unsigned int i = 0; i < nmemb && bufPosition < sizeof(buf); i++, bufPosition++)
	{
		buf[bufPosition] = ptr[i];
	}


	ivp_setStatusItem(gPlugin, "NTRIP Stream", "Streaming");

	std::cout << nmemb << " -> ";

	for(unsigned int i = 0; i < nmemb; i++)
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)ptr[i] << " ";

	std::cout << std::endl;

	RTCM_Corrections_t rtcm;
	memset(&rtcm, 0, sizeof(RTCM_Corrections_t));
	rtcm.msgID = DSRCmsgID_rtcmCorrections;

	/*rtcm.msgCnt = gMessageCount++;
	if (gMessageCount > 127) gMessageCount = 0;*/

	rtcm.rev = RTCM_Revision_rtcmRev3_0;

	rtcm.rtcmHeader.buf = (uint8_t *)malloc(5*sizeof(uint8_t));
	rtcm.rtcmHeader.size = 5;
	rtcm.rtcmHeader.buf[0] = 0x0E;
	rtcm.rtcmHeader.buf[1] = 0x00;
	rtcm.rtcmHeader.buf[2] = 0x00;
	rtcm.rtcmHeader.buf[3] = 0x00;
	rtcm.rtcmHeader.buf[4] = 0x00;


	size_t bytesRemaining = fmin(nmemb,5000);
	size_t offset = 0;

	while (bytesRemaining > 0)
	{
		size_t byteCount = bytesRemaining;
		if (byteCount > 1023)
		{
			byteCount = 1023;
		}
		bytesRemaining -= byteCount;

		RTCMmsg *rtcmMessage = (RTCMmsg*)calloc(1, sizeof(RTCMmsg));
		rtcmMessage->payload.buf = (uint8_t *)malloc(byteCount);
		for(unsigned int i = 0; i < byteCount; i++)
		{
			rtcmMessage->payload.buf[i] = ptr[i + offset];
		}
		rtcmMessage->payload.size = byteCount;
		asn_set_add(&rtcm.rtcmSets.list, rtcmMessage);

		offset += byteCount;
	}

	cout<<"Creating RTCM J2735 MSG"<<endl;
	IvpMessage *msg = ivpJ2735_createMsg(&rtcm, DSRCmsgID_rtcmCorrections, IvpMsgFlags_RouteDSRC);
	if (msg != NULL)
	{
		ivpMsg_addDsrcMetadata(msg, 172, 0x8000);
		ivp_broadcastMessage(gPlugin, msg);
		ivpMsg_destroy(msg);
	}

}

void createSocket(string ipaddress, int socket)
{

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
	addr.sin_port = htons(socket);

	_socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(connect(_socket_fd, (struct sockaddr *)&addr, sizeof(addr))!=0)
	{
		perror("connect failed");
	}

}

int main(void)
{
	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;
	info.onError = onError;
	info.onConfigChanged = onConfigChanged;

	gPlugin = ivp_create(info);

	_ggaSent =0;
	_connected =0;

	if (!gPlugin)
	{
		std::cerr << "Error creating gPlugin" << std::endl;
		return EXIT_FAILURE;
	}

	IvpMsgFilter *filter = ivpSubscribe_addFilterEntry(NULL, "NMEA", "GGA");
	ivp_subscribe(gPlugin, filter);
	ivpSubscribe_destroyFilter(filter);


	ivp_setStatusItem(gPlugin, "NTRIP Stream", "Closed");

	sleep(1);

	while (gPlugin->state != IvpPluginState_registered )
	{
		sleep(1);
	}

	char *endpointCopy = ivp_getCopyOfConfigurationValue(gPlugin,
			"Endpoint IP");
	if (endpointCopy == NULL)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"No configuration value '" "Endpoint IP" "'");
		ivp_setStatus(gPlugin, "No configuration value '" "Endpoint IP" "'");

	}
	std::string endpoint(endpointCopy);
	free(endpointCopy);

	char *port = ivp_getCopyOfConfigurationValue(gPlugin, "Endpoint Port");
	if (port == NULL)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"No configuration value '" "Endpoint Port" "'");
		ivp_setStatus(gPlugin,
				"No configuration value '" "Endpoint Port" "'");

	}
	int portNumber = atoi(port);
	free(port);
	if (portNumber == 0)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"Invalid configuration value '" "Endpoint Port" "'");
		ivp_setStatus(gPlugin,
				"Invalid configuration value '" "Endpoint Port" "'");

	}

	char *usernameCopy = ivp_getCopyOfConfigurationValue(gPlugin,
			"Username");
	if (usernameCopy == NULL)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"No configuration value '" "Username" "'");
		ivp_setStatus(gPlugin, "No configuration value '" "Username" "'");

	}
	std::string username(usernameCopy);
	free(usernameCopy);

	char *passwordCopy = ivp_getCopyOfConfigurationValue(gPlugin,
			"Password");
	if (passwordCopy == NULL)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"No configuration value '" "Password" "'");
		ivp_setStatus(gPlugin, "No configuration value '" "Password" "'");

	}
	std::string password(passwordCopy);
	free(passwordCopy);

	char *mountpointCopy = ivp_getCopyOfConfigurationValue(gPlugin,
			"Mountpoint");
	if (mountpointCopy == NULL)
	{
		ivp_addEventLog(gPlugin, IvpLogLevel_fatal,
				"No configuration value '" "Mountpoint" "'");
		ivp_setStatus(gPlugin, "No configuration value '" "Mountpoint" "'");

	}
	std::string mountPointName(mountpointCopy);
	free(mountpointCopy);


	ivp_setStatus(gPlugin, IVP_STATUS_RUNNING);


	ivp_setStatusItem(gPlugin, "NTRIP Stream", "Connecting");

	createSocket(endpoint, portNumber);

	std::stringstream header;
	header << "GET /"<<mountPointName<<" HTTP/1.0\r\n";
	header <<"User-Agent: NTRIP GNSSInternetRadio/1.2.0\r\n";
	std::string userpass = username + std::string(":") + password;
	header << "Authorization: Basic " << base64::encode((unsigned char *)userpass.c_str(), userpass.length()) << "\r\n\r\n";


	string dataToSend = header.str();


	cout<<"SEND ***"<<endl<<dataToSend<<endl;

	send(_socket_fd, dataToSend.c_str(), dataToSend.size(), 0);
	//changed buffer size to get the packet size below 1024, which is the max that the arada could transmit.  Needed something smaller.
	//chose 512 bytes since it is 1/2 of the size of the packet we wanted to send, which was 1024.
	//768 did not work
	char buff[512]; 
	bzero(&buff, sizeof(buff));

	int bytesRecvd=0;

	while (gPlugin->state != IvpPluginState_error &&
			(bytesRecvd = recv(_socket_fd, &buff, sizeof(buff),0))!=-1)
	{
		if(bytesRecvd>0){
			printf("Received %d bytes\n", bytesRecvd);
			printf("***********************************\n");
			fwrite(buff, bytesRecvd, 1, stdout);
			if(_connected==0)
			{
				if(bytesRecvd >= 12)
				{
					int compare = strncmp("ICY 200 OK\r\n", buff, 12);
					if(compare!=0)
					{
						exit(1);
					}
					_connected=1;
				}
			}
			else
			{
				//fwrite(buff, bytesRecvd, 1, stdout);
				cout<<"Calling ntripStreamCallback"<<endl;
				ntripStreamCallback(buff, bytesRecvd);
			}
			printf("***********************************\n");
		}
		time_t lastReceivedTime = time(NULL);

		sleep(5);
	}

	std::cout << "Closed..." << std::endl;
	close(_socket_fd);

	return EXIT_SUCCESS;
}
