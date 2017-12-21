//============================================================================
// Name        : UIProxyPlugin.cpp
// Author      : Battelle Memorial Institute - Ben Paselsky (paselsky@battelle.org)
// Version     :
// Copyright   : Copyright (c) 2017 Battelle Memorial Institute. All rights reserved.
// Description : Plugin that listens for forwards UI messages over websockets
//============================================================================

#include "UIProxyPlugin.h"

namespace UIProxyPlugin
{


uint64_t UIProxyPlugin::_connectionCount = 0;
uint64_t UIProxyPlugin::_nextSession = 1;

/**
 * Construct a new UIProxyPlugin with the given name.
 *
 * @param name The name to give the plugin for identification purposes
 */
UIProxyPlugin::UIProxyPlugin(string name) : PluginClient(name)
{
	// Add a message filter and handler for each message this plugin wants to receive.

	// Subscribe to all messages specified by the filters above.

}

UIProxyPlugin::~UIProxyPlugin()
{
}

void UIProxyPlugin::OnConfigChanged(const char *key, const char *value)
{
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void UIProxyPlugin::OnStateChange(IvpPluginState state)
{
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered)
	{
		UpdateConfigSettings();
	}
}

void UIProxyPlugin::UpdateConfigSettings()
{
	GetConfigValue<uint64_t>("SleepMS", _sleepMS);
	GetConfigValue<bool>("SSLEnabled", _sslEnabled);
	{
		lock_guard<mutex> lock(_configLock);
		GetConfigValue<string>("SSLPath", _sslPath);
	}

	//PLOG(logDEBUG) << "    Config data - SleepMS: " << SleepMS ;

	_newConfigValues = true;
}

uint64_t UIProxyPlugin::GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ((double) (tv.tv_sec) * 1000
			+ (double) (tv.tv_usec) / 1000);
}


int UIProxyPlugin::WSCallbackHTTP(
		struct lws *wsi,
		enum lws_callback_reasons reason,
		void *user,
		void *in, size_t len)
{
	//FILE_LOG(logDEBUG) << "WSCallbackHTTP reason: " << (int)reason;
	//char response[] = "Hello!";
	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		_connectionCount++;
		break;
	case LWS_CALLBACK_CLOSED:
		_connectionCount--;
		break;
	case LWS_CALLBACK_HTTP:
		//lws_write(wsi, (unsigned char*)response, strlen(response), LWS_WRITE_HTTP);
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		break;
	default:
		break;
	}

	return 0;
}

