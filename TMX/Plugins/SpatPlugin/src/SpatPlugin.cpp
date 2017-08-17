#include "SpatPlugin.h"
#include <tmx/j2735_messages/SpatMessage.hpp>
#include <PluginLog.h>

namespace SpatPlugin {

using namespace std;
using namespace tmx::messages;
using namespace tmx::utils;

SpatPlugin::SpatPlugin(string name) :
		PluginClient(name) {
	//AddMessageFilter(IVPMSG_TYPE_SIGCONT, "ACT", IvpMsgFlags_None);
	//SubscribeToMessages();
}

SpatPlugin::~SpatPlugin() {

}

void SpatPlugin::UpdateConfigSettings() {

	string tmpLocalIp = "";
	GetConfigValue<string>("Local_IP", tmpLocalIp, &data_lock);
	localIp = (char *) tmpLocalIp.c_str();

	string tmpLocalUdpPort = "";
	GetConfigValue<string>("Local_UDP_Port", tmpLocalUdpPort, &data_lock);
	localUdpPort = (char *) tmpLocalUdpPort.c_str();

	string tmpTscIp = "";
	GetConfigValue<string>("TSC_IP", tmpTscIp, &data_lock);
	tscIp = (char *) tmpTscIp.c_str();

	string tmpTscRemoteSnmpPort = "";
	GetConfigValue<string>("TSC_Remote_SNMP_Port", tmpTscRemoteSnmpPort,
			&data_lock);
	tscRemoteSnmpPort = (char *) tmpTscRemoteSnmpPort.c_str();

	string tmpIntersectionName = "";
	GetConfigValue<string>("Intersection_Name", tmpIntersectionName,
			&data_lock);
	intersectionName = (char *) tmpIntersectionName.c_str();

	GetConfigValue<int>("Intersection_Id", intersectionId, &data_lock);

	isConfigurationLoaded = true;
}

void SpatPlugin::OnConfigChanged(const char *key, const char *value) {
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void SpatPlugin::OnStateChange(IvpPluginState state) {
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered) {
		UpdateConfigSettings();
	}
}

void SpatPlugin::OnMessageReceived(IvpMessage *msg) {
	PluginClient::OnMessageReceived(msg);

}

int SpatPlugin::Main() {

	int iCounter = 0;
	SpatEncodedMessage spatEncodedMsg;

	// How long it took to update and send the SPaT message the last time through the loop.
	__useconds_t sendElapsedMicroSec = 0;

	try {
		while (_plugin->state != IvpPluginState_error) {
			// wait to send next message
			if (isConfigurationLoaded) {
				if (!isConfigured) {

					usleep(200000);

					int action = sc.getActionNumber();

					/*pthread_mutex_lock(&gSettingsMutex);
					 std::cout <<  "Get PTLM file specified by configuration settings" << std::endl;
					 std::string ptlmFile = GetPtlmFile(action);
					 pthread_mutex_unlock(&gSettingsMutex);
					 */
					//if (!ptlmFile.empty())
					//{
					_actionNumber = action;

					// sc.spat_message_mutex does not need locked because the thread is not running yet.

					{
						std::lock_guard<std::mutex> lock(data_lock);
						sc.setConfigs(localIp, localUdpPort, tscIp,
								tscRemoteSnmpPort, "", intersectionName,
								intersectionId);
					}
					// Start the signal controller thread.
					sc.Start();
					// Give the spatdata pointer to the message class
					//smr41.setSpatData(sc.getSpatData());

					isConfigured = true;
					//}
				}

				// SPaT must be sent exactly every 100 ms.  So adjust for how long it took to do the last send.
				if (sendElapsedMicroSec < 100000)
					usleep(100000 - sendElapsedMicroSec);
				iCounter++;

				bool messageSent = false;
				PerformanceTimer timer;

				// Update PTLM file if the action number has changed.
				int actionNumber = sc.getActionNumber();
				if (_actionNumber != actionNumber) {
					_actionNumber = actionNumber;

					//pthread_mutex_lock(&gSettingsMutex);
					//std::string ptlmFile = GetPtlmFile(_actionNumber);
					//pthread_mutex_unlock(&gSettingsMutex);

					/*if (!ptlmFile.empty())
					 {
					 pthread_mutex_lock(&sc.spat_message_mutex);
					 sc.updatePtlmFile(ptlmFile.c_str());
					 pthread_mutex_unlock(&sc.spat_message_mutex);
					 }*/
				}
				if (sc.getIsConnected()) {
					SetStatus<string>("TSC Connection", "Connected");

					sc.getEncodedSpat(&spatEncodedMsg);

					
					spatEncodedMsg.set_flags(IvpMsgFlags_RouteDSRC);
					spatEncodedMsg.set_dsrcChannel(172);
					spatEncodedMsg.set_dsrcPsid(0x8002);

					//PLOG(logDEBUG) << spatEncodedMsg;

					BroadcastMessage(static_cast<routeable_message &>(spatEncodedMsg));

					if (iCounter % 20 == 0) {
						iCounter = 0;
						// Action Number
						IvpMessage *actionMsg = ivpSigCont_createMsg(
								sc.getActionNumber());
						if (actionMsg != NULL) {
							ivp_broadcastMessage(_plugin, actionMsg);
							ivpMsg_destroy(actionMsg);
						}
					}
					
				} else {
					SetStatus<string>("TSC Connection", "Disconnected");
				}

				sendElapsedMicroSec = timer.Elapsed().total_microseconds();
//			LOG_DEBUGGING("SpatGen Main Loop Time: " << sendElapsedMicroSec / 1000.0 << " ms, Data sent? " << (messageSent ? "yes" : "no"));
			}
		}
	} catch (exception &ex) {
		stringstream ss;
		ss << "SpatPlugin terminating from unhandled exception: " << ex.what();

		ivp_addEventLog(_plugin, IvpLogLevel_error, ss.str().c_str());
		std::terminate();
	}

	return EXIT_SUCCESS;
}

}
/* End namespace SpatPlugin */

int main(int argc, char *argv[]) {
	return run_plugin<SpatPlugin::SpatPlugin>("SpatPlugin", argc, argv);
}
