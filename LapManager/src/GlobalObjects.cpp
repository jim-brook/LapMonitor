/*
 * GlobalObjects.cpp
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#include "GlobalObjects.h"
#include <signal.h>
using namespace std;
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
//c2:1a:a1
int sortMethod;
int purgeTime;
std::list<LapDescriptor> monitorLaps;

std::mutex descriptorMutex;
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

std::string fifoLines[MAX_LAP_DESCRIPTORS];
int fifoLinesPtr;
bool fifoToLineRdy;
bool lineToListRdy;
bool listToMonitorRdy;
bool isSuccessful;

GlobalObjects::GlobalObjects()
{
	isSuccessful = InitObjects();

}
GlobalObjects::~GlobalObjects()
{
	void* mem;
	int counter = MAX_LAP_DESCRIPTORS - 1;
	while(counter >= 0)
	{
		mem = allLapDescriptors[counter--];
		free(mem);
	}

}


bool GlobalObjects::InitObjects(void)
{
	int counter = 0;
	void* allocatedLapDescriptor;


	for(counter = 0 ; counter < MAX_LAP_DESCRIPTORS ; counter++)
	{
		allocatedLapDescriptor = malloc(sizeof(LapDescriptor));
		//cout << "Create desc" << endl;
		 allLapDescriptors[availLapDescriptorsPtr] = allocatedLapDescriptor;
		 availLapDescriptors[availLapDescriptorsPtr++] = allocatedLapDescriptor;
	}
	purgeTime = DEFAULT_MIN_TO_PURGE;
	lineToListDescriptorsPtr = 0;
	listToMonitorDescriptorsPtr = 0;
	fifoLinesPtr = 0;
	isClosing = false;
	fifoToLineRdy = false;
	lineToListRdy = false;
	listToMonitorRdy = false;
	return(true);
}

GlobalObjects::LapDescriptor* GlobalObjects::GetLapDescriptor(void)
{
	void* desc = nullptr;
	//cout << "Get desc" << endl;
	//cout << "GD " << availLapDescriptorsPtr << endl;
	descriptorMutex.lock();
	desc = availLapDescriptors[--availLapDescriptorsPtr];
	descriptorMutex.unlock();
	return(static_cast<GlobalObjects::LapDescriptor*>(desc));
}
void GlobalObjects::RetrunLapDescriptor(LapDescriptor* desc)
{
	//cout << "Return desc" << endl;
	descriptorMutex.lock();
	//cout << "RD " << availLapDescriptorsPtr << endl;
	availLapDescriptors[availLapDescriptorsPtr++] = static_cast<void*>(desc);
	descriptorMutex.unlock();

#ifdef TEST_SHUTDOWN

	if(globalDescCounter >= 1000)
	{
		isClosing = true;
		fifoToLineRdy = true;
		lineToListRdy = true;
		listToMonitorRdy = true;
		fifoToLineWait.notify_all();
		lineToListWait.notify_all();
		listToMonitorWait.notify_all();
		return;
	}
#endif
}
bool GlobalObjects::FifoToLineIsReady(void)
{
  return (fifoToLineRdy);
}

void GlobalObjects::FifoToLineWaiter(void)
{
	std::unique_lock<std::mutex> mlock(fifoToLineWaitMutex);
	fifoToLineWait.wait(mlock, std::bind(&GlobalObjects::FifoToLineIsReady, this));
}
bool GlobalObjects::LineToListIsReady(void)
{
  return (lineToListRdy);
}

void GlobalObjects::LineToListWaiter(void)
{
	std::unique_lock<std::mutex> mlock(lineToListWaitMutex);
	lineToListWait.wait(mlock, std::bind(&GlobalObjects::LineToListIsReady, this));
}
bool GlobalObjects::ListToMonitorIsReady(void)
{
  return (listToMonitorRdy);
}

void GlobalObjects::ListToMonitorWaiter(void)
{
	std::unique_lock<std::mutex> mlock(listToMonitorWaitMutex);
	listToMonitorWait.wait(mlock, std::bind(&GlobalObjects::ListToMonitorIsReady, this));
}