int UIProxyPlugin::WSCallbackBASE64(
		struct lws *wsi,
		enum lws_callback_reasons reason,
		void *user,
		void *in, size_t len)
{
	struct PerSessionDataBASE64 *psdata = (struct PerSessionDataBASE64 *)user;
	//FILE_LOG(logDEBUG) << "WSCallbackBASE64 reason: " << (int)reason;
	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED";
		{
			_connectionCount++;
			//initialize per session data
			psdata->session = _nextSession++;
			psdata->coreConnected = false;
			//connect to core
			//create socket
			psdata->coreSockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (psdata->coreSockfd < 0)
			{
				FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED socket create failed";
			}
			else
			{
				//get address
				struct sockaddr_in servAddr;
				struct hostent *server;
				server = gethostbyname("127.0.0.1");
				if (server == NULL)
				{
					FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED gethostbyname failed";
				}
				else
				{
					//connect
					bzero((char *) &servAddr, sizeof(servAddr));
					servAddr.sin_family = AF_INET;
					bcopy((char *)server->h_addr, (char *)&servAddr.sin_addr.s_addr, server->h_length);
					servAddr.sin_port = htons(CORE_CONNECT_PORT);
					if (connect(psdata->coreSockfd,(struct sockaddr *) &servAddr,sizeof(servAddr)) < 0)
					{
						FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED socket connect failed";
					}
					else
					{
						//put socket in non-blocking mode
						if(fcntl(psdata->coreSockfd, F_SETFL, fcntl(psdata->coreSockfd, F_GETFL) | O_NONBLOCK) < 0)
						{
							FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED socket non-blocking failed";
							close(psdata->coreSockfd);
						}
						else
						{
							psdata->coreConnected = true;
							FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_ESTABLISHED core connected";
						}
					}
				}
			}
		}
		break;
	case LWS_CALLBACK_CLOSED:
		FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_CLOSED";
		{
			_connectionCount--;
			//clear all data if connection count goes to 0
			if (_connectionCount == 0)
			{

			}
			//delete allocated buffer
			if (psdata->coreConnected)
				close(psdata->coreSockfd);
		}
		break;
	case LWS_CALLBACK_RECEIVE:
		FILE_LOG(logDEBUG) << "WSCallbackBASE64 RECEIVE length: " << len;
		{
			if (psdata->coreConnected == true)
			{
				//decode message
				string message = Base64::Decode(string((const char *)in, len));
				const char *msgBytes = message.c_str();
				//send data to core
				int length = write(psdata->coreSockfd, msgBytes, message.length());
				FILE_LOG(logDEBUG) << "WSCallbackBASE64 bytes sent: " << length;
				if (length < 0)
					psdata->coreConnected = false;
			}
		}
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			//FILE_LOG(logDEBUG) << "WSCallbackBASE64 LWS_CALLBACK_SERVER_WRITEABE";
			//read from core
			if (psdata->coreConnected == true)
			{
				char buffer[READ_BUFFER_SIZE];
				int length = read(psdata->coreSockfd, buffer, READ_BUFFER_SIZE);
				if (length < 0)
				{
					//FILE_LOG(logDEBUG) << "WSCallbackBASE64 CORE READ errno: " << errno;
					if (errno != EAGAIN)
						psdata->coreConnected = false;
				}
				else if (length > 0)
				{
					FILE_LOG(logDEBUG) << "WSCallbackBASE64 CORE READ length: " << length;
					//encode
					string buffer64 = Base64::Encode((const unsigned char*)buffer, length);

					//send to ui
					unsigned char writeBuffer[LWS_SEND_BUFFER_PRE_PADDING + buffer64.length() + LWS_SEND_BUFFER_POST_PADDING];
					memcpy(&writeBuffer[LWS_SEND_BUFFER_PRE_PADDING], buffer64.c_str(), buffer64.length());

					//check if send pipe is choked
					int chokeCount = 0;
					while (lws_send_pipe_choked(wsi) && chokeCount < 4)
					{
						this_thread::sleep_for(chrono::milliseconds(5));
						chokeCount++;
					}
					if (chokeCount < 4)
						lws_write(wsi, &writeBuffer[LWS_SEND_BUFFER_PRE_PADDING], buffer64.length(), LWS_WRITE_TEXT);

				}
			}
		}
		break;
	default:
		break;
	}

	return 0;
}

int UIProxyPlugin::Main()
{
	PLOG(logINFO) << "Starting plugin.";

	struct lws_context *context;

	// list of supported protocols and callbacks
	struct lws_protocols protocols[] = {
			{
					"http",
					(lws_callback_function*)&UIProxyPlugin::WSCallbackHTTP,
					0,
					0
			},
			{
					"base64",
					(lws_callback_function*)&UIProxyPlugin::WSCallbackBASE64,
					sizeof(struct PerSessionDataBASE64),
					0
			},
			{
					NULL, NULL, 0
			}
	};

	//ignore SIGPIPE caused by connection dropped because of rejected certificate
	signal(SIGPIPE, SIG_IGN);

	lws_context_creation_info info;
	memset(&info, 0, sizeof(info));

	info.port = 24603;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	string crtPath = "";
	string keyPath = "";

	//wait for initial config values
	while (!_newConfigValues)
		this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));

	if (_sslEnabled)
	{
		lock_guard<mutex> lock(_configLock);
		crtPath = _sslPath;
		crtPath.append("/tmxcmd.crt");
		info.ssl_cert_filepath = crtPath.c_str();
		keyPath = _sslPath;
		keyPath.append("/tmxcmd.key");
		info.ssl_private_key_filepath = keyPath.c_str();
	}

	// create libwebsocket context representing this server
	context = lws_create_context(&info);

	if (context == NULL)
	{
		PLOG(logDEBUG) << "libwebsocket context create failed";
		return -1;
	}

	PLOG(logDEBUG) << "libwebsocket context created";

	while (_plugin->state != IvpPluginState_error)
	{
		if (_newConfigValues)
		{
			//schedule writable callback for all base64 protocol connections
			lws_callback_on_writable_all_protocol(context, &protocols[1]);

			//service connections
			int sleepTime = _sleepMS;
			lws_service(context, sleepTime);

		}

		//sleep
		this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
	}

	lws_context_destroy(context);

	return (EXIT_SUCCESS);
}


} /* namespace UIProxyPlugin */

int main(int argc, char *argv[])
{
	return run_plugin<UIProxyPlugin::UIProxyPlugin>("UIProxyPlugin", argc, argv);
}
