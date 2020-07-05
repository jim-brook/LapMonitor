/*
 * GlobalObjects.h
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 *      sudo apt-get install libncurses5-dev libncursesw5-dev
 *      sudo apt install libboost-program-options-dev
 */

#ifndef GLOBALOBJECTS_H_
#define GLOBALOBJECTS_H_
#include <stdio.h>
#include <iostream>
#include <condition_variable>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <functional>
#include <list>
#include <string>
#include <vector>

#define MAX_LAP_DESCRIPTORS	1000
#define MAX_UBT_LINES 5000	//If downstream threads fail, the fifo reader will keep producing and cause an overflow
#define DEFAULT_MIN_TO_PURGE 60
#define ONE_MINUTE 60
#define RSSI_METHOD 1
#define RX_TS_METHOD 2
#define LAP_METHOD 3
class GlobalObjects
{
public:
	GlobalObjects();
	virtual ~GlobalObjects();
	struct LapDescriptor
	{
		std::string humLap;
		int numLap;
		std::string humTs;
		long numTs;
		std::string humSignal;
		int numSignal;
		std::string humChannel;
		int numChannel;
		long count;
		int numMinSignal;
		std::string humMinSignal;
		int numMaxSignal;
		std::string humMaxSignal;
		std::string firstSeen;
		std::string lastSeen;
		bool displayable;
	};
	int sortMethod;
	int purgeTime;
	std::list<LapDescriptor> monitorLaps;
	bool FifoToLineIsReady(void);
	void FifoToLineWaiter(void);
	bool LineToListIsReady(void);
	void LineToListWaiter(void);
	bool ListToMonitorIsReady(void);
	void ListToMonitorWaiter(void);
	LapDescriptor* GetLapDescriptor(void);
	void RetrunLapDescriptor(LapDescriptor* desc);

	int str;
	bool fifoToLineRdy;
	bool lineToListRdy;
	bool listToMonitorRdy;
	std::mutex fifoToLineWaitMutex;
	std::mutex lineToListWaitMutex;
	std::mutex listToSoundWaitMutex;
	std::mutex listToMonitorWaitMutex;
	std::condition_variable fifoToLineWait;
	std::condition_variable lineToListWait;
	std::condition_variable listToSoundWait;
	std::condition_variable listToMonitorWait;

	void* allLapDescriptors[MAX_LAP_DESCRIPTORS];
	void* availLapDescriptors[MAX_LAP_DESCRIPTORS];
	int availLapDescriptorsPtr;

	LapDescriptor* lineToListDescriptors[MAX_LAP_DESCRIPTORS];
	int lineToListDescriptorsPtr;

	LapDescriptor* listToMonitorDescriptors[MAX_LAP_DESCRIPTORS];
	int listToMonitorDescriptorsPtr;

	std::string fifoLines[MAX_UBT_LINES];
	int fifoLinesPtr;
	bool isClosing;
	bool isSuccessful;



private:
	std::mutex descriptorMutex;
	bool InitObjects(void);

};
#endif /* GLOBALOBJECTS_H_ */
