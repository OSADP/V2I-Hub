// IVP-SPaTData.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "SPaTData.h"
#include "utils/spat.h"
#include "XmlPtlmParser.h"

uint64_t GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)((double)(tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000);
}

long ConvertUnsignedCharArray_To_Long(unsigned char *TimeArray, int NumBytes)
	{
		unsigned long num;
		int i;

		num = 0;

		//for(i = 0; i < sizeof TimeArray; i++)
		for(i = 0; i < NumBytes; i++)
		{
			num <<= 8;
			num |= TimeArray[i];
		}

		return num;
	}

unsigned short ConvertUnsignedCharArray_To_UShort(unsigned char *Bytes, int NumBytes)
{
	if(NumBytes < 2)
		return 0;

	unsigned short number = (Bytes[1] <<8 ) + Bytes[0];

	return number;
}
	
void SplitCSVStringToIntArray(char *CSVString, char *SepChar, int *NumCSVElements, int* CSVElements)
{
//		printf("Split:%s\n", CSVString);
	*NumCSVElements = 0;
	char *token;
	char *running;
	running = strdupa(CSVString);
	token = strsep (&running, SepChar);
	while( token != NULL )
	{
		CSVElements[*NumCSVElements] = atoi(token);
		*NumCSVElements+=1;
		/* Get next token: */
		token = strsep(&running, SepChar);
	}
}

void SPaTData::updatePtlmFile(const char* ptlmFile)
{
	// Read the XML PTLM file.;

	std::string ptlmFileStr = ptlmFile;

	std::cout << "Loading PTLM file: " << ptlmFile << std::endl;
	XmlPtlmParser xmlParser;
	if (xmlParser.ReadFile(ptlmFileStr))
	{
		IntersectionName = xmlParser.IntersectionName;
		IntersectionId = xmlParser.IntersectionId;
		NumPTLMRecords = xmlParser.NumPTLMRecords;
		PTLMTable = xmlParser.PTLMTable;
		std::cout << "Finished Reading Name:" << IntersectionName << " ID:" << IntersectionId << std::endl;
	}
}

int SPaTData::initializeSpat(SignalControllerNTCIP* sigCon, const char* ptlmFile)
{
	sc = sigCon;
	maxPhases = 16;

	int tscmaxPhases = sc->getMaxPhases();
	if (tscmaxPhases != -1)
	{
		printf("Maximum Phases:%d\n", tscmaxPhases);
	}
	else
	{
		printf("MaxPhaseRead Failed using default maxPhases 16\n");
		tscmaxPhases = 16;
	}

	printf("Allocating Memory\n");

	// Allocate phases based on maxPhases returned
	Phases = (Phase *) calloc (maxPhases, sizeof(Phase));
	Peds = (Ped *) calloc (maxPhases, sizeof(Ped));
	Overlaps = (Overlap *) calloc (maxPhases, sizeof(Overlap));
	TSCPeds = (TSCPedInfo *) calloc (maxPhases, sizeof(TSCPedInfo));
	PedCallsMask = (int *) calloc (maxPhases, sizeof(int));
	PedDetectorsMask = (int *) calloc (maxPhases, sizeof(int));


	maxPhaseGroups = sc->getMaxPhaseGroups();
	if (maxPhaseGroups != -1)
	{
		printf("Maximum Phase Groups:%d\n", maxPhaseGroups);
	}
	else
	{
		printf("Maximum Phase Groups Read Failed using 2\n");
		maxPhaseGroups = 2;
	}


	maxOverlaps = sc->getMaxOverlaps();
	if (maxOverlaps != -1)
	{
		printf("Maximum Overlaps:%d\n", maxOverlaps);
	}
	else
	{
		printf("Maximum Overlaps Read Failed using 16\n");
		maxOverlaps = 16;
	}

	// Read the XML PTLM file.
	updatePtlmFile(ptlmFile);
	sc->EnableSpaTPedestrianPush();
	return 1;
}

