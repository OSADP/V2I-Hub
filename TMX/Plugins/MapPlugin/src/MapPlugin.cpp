
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <time.h>
#include <sys/time.h>

#include <tmx/tmx.h>
#include <tmx/IvpPlugin.h>
#include <tmx/messages/IvpBattelleDsrc.h>
#include <tmx/messages/IvpSignalControllerStatus.h>
#include <asn_j2735_r41/MapData.h>
#include <asn_j2735_r41/UPERframe.h>
#include <tmx/messages/IvpJ2735.h>
#include "XmlMapParser.h"
#include "ConvertToJ2735r41.h"
#include "PluginClient.h"

#include "utils/common.h"
#include "utils/map.h"

#include <MapSupport.h>
using namespace std;
using namespace tmx;
using namespace tmx::utils;

namespace MapPlugin {

UPERframe _uperFrameMessage;

int _mapAction = -1;
bool _isMapFilesNew = false;
bool _isMapLoaded = false;

volatile int gMessageCount = 0;

uint64_t GetMsTimeSinceEpoch() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ((double) (tv.tv_sec) * 1000
			+ (double) (tv.tv_usec) / 1000);
}

class MapPlugin: public PluginClient {
public:
	MapPlugin(string name);
	virtual ~MapPlugin();

	virtual int Main();
protected:
	void UpdateConfigSettings();

	// Virtual method overrides.
	void OnConfigChanged(const char *key, const char *value);
	void OnMessageReceived(IvpMessage *msg);
	void OnStateChange(IvpPluginState state);
private:
	uint64_t gFrequency = 0;
	std::map<int, std::string> _mapFiles;
	std::mutex data_lock;

	bool LoadMapFile(map *, ConvertToJ2735r41 *);
	bool ParseMapFilesJson(cJSON *root);
	void DebugPrintMapFiles();
};

MapPlugin::MapPlugin(string name) :
		PluginClient(name) {
	AddMessageFilter(IVPMSG_TYPE_SIGCONT, "ACT", IvpMsgFlags_None);
	SubscribeToMessages();
}

MapPlugin::~MapPlugin() {

}

void MapPlugin::UpdateConfigSettings() {
	GetConfigValue<uint64_t>("Frequency", gFrequency, &data_lock);

	string rawMapFiles;
	GetConfigValue<string>("MAP_Files", rawMapFiles, &data_lock);

	if (rawMapFiles.length() > 0) {
		cJSON *root = cJSON_Parse(rawMapFiles.c_str());

		if (ParseMapFilesJson(root)) {
			_isMapFilesNew = true;
			DebugPrintMapFiles();
		} else {
			std::cout << "Error parsing MapFiles config setting." << std::endl;
		}
	}

}

