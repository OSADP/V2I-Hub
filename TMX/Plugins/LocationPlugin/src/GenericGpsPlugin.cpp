/*
 ============================================================================
 Name        : GenericGpsPlugin.c
 Author      : Battelle Memorial Institute
 Version     :
 Copyright   : Copyright (c) 2014 Battelle Memorial Institute. All rights reserved.
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "tmx/tmx.h"
#include "tmx/IvpPlugin.h"
#include "tmx/messages/IvpNmea.h"
#include "tmx/messages/IvpRtcm.h"
#include "nmealib-0.5.3/nmea/nmea.h"
#include <signal.h>
#include <LocationMessage.h>
#include <tmx/messages/routeable_message.hpp>

#define CONFIGKEY_DEVICEFILE "Device Stream File"
#define CONFIGKEY_MODE "Mode"

#define DEG2RAD (3.1415926535 / 180.0)

using namespace std;
//using namespace tmx::utils;
using namespace tmx::messages;

double lastHeading;

typedef enum {
	Mode_Spoofed,
	Mode_Live
} Mode;

pthread_t thread;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static Mode gMode = Mode_Spoofed;
IvpPlugin *gPlugin = NULL;

nmeaINFO gInfo;
nmeaPARSER gParser;

void (*oldsig_int)(int);
void (*oldsig_kill)(int);
void (*oldsig_quit)(int);
void (*oldsig_term)(int);
void (*oldsig_segv)(int);
//sighandler_t oldsig_kill;
//sighandler_t oldsig_quit;
//sighandler_t oldsig_term;
//sighandler_t oldsig_segv;


void sig(int sig)
{
	if(gPlugin)
		ivp_setStatusItem(gPlugin, "Device Stream State", "Stream Closed");

	signal(SIGINT, oldsig_int);
	signal(SIGKILL, oldsig_kill);
	signal(SIGQUIT, oldsig_quit);
	signal(SIGTERM, oldsig_term);
	signal(SIGSEGV, oldsig_segv);
	raise(sig);
}

void sendNmeaSentence(IvpPlugin *plugin, const char *sentence)
{
	IvpMessage *msg = ivpNmea_createMsg(sentence);
	if (msg)
	{
		ivp_broadcastMessage(plugin, msg);
		ivpMsg_destroy(msg);
	}
}

FILE *gFd = NULL;

void *threadLiveStart(void *arg)
{
	IvpPlugin *plugin = (IvpPlugin *)arg;

	char filename[2000];
	char *fn = ivp_getCopyOfConfigurationValue(plugin, CONFIGKEY_DEVICEFILE);
	if (!fn)
	{
		ivp_setConfigurationValue(plugin, CONFIGKEY_MODE, "SPOOFED");
		return NULL;
	}

	strncpy(filename, fn, 2000);
	free(fn);

	int errorLogged = 0;

	while(gMode == Mode_Live)
	{
		gFd = fopen(filename, "r");
		if (gFd == NULL)
		{
			if (!errorLogged)
			{
				char buf[2000];
				strcpy(buf, "ERROR: Unable to open device stream file '");
				strcat(buf, filename);
				strcat(buf, "'");
				ivp_addEventLog(plugin, IvpLogLevel_warn, buf);
				errorLogged = 1;
			}

			ivp_setStatusItem(plugin, "Device Stream State", "ERROR: Unable to open stream");
			sleep(1);
			continue;
		}

		errorLogged = 0;

		ivp_setStatusItem(plugin, "Device Stream State", "Stream Opened");
		while(gMode == Mode_Live)
		{
			char buf[2000];
			char *results = fgets(buf, 1990, gFd);
			if (results != NULL)
			{
				char *end = strstr(buf, "\r");
				if (end != NULL || (end = strstr(buf, "\n"))!= NULL)
				{
					strcpy(end, "\r\n");
					sendNmeaSentence(plugin, buf);
					pthread_mutex_lock(&lock);
					printf("%s\n\n", &buf[0]);

					nmea_parse(&gParser, &buf[0], strlen(buf), &gInfo);
					pthread_mutex_unlock(&lock);

					nmeaPOS dpos;
					nmea_info2pos(&gInfo, &dpos);
					LocationMessage locationMessage;
					gInfo.lat;
					int latDeg = gInfo.lat / 100;
					double latMin = gInfo.lat - ((double) latDeg * 100.0);
					latMin = latMin/60.0;
					double lat = (double) latDeg + (latMin);
					int lonDeg = gInfo.lon/100;
					double lonMin = gInfo.lon - ((double) lonDeg * 100.0);
					lonMin = lonMin/60.0;
					double lon = (double) lonDeg + (lonMin);
					locationMessage.set_Latitude(lat);
					locationMessage.set_Longitude(lon);
					locationMessage.set_Speed(gInfo.speed);
					if(gInfo.speed < 5.0) //&& gInfo.direction < 0.0000)
					{
						gInfo.direction = lastHeading;
					} else {
						lastHeading = gInfo.direction;
					}
					locationMessage.set_Heading(gInfo.direction);
					locationMessage.set_Altitude(gInfo.elv);
					locationMessage.set_NumSatellites(gInfo.satinfo.inuse);
					locationMessage.set_HorizontalDOP(gInfo.HDOP);
					printf("lat %f\nlon %f\n", lat, lon);
					printf("speed %f\n", gInfo.speed);
					printf("heading %f\n", gInfo.direction);
					//locationMessage.
					//ocationMessage.set_FixQuality(gInfo.fix);
					//locationMessage.set_SignalQuality(gInfo.sig);
					//locationMessage.set_Id(NewGuid());
					//locationMessage.set_Time(to_string(gInfo.utc)));
					tmx::routeable_message routeableMsg;
					routeableMsg.initialize(locationMessage);
					//BroadcastMessage(routeableMsg);
					IvpMessage *ivpMsg = routeableMsg.get_message();
					ivp_broadcastMessage(plugin, ivpMsg);
					ivpMsg_destroy(ivpMsg);
					ivpMsg = NULL;
				}

			}
			else
				break;

		}

		fclose(gFd);
		gFd = NULL;
		ivp_setStatusItem(plugin, "Device Stream State", "Stream Closed");
		usleep(100000);
		pthread_testcancel();
	}

	return NULL;
}

void *threadSpoofStart(void *arg)
{
	IvpPlugin *plugin = (IvpPlugin *)arg;

	nmeaINFO info;
	nmeaPOS dpos;
	char buff[2048];
	int gen_sz;

	nmea_zero_INFO(&info);

	info.sig = 1;
	info.fix = 3;
	info.satinfo.inuse = 0;
	info.satinfo.inview = 0;

	const int numberOfSentences = 5;
	const int sentences[] = { GPGGA, GPGSA, GPRMC, GPGSV, GPVTG };

	while(gMode == Mode_Spoofed)
	{
		double lat;
		double lon;
		double speed;
		double elv;
		double course;
		double hdop;
		int quality;
		int fix;

		char *rawLat = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Latitude");
		if (rawLat != NULL)
		{
			lat = strtod(rawLat, NULL);
			free(rawLat);
		}
		char *rawLon = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Longitude");
		if (rawLon != NULL)
		{
			lon = strtod(rawLon, NULL);
			free(rawLon);
		}
		char *rawSpeed = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Speed");
		if (rawSpeed != NULL)
		{
			speed = strtod(rawSpeed, NULL);
			free(rawSpeed);
		}
		char *rawElv = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Elevation");
		if (rawElv != NULL)
		{
			elv = strtod(rawElv, NULL);
			free(rawElv);
		}
		char *rawCourse = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Course");
		if (rawCourse != NULL)
		{
			course = strtod(rawCourse, NULL);
			free(rawCourse);
		}
		char *rawQuality = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Quality");
		if (rawQuality != NULL)
		{
			quality = strtol(rawQuality, NULL, 10);
			free(rawQuality);
		}
		char *rawFix = ivp_getCopyOfConfigurationValue(plugin, "Spoofed Fix");
		if (rawFix != NULL)
		{
			fix = strtol(rawFix, NULL, 10);
			free(rawFix);
		}
		char *rawHdop = ivp_getCopyOfConfigurationValue(plugin, "Spoofed HDOP");
		if (rawHdop != NULL)
		{
			hdop = strtod(rawHdop, NULL);
			free(rawHdop);
		}

		dpos.lat = lat * DEG2RAD;
		dpos.lon = lon * DEG2RAD;
		info.speed = speed * NMEA_TUS_MS;
		info.elv = elv;
		info.direction = course;
		info.sig = quality;
		info.fix = fix;
		info.HDOP = hdop;
		nmea_pos2info(&dpos, &info);
		nmea_time_now(&info.utc);

		pthread_mutex_lock(&lock);
		memcpy(&gInfo, &info, sizeof(nmeaINFO));
		pthread_mutex_unlock(&lock);

		int i;
		for(i = 0; i < numberOfSentences; i++)
		{
			gen_sz = nmea_generate(
				&buff[0], 2048, &info,
				sentences[i]
				);

			buff[gen_sz] = 0;

			sendNmeaSentence(plugin, buff);
			printf("%s\n", &buff[0]);

			usleep(200000);
			pthread_testcancel();
		}
	}

	return NULL;
}

void restartThread(IvpPlugin *plugin)
{
	pthread_mutex_lock(&lock);

	ivp_setStatusItem(plugin, "Device Stream State", "Stream Closed");

	pthread_cancel(thread);
	pthread_join(thread, NULL);
	if (gMode == Mode_Live)
		pthread_create(&thread, NULL, threadLiveStart, plugin);
	else
		pthread_create(&thread, NULL, threadSpoofStart, plugin);

	pthread_mutex_unlock(&lock);
}


void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	printf("State Change: %d\n", state);

	if (state == IvpPluginState_registered)
	{
		restartThread(plugin);
	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	fprintf(stderr, "%d - %d, %d\n", err.level, err.error, err.sysErrNo);
}

void onMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	assert(msg != NULL);

	if (gFd == NULL)
		return;

	IvpRtcmData *data = ivpRtcm_getRtcmData(msg);
	if (data != NULL)
	{
		if (data->version == IvpRtcmVersion_23)
		{
			if (gFd != NULL)
				fwrite(data->data, sizeof(uint8_t), data->dataLength, gFd);
		}

		ivpRtcm_destroyRtcmData(data);
	}
}

void onConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	printf("Config Changed | %s: %s\n", key, value);


	if (strcmp(key, CONFIGKEY_MODE) == 0)
	{
		if (strcasecmp(value, "LIVE") == 0)
			gMode = Mode_Live;
		else
			gMode = Mode_Spoofed;

		if (plugin->state == IvpPluginState_registered)
			restartThread(plugin);
	}
	else if (strcmp(key, CONFIGKEY_DEVICEFILE) == 0)
	{
		if (plugin->state == IvpPluginState_registered)
			restartThread(plugin);
	}
}

int main(void) {

	oldsig_int = signal(SIGINT, sig);
	oldsig_kill = signal(SIGKILL, sig);
	oldsig_quit = signal(SIGQUIT, sig);
	oldsig_term = signal(SIGTERM, sig);
	oldsig_segv = signal(SIGSEGV, sig);

    nmea_zero_INFO(&gInfo);
    nmea_parser_init(&gParser);

	pthread_mutex_lock(&lock);

	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;
	info.onError = onError;
	info.onConfigChanged = onConfigChanged;

	gPlugin = ivp_create(info);

	if (!gPlugin)
	{
		printf("Error creating plugin\n");
		return EXIT_FAILURE;
	}

	IvpMsgFilter *filter = NULL;
	filter = ivpSubscribe_addFilterEntry(filter, IVPMSG_TYPE_RTCM, "2.3");
	ivp_subscribe(gPlugin, filter);
	ivpSubscribe_destroyFilter(filter);

	pthread_mutex_unlock(&lock);
	while(gPlugin->state != IvpPluginState_error)
	{
		pthread_mutex_lock(&lock);


		nmeaPOS dpos;
        nmea_info2pos(&gInfo, &dpos);

        char buf[300];

//        snprintf(buf, 300, "%.07f", dpos.lat / DEG2RAD);
//        ivp_setStatusItem(gPlugin, "Latitude", buf);
//        snprintf(buf, 300, "%.07f", dpos.lon / DEG2RAD);
//        ivp_setStatusItem(gPlugin, "Longitude", buf);
//        snprintf(buf, 300, "%.0f", gInfo.direction);
//        ivp_setStatusItem(gPlugin, "Course (true)", buf);
//        snprintf(buf, 300, "%.02f", gInfo.speed / NMEA_TUS_MS);
//        ivp_setStatusItem(gPlugin, "Speed (m/s)", buf);
//        snprintf(buf, 300, "%.02f", gInfo.elv);
//        ivp_setStatusItem(gPlugin, "Elevation", buf);
////        snprintf(buf, 300, "%i", gInfo.fix); //taken care of below
////        ivp_setStatusItem(gPlugin, "Fix Quality", buf);
//        snprintf(buf, 300, "%.02f", gInfo.HDOP);
//        ivp_setStatusItem(gPlugin, "HDOP", buf);
//        snprintf(buf, 300, "%i", gInfo.satinfo.inuse);
//        ivp_setStatusItem(gPlugin, "Sat num", buf);



        switch (gInfo.sig)
        {
        case 0:
            //ivp_setStatusItem(gPlugin, "Signal", "No Signal");
        	break;
        case 1:
            //ivp_setStatusItem(gPlugin, "Signal", "Fix");
        	break;
        case 2:
            //ivp_setStatusItem(gPlugin, "Signal", "Differential");
        	break;
        case 3:
            //ivp_setStatusItem(gPlugin, "Signal", "Sensitive");
        	break;
        default:
            //ivp_setStatusItem(gPlugin, "Signal", "Unknown");
        	break;
        }

        switch (gInfo.fix)
		{
		case 1:
			//ivp_setStatusItem(gPlugin, "Fix", "No Fix");
			break;
		case 2:
			//ivp_setStatusItem(gPlugin, "Fix", "2D");
			break;
		case 3:
			//ivp_setStatusItem(gPlugin, "Fix", "3D");
			break;
		default:
			//ivp_setStatusItem(gPlugin, "Fix", "Unknown");
			break;
		}

        //restartThread(gPlugin);
		pthread_mutex_unlock(&lock);

		sleep(2);
	}

	return EXIT_SUCCESS;
}