void SPaTData::buildSpat(unsigned char *BytesRecvd, int NumBytesRcvdFromTSC)
{

	int i = 0;
	long prevTSCTimeMsec = 0;
	long currTSCTimeMsec = 0;

//	printf("Building SPAT\n");

	//Process the Phase, Overlap, and Peds minimum and maximum remaining time. For the location of the bytes in the TSC stream, refer to the SPaT Data desciption document prepared by Econolite
	int StartByte = 3;
	for (int j = 1; j < maxPhases; j++)
	{
		Phases[j].MinTimeBytes[0] = BytesRecvd[StartByte + 1 + (j - 1) * 13];
		Phases[j].MinTimeBytes[1] = BytesRecvd[StartByte + (j - 1) * 13];

		unsigned short minTime = (Phases[j].MinTimeBytes[1] << 8) + Phases[j].MinTimeBytes[0];
		Phases[j].MinTime = ConvertUnsignedCharArray_To_UShort(Phases[j].MinTimeBytes, 2);

		//printf("Phase =%d MinTimeBytes[0] = 0x%02X MaxTimeBytes[1] = 0x%02X\n", j, Phases[j].MinTimeBytes[0], Phases[j].MinTimeBytes[1]);

		Phases[j].MaxTimeBytes[0] = BytesRecvd[StartByte + 3 + (j - 1) * 13];
		Phases[j].MaxTimeBytes[1] = BytesRecvd[StartByte + 2 + (j - 1) * 13];
		Phases[j].MaxTime = ConvertUnsignedCharArray_To_UShort(Phases[j].MaxTimeBytes, 2);

		double minTimeD = (double) Phases[j].MinTime / 10.0;
		double maxTime = (double) Phases[j].MaxTime /10.0;
		//printf("Phase = %d	MinTime = %d	MaxTime = %d\n", j, Phases[j].MinTime, Phases[j].MaxTime);
//		printf("Phase = %d MinTimeRaw = %d	MinTime = %0.1f	MaxTime = %0.1f\n", j, minTime, minTimeD, maxTime);

		Peds[j].MinTimeBytes[0] = BytesRecvd[StartByte + 5 + (j - 1) * 13];
		Peds[j].MinTimeBytes[1] = BytesRecvd[StartByte + 4 + (j - 1) * 13];
		Peds[j].MinTime = ConvertUnsignedCharArray_To_UShort(Peds[j].MinTimeBytes, 2);

		Peds[j].MaxTimeBytes[0] = BytesRecvd[StartByte + 7 + (j - 1) * 13];
		Peds[j].MaxTimeBytes[1] = BytesRecvd[StartByte + 6 + (j - 1) * 13];
		Peds[j].MaxTime = ConvertUnsignedCharArray_To_UShort(Peds[j].MaxTimeBytes, 2);
//--		printf("Peds = %d	MinTime = %d	MaxTime = %d\n", j, Peds[j].MinTime, Phases[j].MaxTime);

		Overlaps[j].MinTimeBytes[0] = BytesRecvd[StartByte + 9 + (j - 1) * 13];
		Overlaps[j].MinTimeBytes[1] = BytesRecvd[StartByte + 8 + (j - 1) * 13];
		Overlaps[j].MaxTime = ConvertUnsignedCharArray_To_UShort(Overlaps[j].MaxTimeBytes, 2);

		Overlaps[j].MaxTimeBytes[0] = BytesRecvd[StartByte + 11 + (j - 1) * 13];
		Overlaps[j].MaxTimeBytes[1] = BytesRecvd[StartByte + 10 + (j - 1) * 13];
		Overlaps[j].MaxTime = ConvertUnsignedCharArray_To_UShort(Overlaps[j].MaxTimeBytes, 2);
//		printf("Overlaps = %d	MinTime = %d	MaxTime = %d\n", j, Overlaps[j].MinTime, Phases[j].MaxTime);
	}

 	//Process the Red status of phases 1 - 16
	unsigned char PhaseRedsGrp1Byte = BytesRecvd[211];
	unsigned char stateMask = 128;
	int tmpGroup = 1;

	//Process the Red status of phasegroup 1, i.e. phases 1 - 8
	for (i = 8; i > 0; i--)
	{
		if ((stateMask & PhaseRedsGrp1Byte) == stateMask)
		{
			Phases[i + (tmpGroup - 1)* 8].Red = true;
			Phases[i + (tmpGroup - 1)* 8].Status = Red;
		}
		else
		{
			Phases[i + (tmpGroup - 1)* 8].Red = false;
		}
		stateMask = stateMask >> 1;
	}

	unsigned char PhaseRedsGrp2Byte = BytesRecvd[210];
	stateMask = 128;
	tmpGroup = 2;
	//Process the Red status of phasegroup 2, i.e.,  phases 9 - 16
	for (i = 8; i > 0; i--)
	{
		if ((stateMask & PhaseRedsGrp2Byte) == stateMask)
		{
			Phases[i + (tmpGroup - 1)* 8].Red = true;
			Phases[i + (tmpGroup - 1)* 8].Status = Red;
		}
		else
		{
			Phases[i + (tmpGroup - 1)* 8].Red = false;
		}
		stateMask = stateMask >> 1;
	}

	//Process the Yellow status of phases 1-16
	unsigned char PhaseYellowsGrp1Byte = BytesRecvd[213];
	stateMask = 128;
	tmpGroup = 1;
	//Process the Yellow status of phasegroup 1, i.e. phases 1 - 8
	for (i = 8; i > 0; i--)
	{
		if ((stateMask & PhaseYellowsGrp1Byte) == stateMask)
		{
			Phases[i + (tmpGroup - 1)* 8].Yellow = true;
			Phases[i + (tmpGroup - 1)* 8].Status = Yellow;
//            printf("Yellow Phase: %d++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", i + (tmpGroup - 1) * 8);

		}
		else
		{
			Phases[i + (tmpGroup - 1)* 8].Yellow = false;
		}
		stateMask = stateMask >> 1;
	}

	unsigned char PhaseYellowsGrp2Byte = BytesRecvd[212];
	stateMask = 128;
	tmpGroup = 2;
	//Process the Yellow status of phasegroup 2, i.e.,  phases 9 - 16
	for (i = 8; i > 0; i--)
	{
		if ((stateMask & PhaseYellowsGrp2Byte) == stateMask)
		{
			Phases[i + (tmpGroup - 1)* 8].Yellow = true;
			Phases[i + (tmpGroup - 1)* 8].Status = Yellow;
//            printf("Yellow Phase: %d+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", i + (tmpGroup - 1) * 8);

		}
		else
		{
			Phases[i + (tmpGroup - 1)* 8].Yellow = false;
		}
		stateMask = stateMask >> 1;
	}

	//Process the Green status of phasegroup 1, i.e.,  phases 1 - 8
	unsigned char PhaseGreensGrp1Byte = BytesRecvd[215];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PhaseGreensGrp1Byte) == stateMask)
        {
            Phases[i + (tmpGroup - 1) * 8].Green = true;
            Phases[i + (tmpGroup - 1) * 8].Status = Green;
//            printf("Green Phase: %d-------------------------------------------------------------------\n", i + (tmpGroup - 1) * 8);
        }
        else
        {
            Phases[i + (tmpGroup - 1) * 8].Green = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Green status of phasegroup 2, i.e.,  phases 9 - 16
	unsigned char PhaseGreensGrp2Byte = BytesRecvd[214];
    stateMask = 128;
    tmpGroup = 2;

    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PhaseGreensGrp2Byte) == stateMask)
        {
            Phases[i + (tmpGroup - 1) * 8].Green = true;
            Phases[i + (tmpGroup - 1) * 8].Status = Green;
//            printf("Green Phase: %d----------------------------------------------------------------------------\n", i + (tmpGroup - 1) * 8);
        }
        else
        {
            Phases[i + (tmpGroup - 1) * 8].Green = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the DontWalk status of Pedgroup 1, i.e.,  Peds 1 - 8
	unsigned char PedDontWalksGrp1Byte = BytesRecvd[217];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedDontWalksGrp1Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedDontWalk = true;
            Peds[i + (tmpGroup - 1) * 8].Status = DontWalk;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedDontWalk = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the DontWalk status of Pedgroup 2, i.e.,  Peds 9 - 16
	unsigned char PedDontWalksGrp2Byte = BytesRecvd[216];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedDontWalksGrp2Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedDontWalk = true;
            Peds[i + (tmpGroup - 1) * 8].Status = DontWalk;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedDontWalk = false;
        }
        stateMask = stateMask >> 1;
    }
	
	//Process the PedClear status of Pedgroup 1, i.e.,  Peds 1 - 8
	unsigned char PedClearGrp1Byte = BytesRecvd[219];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedClearGrp1Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedClear = true;
            Peds[i + (tmpGroup - 1) * 8].Status = PedClear;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedClear = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the PedClear status of Pedgroup 2, i.e.,  Peds 9 - 16
	unsigned char PedClearGrp2Byte = BytesRecvd[218];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedClearGrp2Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedClear = true;
            Peds[i + (tmpGroup - 1) * 8].Status = PedClear;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedClear = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the PedWalk status of Pedgroup 1, i.e.,  Peds 1 - 8
	unsigned char PedWalksGrp1Byte = BytesRecvd[221];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedWalksGrp1Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedWalk = true;
            Peds[i + (tmpGroup - 1) * 8].Status = Walk;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedWalk = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the PedWalk status of Pedgroup 2, i.e.,  Peds 9 - 16
	unsigned char PedWalksGrp2Byte = BytesRecvd[220];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PedWalksGrp2Byte) == stateMask)
        {
            Peds[i + (tmpGroup - 1) * 8].PedWalk = true;
            Peds[i + (tmpGroup - 1) * 8].Status = Walk;
        }
        else
        {
            Peds[i + (tmpGroup - 1) * 8].PedWalk = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Red status of Overlapgroup 1, i.e.,  Overlaps 1 - 8
	unsigned char OvlpRedsGrp1Byte = BytesRecvd[223];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpRedsGrp1Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Red = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Red;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Red = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Red status of Overlapgroup 2, i.e.,  Overlaps 9 - 16
	unsigned char OvlpRedsGrp2Byte = BytesRecvd[220];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpRedsGrp2Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Red = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Red;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Red = false;
        }
        stateMask = stateMask >> 1;
    }
	

	//Process the Yellow status of Overlapgroup 1, i.e.,  Overlaps 1 - 8
	unsigned char OvlpYellowsGrp1Byte = BytesRecvd[225];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpYellowsGrp1Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Yellow = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Yellow;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Yellow = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Yellow status of Overlapgroup 2, i.e.,  Overlaps 9 - 16
	unsigned char OvlpYellowsGrp2Byte = BytesRecvd[224];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpYellowsGrp2Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Yellow = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Yellow;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Yellow = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Green status of Overlapgroup 1, i.e.,  Overlaps 1 - 8
	unsigned char OvlpGreensGrp1Byte = BytesRecvd[227];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpGreensGrp1Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Green = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Green;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Green = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Green status of Overlapgroup 2, i.e.,  Overlaps 9 - 16
	unsigned char OvlpGreensGrp2Byte = BytesRecvd[226];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpGreensGrp2Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Green = true;
            Overlaps[i + (tmpGroup - 1) * 8].Status = Green;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Green = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Flashing status of Phasgroup 1, i.e.,  Phases 1 - 8
	unsigned char PhaseFlashingStatusGrp1Byte = BytesRecvd[229];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PhaseFlashingStatusGrp1Byte) == stateMask)
        {
            Phases[i + (tmpGroup - 1) * 8].Flashing = true;
        }
        else
        {
            Phases[i + (tmpGroup - 1) * 8].Flashing = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Flashing status of Phasgroup 2, i.e.,  Phases 9 - 16
	unsigned char PhaseFlashingStatusGrp2Byte = BytesRecvd[228];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PhaseFlashingStatusGrp2Byte) == stateMask)
        {
            Phases[i + (tmpGroup - 1) * 8].Flashing = true;
        }
        else
        {
            Phases[i + (tmpGroup - 1) * 8].Flashing = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Flashing status of Overlapgroup 1, i.e.,  Overlaps 1 - 8
	unsigned char OvlpFlashingStatusGrp1Byte = BytesRecvd[231];
    stateMask = 128;
    tmpGroup = 1;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & OvlpFlashingStatusGrp1Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Flashing = true;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Flashing = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Flashing status of Overlapgroup 2, i.e.,  Overlaps 9 - 16
	unsigned char OvlpFlashingStatusGrp2Byte = BytesRecvd[230];
    stateMask = 128;
    tmpGroup = 2;
    for (int i = 8; i > 0; i--)
    {
        if ((stateMask & PhaseFlashingStatusGrp2Byte) == stateMask)
        {
            Overlaps[i + (tmpGroup - 1) * 8].Flashing = true;
        }
        else
        {
            Overlaps[i + (tmpGroup - 1) * 8].Flashing = false;
        }
        stateMask = stateMask >> 1;
    }

	//Process the Intersection status byte
	//enum_IntersectionStatus IntersectionStatus = normal;

	unsigned char  ManualControlMask = 1;
	unsigned char  StopTimeMask = 2;
	unsigned char  FaultFlashMask = 4;
	unsigned char  PreemptMask = 8;
	unsigned char  TSPMask = 16;
	unsigned char  CoordinationMask = 32;
	unsigned char  CoordinationinTransitionMask = 64;
	unsigned char  ProgrammedFlashMask = 128;

	type_IntersectionStatus IntersectionStatus;

	IntersectionStatus.ManualControlActive = false;
	IntersectionStatus.StopTimeActive = false;
	IntersectionStatus.FaultFlashActive = false;
	IntersectionStatus.PreemptActive = false;
	IntersectionStatus.TSPActive = false;
	IntersectionStatus.CoordinationActive = false;
	IntersectionStatus.CoordinationInTransitionActive = false;
	IntersectionStatus.ProgrammedFlashActive = false;


	actionNumber = BytesRecvd[233];
//	printf("Action Number:%d\n", actionNumber);

 	unsigned char byteIntersectionStatus = BytesRecvd[232];
	J2735IntersectionStatus = byteIntersectionStatus;

//	printf("Intersection Status Byte: %d\n", byteIntersectionStatus);
	if ((ManualControlMask & byteIntersectionStatus) == ManualControlMask)
    {
        IntersectionStatus.ManualControlActive = true;
//		printf("ManualControl Active");
    }

    if ((StopTimeMask & byteIntersectionStatus) == StopTimeMask)
    {
        IntersectionStatus.StopTimeActive = true;
//		printf("StopTime Active\n");
    }

    if ((FaultFlashMask & byteIntersectionStatus) == FaultFlashMask)
    {
        IntersectionStatus.FaultFlashActive = true;
//		printf("FaultFlash Active\n");
    }

    if ((PreemptMask & byteIntersectionStatus) == PreemptMask)
    {
        IntersectionStatus.PreemptActive = true;
//		printf("Preempt Active\n");
    }

    if ((TSPMask & byteIntersectionStatus) == TSPMask)
    {
        IntersectionStatus.TSPActive = true;
//		printf("TSP Active\n");
    }

    if ((CoordinationMask & byteIntersectionStatus) == CoordinationMask)
    {
        IntersectionStatus.CoordinationActive = true;
//		printf("Coordination Active\n");
    }

    if ((CoordinationinTransitionMask & byteIntersectionStatus) == CoordinationinTransitionMask)
    {
        IntersectionStatus.CoordinationInTransitionActive = true;
//		printf("CoordinationTransition Active\n");
    }
    if ((ProgrammedFlashMask & byteIntersectionStatus) == ProgrammedFlashMask)
    {
        IntersectionStatus.ProgrammedFlashActive = true;
//		printf("ProgrammedFlash Active\n");
    }

    if (NumBytesRcvdFromTSC >= 240)
    {
        //Process the SPaT Data Msg timestamp
        unsigned char SSM[4];
        unsigned char MSec[4];

        prevTSCTimeMsec = currTSCTimeMsec;

        int intSSM = 0;
        int intMsec = 0;

        SSM[2] = BytesRecvd[238];
        SSM[1] = BytesRecvd[237];
        SSM[0] = BytesRecvd[236];
        intSSM = ConvertUnsignedCharArray_To_Long(SSM, 3);

        MSec[1] = BytesRecvd[240];
        MSec[0] = BytesRecvd[239];
        intMsec = ConvertUnsignedCharArray_To_UShort(MSec, 2);
        currTSCTimeMsec = intSSM * 1000 + intMsec;

        int hour = 0;
        int minute = 0;
        int second = 0;

        hour = intSSM / 3600;
        minute = (intSSM - (hour * 3600)) / 60;
        second = (intSSM - (hour * 3600) - (minute * 60));
//		printf("Time SPaT Data generated by TSC: %d:%d:%d:%d\n", hour, minute, second, intMsec);
    }

    //Process the TSC Ped information


    for (int p = 1; p < maxPhases; p++)
    {
        TSCPeds[p].Call = false;
        TSCPeds[p].Detect = false;
        PedCallsMask[p] = (int)(pow(2, p-1));
        PedDetectorsMask[p] = (int)(pow(2, p-1));
    }

	if (NumBytesRcvdFromTSC >= 244)
    {
        unsigned char pedCall[2];
        unsigned char pedDetect[2];
        int intPedCall = 0;
        int intPedDetect = 0;


        pedCall[0] = BytesRecvd[244];
        pedCall[1] = BytesRecvd[243];
        intPedCall = ConvertUnsignedCharArray_To_UShort(pedCall, 2);

        pedDetect[0] = BytesRecvd[242];
        pedDetect[1] = BytesRecvd[241];
        intPedDetect = ConvertUnsignedCharArray_To_UShort(pedDetect, 2);
//		printf("\nActive Ped Calls: ");

        for (int p = 1; p < maxPhases; p++)
        {
            if ((intPedCall & PedCallsMask[p]) == PedCallsMask[p])
            {
                TSCPeds[p].Call = true;
//				printf("%d, ", p);
            }
            else
            {
                TSCPeds[p].Call = false;
            }
        }

//		printf("\nActive Ped Detectors: ");
        for (int p = 1; p < maxPhases; p++)
        {
            if ((intPedDetect & PedDetectorsMask[p]) == PedDetectorsMask[p])
            {
                TSCPeds[p].Detect = true;
//				printf("%d, ", p);
            }
            else
            {
                TSCPeds[p].Detect = false;
            }
        }
    }





	//Prepare SPAt message content
	int tmpPhsOvlpNo = 0;
	unsigned char tmpPhsOvlpStatus = 0;
	bool tmpFlashing = false;
	unsigned short int tmpMinTime = 0;
	unsigned short int tmpMaxTime = 0;
	unsigned short int tmpYellowTime = 0;
	int totalPedsDetected = 0;
	unsigned int pedCallMask = 16;
	unsigned char SecondPedDetector = 32;
	//spatMsg.spat_load();
					
	//char MvmntPedCallInfo[52];
	//char MvmntPedDetInfo[52];
	//char strMvmntPedInfo[52];
	for (int m = 0; m < NumPTLMRecords; m++)
	{
		totalPedsDetected = 0;
		pedCallMask = 16;
		SecondPedDetector = 32;


		tmpPhsOvlpStatus = 0;
		tmpFlashing = false;
		tmpMinTime = 0;
		tmpMaxTime = 0;
		tmpYellowTime = 0;
		tmpPhsOvlpNo = PTLMTable[m].TSCPhsOvlp;
                        						
		PTLMTable[m].pedestrian = pc_unavailableCount;
		PTLMTable[m].count = 0;


		if (PTLMTable[m].LanesType == pedLane)
		{
//			printf("PTLM Record %d LanesType == pedLane\n", m);
			//Determine if the movement has any ped calls active on the ped calls corresponding to the movement
			int *pedCalls;
			int NumPedCalls = 0;

			if (PTLMTable[m].PedCalls != NULL)
			{
				SplitCSVStringToIntArray(PTLMTable[m].PedCalls,(char *) ",", &NumPedCalls, (int *) &CSVElements);
				pedCalls = CSVElements;
							
				for (int p = 0; p < NumPedCalls; p++)
				{
					if (TSCPeds[*(pedCalls + p)].Call == true)
					{
						PTLMTable[m].callActive = true;
					}
				}
			}

			//Determine if any pedestrians are detected by the ped detector corresponding to the movement 
			int *pedDetectors;
			int NumPedDets = 0;

			if (PTLMTable[m].PedDetectors != NULL)
			{
				SplitCSVStringToIntArray(PTLMTable[m].PedDetectors,(char *) ",", &NumPedDets, (int *) &CSVElements);
				pedDetectors = CSVElements;
				for (int p = 0; p < NumPedCalls; p++)
				{
					if (TSCPeds[*(pedDetectors + p)].Detect == true)
					{
						printf("PTLM Record %d pedDetected\n", m);
						totalPedsDetected = totalPedsDetected + 1;
					}
				}
			}
			else
			{
				PTLMTable[m].pedestrian = pc_unavailableCount;
			}
		}

        //Adjust the mintime and maxtime remaining if the intersection is in faultflash             
		if (IntersectionStatus.FaultFlashActive == true)
		{
//			printf("PTLM Record %d FaultFlash == true\n", m);

			PTLMTable[m].yellowTime = 0;
			PTLMTable[m].yellowState = 0;
			PTLMTable[m].minTime = 12002;
			PTLMTable[m].maxTime = 12002;

			if (PTLMTable[m].LanesType == pedLane)
			{
				PTLMTable[m].state = 0x01;
				if (totalPedsDetected >= 1)
				{
					PTLMTable[m].pedestrian = pc_oneormore;
				}
			}
			else if (PTLMTable[m].LanesType == vehicleLane)
			{
				if (PTLMTable[m].Movement == man_straight)
				{
					PTLMTable[m].state = FlashingRedBall;
				}
				else if (PTLMTable[m].Movement == man_leftturn)
				{
					PTLMTable[m].state = FlashingRedLeftArrow;
				}
				else if (PTLMTable[m].Movement == man_rightturn)
				{
					PTLMTable[m].state = FlashingRedRightArrow;
				}
			}
		}
        //Adjust the mintime and maxtime remaining if the intersection is in programmedflash             
		else if (IntersectionStatus.ProgrammedFlashActive == true)
		{
//			printf("PTLM Record %d ProgrammedFlashActive == true\n", m);

			PTLMTable[m].yellowTime = 0;
			PTLMTable[m].yellowState = 0;
			PTLMTable[m].minTime = 12002;
			PTLMTable[m].maxTime = 12002;

			if (PTLMTable[m].LanesType == pedLane)
			{
				if (Peds[tmpPhsOvlpNo].Status == DontWalk)
				{
					PTLMTable[m].state = 0x01;
				}
				else if (Peds[tmpPhsOvlpNo].Status == PedClear)
				{
					PTLMTable[m].state = 0x02;
				}
				if (Peds[tmpPhsOvlpNo].Status == Walk)
				{
					PTLMTable[m].state = 0x03;
				}

				if (totalPedsDetected >= 1)
				{
					PTLMTable[m].pedestrian = pc_oneormore;
				}
			}
			else if (PTLMTable[m].LanesType == vehicleLane)
			{
//				printf("PTLM Record %d LanesType == vehicleLane\n", m);

				if (Phases[tmpPhsOvlpNo].Status == Yellow)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state = FlashingYellowBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						PTLMTable[m].state = FlashingYellowLeftArrow;
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						PTLMTable[m].state = FlashingYellowRightArrow;
					}
				}
				else if (Phases[tmpPhsOvlpNo].Status == Red)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state = FlashingRedBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						PTLMTable[m].state = FlashingRedLeftArrow;
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						PTLMTable[m].state = FlashingRedRightArrow;
					}
				}
				else
				{
					PTLMTable[m].state = DarkBall;
				}
			}
		}
		//Set the remaining time based on the lane movement type 
		else if (PTLMTable[m].LanesType == pedLane)
		{
//			printf("PTLM Record %d LanesType == pedLane\n", m);

			if ((IntersectionStatus.ManualControlActive == true) || (IntersectionStatus.StopTimeActive == true))
			{
				PTLMTable[m].minTime = 12002;
				PTLMTable[m].maxTime = 12002;
			}
			else
			{
				PTLMTable[m].minTime = Peds[tmpPhsOvlpNo].MinTime;
				PTLMTable[m].maxTime = Peds[tmpPhsOvlpNo].MaxTime;
			}
			PTLMTable[m].yellowState = 0;
			PTLMTable[m].yellowTime = 0;
			if (Peds[tmpPhsOvlpNo].Status == DontWalk)
			{
				PTLMTable[m].state = 0x01;

			}
			else if (Peds[tmpPhsOvlpNo].Status == PedClear)
			{
				PTLMTable[m].state = 0x02;
			}
			if (Peds[tmpPhsOvlpNo].Status == Walk)
			{
				PTLMTable[m].state = 0x03;
				//The ped Walk phase is similar to the vehicle Green Phase
				// and the PedClear is similar to the Yellow phase, therefore the yellow time after the Walk is PedClear
				PTLMTable[m].yellowState = 0x02;
				PTLMTable[m].yellowTime = Peds[tmpPhsOvlpNo].PedClear;
			}
			if (totalPedsDetected >= 1)
			{
				PTLMTable[m].pedestrian = pc_oneormore;
			}
		}
		else if (PTLMTable[m].LanesType == vehicleLane)
		{
//			printf("PTLM Record %d LanesType == vehicleLane\n", m);


			if (PTLMTable[m].TSCEntityType == phase)
			{
				tmpMinTime = Phases[tmpPhsOvlpNo].MinTime;
				tmpMaxTime = Phases[tmpPhsOvlpNo].MaxTime;
				tmpPhsOvlpStatus = Phases[tmpPhsOvlpNo].Status;
				tmpFlashing = Phases[tmpPhsOvlpNo].Flashing;
				tmpYellowTime = Phases[tmpPhsOvlpNo].YellowChange;
			}
			else if (PTLMTable[m].TSCEntityType == overlap)
			{
				tmpMinTime = Overlaps[tmpPhsOvlpNo].MinTime;
				tmpMaxTime = Overlaps[tmpPhsOvlpNo].MaxTime;
				tmpPhsOvlpStatus = Overlaps[tmpPhsOvlpNo].Status;
				tmpFlashing = Overlaps[tmpPhsOvlpNo].Flashing;
				tmpYellowTime = 0;
			}
			if ((IntersectionStatus.ManualControlActive == true) || (IntersectionStatus.StopTimeActive == true))
			{
				tmpMinTime = 12002;
				tmpMaxTime = 12002;
			}

			PTLMTable[m].minTime = tmpMinTime;
			PTLMTable[m].maxTime = tmpMaxTime;
			PTLMTable[m].yellowState = 0;
			PTLMTable[m].yellowTime = tmpYellowTime;
			PTLMTable[m].pedestrian = pc_unavailableCount;
			PTLMTable[m].count = 0;

			if (tmpFlashing == true)
			{
//				printf("PTLM Record %d tmpFlashing == true\n", m);

				if (tmpPhsOvlpStatus == Green)
				{
					//This should never happen happen, i.e., Flashing Green phase. Report as an error
					//Report as an error
						//dailyLogFile.WriteLine("Error: Flashing Green Phase." +
						//                       "\r\n\tPhaseNo: " + tmpPhsOvlpNo +
						//                       "\r\n\tMovement: " + GlobalVars.PTLMTable[m].Movement.ToString() +
						//                       "\r\n\tLanes: " + GlobalVars.PTLMTable[m].Lanes +
						//                       "\r\n\tPhaseType: " + GlobalVars.PTLMTable[m].PhaseType.ToString() +
						//                       "\r\n\tLanesType: " + GlobalVars.PTLMTable[m].LanesType.ToString() +
						//                       "\r\n\tMinTime: " + GlobalVars.PTLMTable[m].minTime.ToString() +
						//                       "\r\n\tMaxTime: " + GlobalVars.PTLMTable[m].maxTime.ToString());
				}
				else if (tmpPhsOvlpStatus == Yellow)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state = FlashingYellowBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						PTLMTable[m].state = FlashingYellowLeftArrow;
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						PTLMTable[m].state = FlashingYellowRightArrow;
					}
				}
				else if (tmpPhsOvlpStatus == Red)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state = FlashingRedBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						PTLMTable[m].state = FlashingRedLeftArrow;
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						PTLMTable[m].state = FlashingRedRightArrow;
					}
				}
				else
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					PTLMTable[m].state = DarkBall;
				}
			}
			else if (tmpFlashing == false)
			{
//				printf("PTLM Record %d tmpFlashing == false\n", m);


				if (tmpPhsOvlpStatus == Green)
				{
					if (PTLMTable[m].Movement == man_straight)
					{
//						printf("PTLM Record %d Setting Greenball\n", m);
						PTLMTable[m].state = GreenBall;
						PTLMTable[m].yellowState = YellowBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state = GreenLeftArrow;
							PTLMTable[m].yellowState = YellowLeftArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = GreenBall;
							PTLMTable[m].yellowState = YellowBall;
						}
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state = GreenRightArrow;
							PTLMTable[m].yellowState = YellowRightArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = GreenBall;
							PTLMTable[m].yellowState = YellowBall;
						}
					}
				}
				else if (tmpPhsOvlpStatus == Yellow)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state =YellowBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state = YellowLeftArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = YellowBall;
						}
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state =YellowRightArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = YellowBall;
						}
					}
				}
				else if (tmpPhsOvlpStatus == Red)
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					if (PTLMTable[m].Movement == man_straight)
					{
						PTLMTable[m].state = RedBall;
					}
					else if (PTLMTable[m].Movement == man_leftturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state = RedLeftArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = RedBall;
						}
					}
					else if (PTLMTable[m].Movement == man_rightturn)
					{
						if (PTLMTable[m].PhaseType == protectedPhase)
						{
							PTLMTable[m].state = RedRightArrow;
						}
						else if (PTLMTable[m].PhaseType == permittedPhase)
						{
							PTLMTable[m].state = RedBall;
						}
					}
				}
				else
				{
					PTLMTable[m].yellowTime = 0;
					PTLMTable[m].yellowState = 0;

					PTLMTable[m].state = DarkBall;
				}
			}
		}
