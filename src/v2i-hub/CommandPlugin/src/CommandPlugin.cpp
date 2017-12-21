//============================================================================
// Name        : CommandPlugin.cpp
// Author      : Battelle Memorial Institute - Ben Paselsky (paselsky@battelle.org)
// Version     :
// Copyright   : Copyright (c) 2017 Battelle Memorial Institute. All rights reserved.
// Description : Plugin that listens for websocket connections from the TMX admin portal
//				 and processes commands
//============================================================================

#include "CommandPlugin.h"

namespace CommandPlugin
{

string CommandPlugin::_databaseAddress = "127.0.0.1";
string CommandPlugin::_databasePort = "3306";
uint64_t CommandPlugin::_updateIntervalMS = 1000;
uint64_t CommandPlugin::_heartbeatIntervalMS = 30000;
uint64_t CommandPlugin::_lastPluginsUpdateTimeMS = 0;
uint64_t CommandPlugin::_nextSession = 1;
TmxControl CommandPlugin::_tmxControl;
uint64_t CommandPlugin::_connectionCount = 0;
bool CommandPlugin::_haveList = false;
bool CommandPlugin::_haveConfig = false;
bool CommandPlugin::_haveStatus = false;
bool CommandPlugin::_haveState = false;
bool CommandPlugin::_haveMessages = false;
bool CommandPlugin::_haveSystemConfig = false;
bool CommandPlugin::_haveEvents = false;
string CommandPlugin::_listJSON = "";
string CommandPlugin::_configJSON = "";
string CommandPlugin::_statusJSON = "";
string CommandPlugin::_stateJSON = "";
string CommandPlugin::_messagesJSON = "";
string CommandPlugin::_systemConfigJSON = "";
string CommandPlugin::_eventsJSON = "";
string CommandPlugin::_lastEventTime = "";
std::map<string, string> CommandPlugin::_listPluginsJSON;
std::map<string, string> CommandPlugin::_configPluginsJSON;
std::map<string, string> CommandPlugin::_statusPluginsJSON;
std::map<string, string> CommandPlugin::_statePluginsJSON;
std::map<string, string> CommandPlugin::_messagesPluginsJSON;
std::map<string, string> CommandPlugin::_systemConfigPluginsJSON;
std::map<string, string> CommandPlugin::_listPluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_configPluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_statusPluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_statePluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_messagesPluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_systemConfigPluginsUpdatesJSON;
std::map<string, string> CommandPlugin::_listPluginsRemoveJSON;
std::map<string, string> CommandPlugin::_configPluginsRemoveJSON;
std::map<string, string> CommandPlugin::_statusPluginsRemoveJSON;
std::map<string, string> CommandPlugin::_statePluginsRemoveJSON;
std::map<string, string> CommandPlugin::_messagesPluginsRemoveJSON;
std::map<string, string> CommandPlugin::_systemConfigPluginsRemoveJSON;
string CommandPlugin::_eventsUpdatesJSON = "";


/**
 * Construct a new CommandPlugin with the given name.
 *
 * @param name The name to give the plugin for identification purposes
 */
CommandPlugin::CommandPlugin(string name) : PluginClient(name)
{
	//set tmxcontrol connection url
	_tmxControl.SetConnectionUrl(string("tcp://" + _databaseAddress + ":" + _databasePort));
	_tmxControl.DisablePermissionCheck();

	// Add a message filter and handler for each message this plugin wants to receive.

	// Subscribe to all messages specified by the filters above.

}

CommandPlugin::~CommandPlugin()
{
}

void CommandPlugin::OnConfigChanged(const char *key, const char *value)
{
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void CommandPlugin::OnStateChange(IvpPluginState state)
{
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered)
	{
		UpdateConfigSettings();
	}
}

void CommandPlugin::UpdateConfigSettings()
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

uint64_t CommandPlugin::GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ((double) (tv.tv_sec) * 1000
			+ (double) (tv.tv_usec) / 1000);
}



