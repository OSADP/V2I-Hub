#ifndef IVP_SPATDATA_H_
#define IVP_SPATDATA_H_
/* IVP-SPaTData.h
 *
 *  Created on: Sep 30, 2014
 *      Author: ivp
 */

#include "utils/common.h"
#include "utils/spat.h"
#include "SignalControllerNTCIP.h"

typedef enum { is_normal = 0, is_manual = 1, is_stopped = 2, is_faultFlash = 4,
		is_preempt = 8, is_priority = 16, is_coordination = 32, is_coordinationinTransition = 64,
		is_programmedFlash = 128} enum_IntersectionStatus;
typedef enum { protectedPhase , permittedPhase } enum_PhaseType;
typedef enum { unavailableLaneType, pedLane, vehicleLane, specialLane } enum_LanesTypes;
typedef enum { unavailableTSCEntity, phase, overlap } enum_TSCEntityType;
typedef enum { man_pedestrian = 16, man_straight = 1, man_leftturn = 2, man_rightturn = 4, man_uturn = 8 } enum_Maneuvers;
typedef enum { pc_unavailableCount = 0, pc_none = 1, pc_oneormore = 2 } enum_PedestrianCount;
typedef enum
{
	unavailablePhaseStatus = 0,
	Dark = 1,
	Green = 2,
	Yellow = 3,
	Red = 4,
	RedRivert = 5,
	RedClear = 6,
	Walk = 7,
	DontWalk = 8,
	PedClear = 9,
	FlashingDontWalk = 10,
} enum_PhaseStatus;
typedef enum
{
	DarkBall = 0x00,
	GreenBall = 0x01,
	YellowBall = 0x02,
	RedBall = 0x04,
	FlashingGreenBall = 0x09,
	FlashingYellowBall = 0x0A,
	FlashingRedBall = 0x0C,
	GreenLeftArrow = 0x10,
	YellowLeftArrow = 0x20,
	RedLeftArrow = 0x40,
	FlashingGreenLeftArrow = 0x90,
	FlashingYellowLeftArrow = 0xA0,
	FlashingRedLeftArrow = 0xC0,
	GreenRightArrow = 0x0100,
	YellowRightArrow = 0x0200,
	RedRightArrow = 0x0400,
	FlashingGreenRightArrow = 0x0900,
	FlashingYellowRightArrow = 0x0A00,
	FlashingRedRightArrow = 0x0C00,
	GreenSoftLeftArrow = 0x010000,
	YellowSoftLeftArrow = 0x020000,
	RedSoftLeftArrow = 0x040000,
	FlashingGreenSoftLeftArrow = 0x090000,
	FlashingYellowSoftLeftArrow = 0x0A0000,
	FlashingRedSoftLeftArrow = 0x0C0000,
	GreenSoftRightArrow = 0x100000,
	YellowSoftRightArrow = 0x200000,
	RedSoftRightArrow = 0x400000,
	FlashingGreenSoftRightArrow = 0x900000,
	FlashingYellowSoftRightArrow = 0xA00000,
	FlashingRedSoftRightArrow = 0xC00000,
	GreenUTurnArrow = 0x01000000,
	YellowUTurnArrow = 0x02000000,
	RedUTurnArrow = 0x41000000,
	FlashingGreenUTurnArrow = 0x09000000,
	FlashingYellowUTurnArrow = 0x0A000000,
	FlashingRedUTurnArrow = 0x0C000000,
} Enum_LaneMovementStatus;

struct Phase
{
	//Properties
	int Number;
	unsigned int YellowChange;
	unsigned int RedClear;
	unsigned int RedRevert;

	bool Red;
	bool Yellow;
	bool Green;

	enum_PhaseStatus Status;

	bool Flashing;

	//SPaT Data
	unsigned char MinTimeBytes[2];
	unsigned char MaxTimeBytes[2];
	unsigned int MinTime;
	unsigned int MaxTime;
};
struct Ped
{
	//Properties
	int Number;
	unsigned int PedClear;

	bool PedDontWalk;
	bool PedWalk;
	bool PedClearState;

	enum_PhaseStatus Status;

	//SPaT Data
	unsigned char MinTimeBytes[2];
	unsigned char MaxTimeBytes[2];
	unsigned int MinTime;
	unsigned int MaxTime;
};
struct Overlap
{
	//Properties
	int Number;
	unsigned int YellowChange;

	bool Red;
	bool Yellow;
	bool Green;

	enum_PhaseStatus Status;

	bool Flashing;

	//SPaT Data
	unsigned char MinTimeBytes[2];
	unsigned char MaxTimeBytes[2];
	unsigned int MinTime;
	unsigned int MaxTime;
};

/*struct BytesSPaTMsg
{
	unsigned char VMinTime[2];
	unsigned char VMaxTime[2];
	unsigned char PedMinTime[2];
	unsigned char PedMaxTime[2];
	unsigned char OvlpMinTime[2];
	unsigned char OvlpMaxTime[2];
};
*/
struct type_IntersectionStatus
{
		bool ManualControlActive;
		bool StopTimeActive;
		bool FaultFlashActive;
		bool PreemptActive;
		bool TSPActive;
		bool CoordinationActive;
		bool CoordinationInTransitionActive;
		bool ProgrammedFlashActive;
};

struct TSCPedInfo
{
	bool DetectionAvailable;
	bool Call;
	bool Detect;
};

//Structure defining the Phase-to-Lane-Movement input data elements required to generate Movement data necessary for the SPaT message.
struct PTLM
{
	enum_Maneuvers Movement;
	char *Lanes;
	enum_LanesTypes LanesType;
	enum_TSCEntityType TSCEntityType;
	enum_PhaseType PhaseType;
	int TSCPhsOvlp;
	char *PedCalls;
	char *PedDetectors;
	bool Enabled;
	bool callActive;
	int SignalGroup;

	unsigned long state;
	unsigned int minTime;
	unsigned int maxTime;
	unsigned int yellowState;
	unsigned int yellowTime;
	unsigned char pedestrian;	//byte
	//enum_PedestrianCount pedestrian;	//byte
	unsigned char count;		//byte
};

#define _TCHAR wchar_t

class SPaTData
{
	public:
			int actionNumber;
	public:
			// Read values from controller and initialize all of the data elements
			int initializeSpat(SignalControllerNTCIP* sigCon, const char* ptlmFile);
			void updatePtlmFile(const char* ptlmFile);
			int ReadPTLMTable();
			void buildSpat(unsigned char *BytesRecvd, int NumBytesRcvdFromTSC);
			void convertPtlmToSpat(spat* spat);

			int maxPhases;
			int maxPhaseGroups;
			int maxOverlaps;
			int NumPTLMRecords;
			int CSVElements [17];

			SignalControllerNTCIP* sc;

			char *IntersectionName;
			int IntersectionId;
			unsigned char J2735IntersectionStatus;

			unsigned char timestampseconds;
			unsigned char timestamptenths;

			Phase *Phases;
			Ped *Peds;
			Overlap *Overlaps;
			TSCPedInfo *TSCPeds;
			PTLM *PTLMTable;
			int *PedCallsMask;
			int *PedDetectorsMask;
	private:



};

#endif /* IVP_SPATDATA_H_ */