//		printf("PTLM Record %d State == %ld\n", m, PTLMTable[m].state);
	}
}


void SPaTData::convertPtlmToSpat(spat* spat)
{
	spat_clear(spat);

	spat->intersectionid = IntersectionId;
	spat->intersectionstatus = J2735IntersectionStatus;

	uint64_t timeMs = GetMsTimeSinceEpoch();
	spat->timestampseconds = (uint32_t)(timeMs / 1000);
	spat->timestamptenths = (uint8_t)((timeMs % 1000) / 10);

	for (int m = 0; m < NumPTLMRecords; m++)
	{
//		printf("Movement:%d Type:%d\n", m,PTLMTable[m].Movement);
		spat->movement[m].data.maxtime = (unsigned int)(PTLMTable[m].maxTime );
		spat->movement[m].data.mintime = (unsigned int)(PTLMTable[m].minTime );
		spat->movement[m].data.pedestrian = PTLMTable[m].pedestrian;
		spat->movement[m].data.state = PTLMTable[m].state;
		spat->movement[m].data.yellowstate = PTLMTable[m].yellowState;
		spat->movement[m].data.yellowtime = PTLMTable[m].yellowTime;
		spat->movement[m].data.count = PTLMTable[m].count;
		spat->movement[m].type = PTLMTable[m].Movement;

		int *laneSet;
		int NumLanes = 0;

		char* comma = (char *) ",";
		SplitCSVStringToIntArray(PTLMTable[m].Lanes,comma, &NumLanes, (int *) &CSVElements);
		laneSet = CSVElements;
		for (int p = 0; p < NumLanes; p++)
		{
			spat->movement[m].lane[p] = laneSet[p];
//			printf("Lane number:%d\n", spat->movement[m].lane[p]);
		}


	}
}