void MapPlugin::OnConfigChanged(const char *key, const char *value) {
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void MapPlugin::OnStateChange(IvpPluginState state) {
	PluginClient::OnStateChange(state);

	if (state == IvpPluginState_registered) {
		UpdateConfigSettings();
	}
}

void MapPlugin::OnMessageReceived(IvpMessage *msg) {
	PluginClient::OnMessageReceived(msg);

	if ((strcmp(msg->type, IVPMSG_TYPE_SIGCONT) == 0)
			&& (strcmp(msg->subtype, "ACT") == 0)
			&& (msg->payload->type == cJSON_String)) {
		int action = ivpSigCont_getIvpSignalControllerAction(msg);
		if (_mapAction != action) {
			_mapAction = action;
			_isMapFilesNew = true;
		}
	}
}

int MapPlugin::Main() {
	PLOG(logINFO) << "Starting plugin.";

	map mm;
	ConvertToJ2735r41 mapConverter;

	/*
	 XmlMapParser mapParser;
	 std::string gidFile = "GID_Telegraph-Twelve_Mile_withEgress.xml";
	 mapParser.ReadGidFile(gidFile, &mm);

	 //map_initialize(&mm);
	 //map_load(&mm);

	 map_encode(&mm, 1);
	 printf("MAP size: %d\n\n", mm.payload.size);
	 for (int i=0; i<mm.payload.size; i++) { printf("%02x ", mm.payload.pblob[i]); }
	 */
	uint64_t lastSendTime = 0;

	while (_plugin->state != IvpPluginState_error) {
		uint64_t frequency = gFrequency;

		if (_isMapFilesNew) {
			_isMapFilesNew = false;

			_isMapLoaded = LoadMapFile(&mm, &mapConverter);

			if (_isMapLoaded) {
				map_encode(&mm, 1);
//				unsigned int i;
//				printf("MAP size: %d\nPayload: ", mm.payload.size);
//				for (i=0; i < mm.payload.size; i++) { printf("%02x ", mm.payload.pblob[i]); }
//				printf("\n");
			} else {
				PLOG(logERROR) << "Map was not loaded correctly";
			}
		}

		uint64_t time = GetMsTimeSinceEpoch();

		if (_isMapLoaded && frequency > 0
				&& (time - lastSendTime) > frequency) {
			lastSendTime = time;

			// Battelle map message
			/*
			 IvpMessage *msg = ivpBattelleDsrc_createMsg(mm.payload.pblob, mm.payload.size, IvpBattelleDsrcMsgType_GID, IvpMsgFlags_RouteDSRC);
			 if (msg != NULL)
			 {
			 ivpMsg_addDsrcMetadata(msg, 172, 0xBFF0);
			 ivp_broadcastMessage(gPlugin, msg);
			 ivpMsg_destroy(msg);
			 }
			 */
			// MapMessage r41
			IvpMessage *msg = ivpJ2735_createMsgFromEncodedwType(
					mapConverter.derEncoded, mapConverter.derEncodedByteCount,
					IvpMsgFlags_RouteDSRC, "MAP-P");
			if (msg != NULL) {
				ivpMsg_addDsrcMetadata(msg, 172, 0x8002);
				ivp_broadcastMessage(_plugin, msg);
				ivpMsg_destroy(msg);
			}

		}

		usleep(50000);
	}

	return (EXIT_SUCCESS);
}

void Decode(char * payload, int length) {
	MapData map;
	ConvertToJ2735r41 converter;
	converter.decodePerMapData(&map, payload, length);

	ParsedMap _map;

	int intersectionId;
	intersectionId = 0;
	//Why would a map file ever have more than one intersection? hardcode to assume 1 for now.
	int interIter = 0;
	//Grab unique id for intersection.
	intersectionId = map.intersections->list.array[interIter]->id.id;
	Position3D_2_t referencePoint =
			map.intersections->list.array[interIter]->refPoint;
	_map.ReferencePoint.Latitude = referencePoint.lat;
	_map.ReferencePoint.Longitude = referencePoint.Long;
	double baseLatitude = _map.ReferencePoint.Latitude / 10000000.0;
	double baseLongitude = _map.ReferencePoint.Longitude / 10000000.0;

	//Iterate over lanes of intersection.
	for (int l;
			l < map.intersections->list.array[interIter]->laneSet.list.count;
			l++) {
		MapLane mapLane;
		//Get lane reference.
		GenericLane *lane =
				map.intersections->list.array[interIter]->laneSet.list.array[l];
		//get laneid
		long laneId = lane->laneID;
		mapLane.LaneNumber = lane->laneID;

		LaneTypeAttributes_t laneType = lane->laneAttributes.laneType;
		LaneDirection_t dir = lane->laneAttributes.directionalUse;
//		if(dir== )
//		{
//			mapLane._laneDirectionEgress
//		}
		auto laneTypeVal = laneType.present;



		double totalXOffset = 0.0;
		double totalYOffset = 0.0;
		double xOffset;
		double yOffset;
		//iterate over all nodes in this lane.
		for (int n; n < lane->nodeList.choice.nodes.list.count; n++) {
			Node *node = lane->nodeList.choice.nodes.list.array[n];
			long latval = node->delta.choice.node_LatLon.lat;
			long longval = node->delta.choice.node_LatLon.lon;
			NodeOffsetPoint_PR prtypeval = node->delta.present;
//			auto xoff4 = node->delta.choice.node_XY4.x;
//			auto yoff4 = node->delta.choice.node_XY4.y;
//			auto xoff5 = node->delta.choice.node_XY5.x;
//			auto yoff5 = node->delta.choice.node_XY5.y;

			/*The geometry of a lane is described by a list of nodes (always at least two) each with a
			 * Northerly and Easterly offset (positive values) or Southerly and Westerly offsets (negative values).
			 *  The offsets for the first node are relative to the intersections reference point that is given
			 *  as a lat/long position, the offsets for all remaining nodes, after the first one, are relative
			 *   to the previous node.  You should typically set you offset resolution to decimeter.
			 *
			 */
			//std::cout << "xOffset " << xOffset << ", yOffset " << yOffset << std::endl;

			//TODO change decimeters to meters??
			double scaleOffset = 0.1;//GetScaleFactor();
			yOffset = latval * scaleOffset;
			xOffset = longval * scaleOffset;

			totalXOffset += xOffset;
			totalYOffset += yOffset;

			LaneNode laneNode;
			double absLatitude = Conversions::NodeOffsetToLatitude(baseLatitude,totalYOffset);
			double absLongitude = Conversions::NodeOffsetToLongitude(baseLongitude,baseLatitude, totalXOffset);
			//Use the node offset and the reference point to calculate a real lat/long.
			laneNode.Point.Latitude = absLatitude;
			laneNode.Point.Longitude = absLongitude;
			//Add the node
			mapLane.Nodes.push_back(laneNode);
		}
		//Add lane to lane list.
		_map.Lanes.push_back(mapLane);
	}
}

bool MapPlugin::LoadMapFile(map *mapMessage, ConvertToJ2735r41 *mapConverter) {
	if (_mapFiles.empty())
		return false;

	if (_mapAction < 0)
		_mapAction = 1;

	// Try to find a map file for the current action number.
	std::map<int, std::string>::iterator pair = _mapFiles.find(_mapAction);
	if (pair == _mapFiles.end()) {
		PLOG(logWARNING) << "Map file not found for Action " << _mapAction;

		// Could not find a map for the current action number.
		if (_mapFiles.size() > 1) {
			PLOG(logWARNING)
					<< "Multiple map files available, using the first one.";
		}

		// Default to any in the std::map.
		pair = _mapFiles.begin();
		_mapAction = pair->first;
		PLOG(logINFO) << "Using map file for Action " << _mapAction;
	}

	std::string actionString;
	std::stringstream out;
	out << _mapAction;
	actionString = out.str();

	SetStatus<string>("Current Action", actionString);

	XmlMapParser mapParser;
	std::string gidFile = pair->second;

	std::cout << "Loading map file: " << gidFile << std::endl;

	if (mapParser.ReadGidFile(gidFile, mapMessage))
	{
		// Convert Map Blob to new J2735 r41.
		mapConverter->convertMap(
				&mapConverter->mapDataStructure, mapMessage,
				mapConverter->encoded);

		std::cout << std::endl << "Encoded Bytes:" << mapConverter->encodedByteCount << std::endl;

		if (mapConverter->encodedByteCount > 0)
		{
			std::cout << "MAP successfully encoded." << std::endl;

			if (mapConverter->createUPERframe_DERencoded_msg())
			{
				//test play kg. delete
				//Decode((char *) mapConverter->encoded,	mapConverter->encodedBytes);
				//end test play + above function
				return true;
			}
		}
		else
		{
			std::cout << "MAP was not encoded correctly." << std::endl;
		}
	}

	return false;
}

bool MapPlugin::ParseMapFilesJson(cJSON *root) {
	if (root == NULL)
		return false;

	cJSON *actionNumber;
	cJSON *filePath;

	cJSON *item = cJSON_GetObjectItem(root, "MapFiles");

	if (item == NULL || item->type != cJSON_Array)
		return false;

	_mapFiles.clear();

	for (int i = 0; i < cJSON_GetArraySize(item); i++) {
		cJSON *subitem = cJSON_GetArrayItem(item, i);

		actionNumber = cJSON_GetObjectItem(subitem, "Action");
		filePath = cJSON_GetObjectItem(subitem, "FilePath");

		if (actionNumber == NULL || actionNumber->type != cJSON_Number
				|| filePath == NULL || filePath->type != cJSON_String)
			return false;

		_mapFiles.insert(
				std::pair<int, std::string>(actionNumber->valueint,
						filePath->valuestring));
	}

	return true;
}

void MapPlugin::DebugPrintMapFiles() {
	std::map<int, std::string>::iterator iter;

	PLOG(logDEBUG) << _mapFiles.size()
			<< " map files specified by configuration settings:";

	for (iter = _mapFiles.begin(); iter != _mapFiles.end(); iter++) {
		int key = iter->first;
		std::string value = iter->second;
		PLOG(logDEBUG) << "-- Action " << key << " file is " << value;
	}
}

} /* End namespace MapPlugin */

int main(int argc, char *argv[]) {
	return run_plugin<MapPlugin::MapPlugin>("MapPlugin", argc, argv);
}