int CommandPlugin::WSCallbackHTTP(
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
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		break;
	case LWS_CALLBACK_HTTP:
		//lws_write(wsi, (unsigned char*)response, strlen(response), LWS_WRITE_HTTP);
		break;
	default:
		break;
	}

	return 0;
}


int CommandPlugin::WSCallbackBASE64(
		struct lws *wsi,
		enum lws_callback_reasons reason,
		void *user,
		void *in, size_t len)
{
	//if (!user)
	//	return 0;
	struct PerSessionDataBASE64 *psdata = (struct PerSessionDataBASE64 *)user;
	FILE_LOG(logDEBUG) << "WSCallbackBASE64 reason: " << (int)reason;
	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		//FILE_LOG(logDEBUG) << "WSCallbackBASE64 initial lists";
		{
			_connectionCount++;
			//initialize per session data
			psdata->bufferLength = 0;
			psdata->lastUpdateSendTimeMS = GetMsTimeSinceEpoch();
			psdata->session = _nextSession++;
			psdata->sendFullTelemetry = true;
			psdata->outputbuffer = new string("");
			psdata->lastEventId = 0;
			psdata->authorized = false;
			psdata->authorizationLevel = AuthorizationLevels::ReadOnly;
			psdata->lastHeartbeatSendTimeMS = GetMsTimeSinceEpoch();
		}
		break;
	case LWS_CALLBACK_CLOSED:
		{
			_connectionCount--;
			//clear all data if connection count goes to 0
			if (_connectionCount == 0)
			{
				_lastPluginsUpdateTimeMS = 0;
				_haveList = false;
				_haveConfig = false;
				_haveStatus = false;
				_haveState = false;
				_haveMessages = false;
				_haveSystemConfig = false;
				_haveEvents = false;
				_listJSON = "";
				_configJSON = "";
				_statusJSON = "";
				_stateJSON = "";
				_messagesJSON = "";
				_systemConfigJSON = "";
				_eventsJSON = "";
				_lastEventTime = "";
				_listPluginsJSON.clear();
				_configPluginsJSON.clear();
				_statusPluginsJSON.clear();
				_statePluginsJSON.clear();
				_messagesPluginsJSON.clear();
				_systemConfigPluginsJSON.clear();
				_listPluginsUpdatesJSON.clear();
				_configPluginsUpdatesJSON.clear();
				_statusPluginsUpdatesJSON.clear();
				_statePluginsUpdatesJSON.clear();
				_messagesPluginsUpdatesJSON.clear();
				_systemConfigPluginsUpdatesJSON.clear();
				_listPluginsRemoveJSON.clear();
				_configPluginsRemoveJSON.clear();
				_statusPluginsRemoveJSON.clear();
				_statePluginsRemoveJSON.clear();
				_messagesPluginsRemoveJSON.clear();
				_systemConfigPluginsRemoveJSON.clear();
				_eventsUpdatesJSON = "";
			}
			//delete allocated buffer
			delete psdata->outputbuffer;
		}
		break;
	case LWS_CALLBACK_RECEIVE:
		//FILE_LOG(logDEBUG) << "WSCallbackBASE64 RECEIVE len: " << len;
		{
			//decode message
			string message = Base64::Decode(string((const char *)in, len));
			const char *msgBytes = message.c_str();
			for (unsigned int i = 0;i<message.length();i++)
			{
				if (msgBytes[i] == 0x02)
				{
					//start of message
					//reset count
					psdata->bufferLength = 0;
				}
				else if (msgBytes[i] == 0x03)
				{
					//end of message
					if (psdata->bufferLength > 0)
					{
						//process message
						if (psdata->bufferLength >= READ_BUFFER_SIZE)
						{
							//buffer is full, cant process
							FILE_LOG(logDEBUG) << "WSCallbackBASE64 incoming message is too large";
						}
						else
						{
							//terminate string
							psdata->buffer[psdata->bufferLength] = '\0';
							//process message
							FILE_LOG(logDEBUG) << "WSCallbackBASE64 processing message '" << psdata->buffer << "'";

							try
							{
								// Read the JSON into a boost property tree.
								ptree pt;
								ptree header;
								ptree payload;
								istringstream is(psdata->buffer);
								read_json(is, pt);
								//get header info
								header = pt.get_child("header");
								string msgType = header.get<string>("type");
								string msgSubtype = header.get<string>("subtype");
								if (msgType == "Command" && msgSubtype == "Execute")
								{
									//process command
									payload = pt.get_child("payload");
									//ptree authorization = payload.get_child("authorization");
									//string user = authorization.get<string>("user");
									//string password = authorization.get<string>("password");
									string command = payload.get<string>("command");
									string id = payload.get<string>("id");
									map<string, string> argsList;
									try
									{
										//get arg list
										ptree args = payload.get_child("args");
										BOOST_FOREACH(ptree::value_type &arg, args)
										{
											argsList[arg.first] = arg.second.data();
										}
									}
									catch (exception argsEx)
									{
										//no args
										FILE_LOG(logDEBUG) << "WSCallbackBASE64 process command error: no arguments";
									}

									//check authorization
									if (psdata->authorized)
									{
										//process command if authorized at correct level
										if (command == "enable" && psdata->authorizationLevel >= AuthorizationLevels::ApplicationAdministrator)
										{
											if (argsList.find("plugin") != argsList.end())
											{
												TmxControl::pluginlist plugins;
												plugins.push_back(argsList["plugin"]);
												bool rc = _tmxControl.enable(plugins);
												if (rc)
													FILE_LOG(logDEBUG) << "WSCallbackBASE64 enable " << argsList["plugin"] << " success";
												else
													FILE_LOG(logDEBUG) << "WSCallbackBASE64 enable " << argsList["plugin"] << " failed";
											}
										}
										else if (command == "disable" && psdata->authorizationLevel >= AuthorizationLevels::ApplicationAdministrator)
										{
											if (argsList.find("plugin") != argsList.end())
											{
												TmxControl::pluginlist plugins;
												plugins.push_back(argsList["plugin"]);
												bool rc = _tmxControl.disable(plugins);
												if (rc)
													FILE_LOG(logDEBUG) << "WSCallbackBASE64 disable " << argsList["plugin"] << " success";
												else
													FILE_LOG(logDEBUG) << "WSCallbackBASE64 disable " << argsList["plugin"] << " failed";
											}
										}
										else if (command == "set" && psdata->authorizationLevel >= AuthorizationLevels::ApplicationAdministrator)
										{
											if (argsList.find("plugin") != argsList.end())
											{
												if (argsList.find("key") != argsList.end() && argsList.find("value") != argsList.end())
												{
													TmxControl::pluginlist plugins;
													plugins.push_back(argsList["plugin"]);
													_tmxControl.ClearOptions();
													_tmxControl.SetOption("key", argsList["key"]);
													_tmxControl.SetOption("value", argsList["value"]);
													if (argsList.find("defaultValue") != argsList.end())
														_tmxControl.SetOption("defaultValue", argsList["defaultValue"]);
													else
														_tmxControl.SetOption("defaultValue", argsList[""]);
													if (argsList.find("description") != argsList.end())
														_tmxControl.SetOption("description", argsList["description"]);
													else
														_tmxControl.SetOption("description", argsList["Added by CommandPlugin"]);
													bool rc;
													//check if we are setting a system config parameter
													if (argsList["plugin"] == "SYSTEMCONFIG")
														rc = _tmxControl.set_system(plugins);
													else
														rc = _tmxControl.set(plugins);
													if (rc)
														FILE_LOG(logDEBUG) << "WSCallbackBASE64 set " << argsList["plugin"] << ": " << argsList["key"] << "=" << argsList["value"] << " success";
													else
														FILE_LOG(logDEBUG) << "WSCallbackBASE64 set " << argsList["plugin"] << ": " << argsList["key"] << "=" << argsList["value"] << " failed";
												}
											}
										}
										else if (command == "logout")
										{
											//set per session data
											psdata->bufferLength = 0;
											psdata->lastUpdateSendTimeMS = GetMsTimeSinceEpoch();
											psdata->session = _nextSession++;
											psdata->sendFullTelemetry = true;
											psdata->outputbuffer->clear();
											psdata->lastEventId = 0;
											psdata->authorized = false;
											psdata->authorizationLevel = AuthorizationLevels::ReadOnly;

											//send response
											std::map<string, string> data;
											BuildCommandResponse(psdata->outputbuffer, id, command, "success", "", data);

											FILE_LOG(logDEBUG) << "WSCallbackBASE64 logout";
										}
										else if (command == "clearlog" && psdata->authorizationLevel >= AuthorizationLevels::SystemAdministrator)
										{
											TmxControl::pluginlist plugins;
											plugins.push_back("%");
											bool rc = _tmxControl.clear_event_log(plugins);
											std::map<string, string> data;
											if (rc)
											{
												FILE_LOG(logDEBUG) << "WSCallbackBASE64 clear_event_log success";
												//clear events data
												_eventsJSON = "";
												_eventsUpdatesJSON = "";
												//send response
												BuildCommandResponse(psdata->outputbuffer, id, command, "success", "", data);

											}
											else
											{
												FILE_LOG(logDEBUG) << "WSCallbackBASE64 clear_event_log failed";
												BuildCommandResponse(psdata->outputbuffer, id, command, "failed", "Database DELETE failed", data);
											}
										}
									}
									else
									{
										//only login command can be processed if not authorized
										if (command == "login")
										{
											if (argsList.find("user") != argsList.end() && argsList.find("password") != argsList.end())
											{
												std::map<string, string> data;
												//get user info
												_tmxControl.ClearOptions();
												_tmxControl.SetOption("username", argsList["user"]);
												if (_tmxControl.user_info())
												{
													//get json output
													string uJSON = _tmxControl.GetOutput(TmxControlOutputFormat_JSON, false);
													if (uJSON == "")
													{
														//user not found
														BuildCommandResponse(psdata->outputbuffer, id, command, "failed", "Invalid user name or password", data);
													}
													else
													{
														bool authorized = false;
														tmx::message_container_type *output = _tmxControl.GetOutput();
														BOOST_FOREACH(ptree::value_type &userInfo, output->get_storage().get_tree())
														{
															BOOST_FOREACH(ptree::value_type &userData, userInfo.second)
															{
																//check each user record returned
																if (userData.second.get<string>("username") == argsList["user"] && userData.second.get<string>("password") == argsList["password"] && authorized == false)
																{
																	//send response
																	authorized = true;
																	data["level"] = userData.second.get<string>("accessLevel");
																	BuildCommandResponse(psdata->outputbuffer, id, command, "success", "", data);
																	//set session data
																	psdata->authorized = true;
																	psdata->authorizationLevel = stoi(userData.second.get<string>("accessLevel"));
																}
															}
														}
														if (!authorized)
														{
															//bad password
															BuildCommandResponse(psdata->outputbuffer, id, command, "failed", "Invalid user name or password", data);
														}

													}
												}
												else
												{
													//db query failed
													BuildCommandResponse(psdata->outputbuffer, id, command, "failed", "Cannot get user info", data);
												}

											}
										}
									}
								}
							}
							catch (exception ex)
							{
								//parse error
								FILE_LOG(logDEBUG) << "WSCallbackBASE64 process message exception";
							}

						}
					}
				}
				else
				{
					//part of message
					if (psdata->bufferLength >= READ_BUFFER_SIZE)
					{
						//buffer is full, cant process
						//FILE_LOG(logDEBUG) << "WSCallbackBASE64 incoming message is too large";
					}
					else
					{
						//add to buffer
						psdata->buffer[psdata->bufferLength] = msgBytes[i];
						psdata->bufferLength++;
					}
				}
			}
		}
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			//check authorization, minimum level of readonly allows telemetry
			if (psdata->authorized)
			{
				//check if we need to send full telemetry or update
				if (psdata->sendFullTelemetry)
				{
					FILE_LOG(logDEBUG) << "WSCallbackBASE64 Full";
					psdata->sendFullTelemetry = false;
					//set last update time so we dont immediately send update telemetry
					psdata->lastUpdateSendTimeMS = _lastPluginsUpdateTimeMS;
					//build full telemetry
					BuildFullTelemetry(psdata->outputbuffer, "List");
					BuildFullTelemetry(psdata->outputbuffer, "Config");
					BuildFullTelemetry(psdata->outputbuffer, "Status");
					BuildFullTelemetry(psdata->outputbuffer, "State");
					BuildFullTelemetry(psdata->outputbuffer, "Messages");
					BuildFullTelemetry(psdata->outputbuffer, "SystemConfig");
					BuildFullTelemetry(psdata->outputbuffer, "Events");
				}
				else
				{
					//check if time to send updates
					if (psdata->lastUpdateSendTimeMS < _lastPluginsUpdateTimeMS)
					{
						FILE_LOG(logDEBUG) << "WSCallbackBASE64 send updates, " << psdata->session << ", " << psdata->lastUpdateSendTimeMS << ", " << _lastPluginsUpdateTimeMS;
						//set last send time
						psdata->lastUpdateSendTimeMS = _lastPluginsUpdateTimeMS;
						//build updates
						BuildUpdateTelemetry(psdata->outputbuffer, "List");
						BuildUpdateTelemetry(psdata->outputbuffer, "Config");
						BuildUpdateTelemetry(psdata->outputbuffer, "Status");
						BuildUpdateTelemetry(psdata->outputbuffer, "State");
						BuildUpdateTelemetry(psdata->outputbuffer, "Messages");
						BuildUpdateTelemetry(psdata->outputbuffer, "SystemConfig");
						BuildUpdateTelemetry(psdata->outputbuffer, "Events");
					}
				}
			}
			uint64_t currentTime = GetMsTimeSinceEpoch();
			//always send heartbeat if connected
			if (currentTime >= psdata->lastHeartbeatSendTimeMS + _heartbeatIntervalMS)
			{
				//set last send time
				psdata->lastHeartbeatSendTimeMS = currentTime;
				BuildFullTelemetry(psdata->outputbuffer, "Heartbeat");
			}

			//check if theres something to send
			if (psdata->outputbuffer->length() > 0)
			{
				//send data
				SendData(psdata->outputbuffer, wsi);
			}
		}
		break;
	default:
		break;
	}

	return 0;
}

int CommandPlugin::Main()
{
	PLOG(logINFO) << "Starting plugin.";

	struct lws_context *context;

	// list of supported protocols and callbacks
	struct lws_protocols protocols[] = {
			{
					"http",
					(lws_callback_function*)&CommandPlugin::WSCallbackHTTP,
					0,
					0
			},
			{
					"base64",
					(lws_callback_function*)&CommandPlugin::WSCallbackBASE64,
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

	info.port = 19760;
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
			EventLogMessage msg;
			uint64_t currentTime = GetMsTimeSinceEpoch();
			//check if plugins data needs updated only if we have connections
			if (_connectionCount > 0 && currentTime >= _lastPluginsUpdateTimeMS + _updateIntervalMS)
			{
				//set update time
				_lastPluginsUpdateTimeMS = currentTime;
				//update data
				GetTelemetry("List");
				GetTelemetry("Config");
				GetTelemetry("Status");
				GetTelemetry("State");
				GetTelemetry("Messages");
				GetTelemetry("SystemConfig");
				GetEventTelemetry();
			}

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


} /* namespace CommandPlugin */

int main(int argc, char *argv[])
{
	return run_plugin<CommandPlugin::CommandPlugin>("CommandPlugin", argc, argv);
}
