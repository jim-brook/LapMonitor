/*
 * ListManager.cpp
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#include "ListManager.h"

using namespace std;

ListManager::ListManager(GlobalObjects* g)
{
	gObs = g;

}
ListManager::~ListManager()
{
	// TODO Auto-generated destructor stub
}

void ListManager::Start(void)
{

	GlobalObjects::LapDescriptor* cpy[MAX_LAP_DESCRIPTORS];
	GlobalObjects::LapDescriptor* descp;
	GlobalObjects::LapDescriptor rxLap;
	std::list <GlobalObjects::LapDescriptor> rxLapList;
	int cpyPtr = 0;


	while(gObs->isClosing == false)
	{
		if(gObs->lineToListRdy == false)
		{
			//std::cout << "LM SLEEP" << std::endl;
			gObs->LineToListWaiter();
		}
		//std::cout << "LM AWAKE" << std::endl;
		gObs->lineToListRdy = false;
		if(gObs->isClosing == true)
		{
			return;
		}
		//std::cout << "LM LOCK" << std::endl;
		gObs->lineToListWaitMutex.lock();
		std::copy(gObs->lineToListDescriptors, gObs->lineToListDescriptors + gObs->lineToListDescriptorsPtr, cpy);
		cpyPtr = gObs->lineToListDescriptorsPtr;
		gObs->lineToListDescriptorsPtr = 0;
		gObs->lineToListWaitMutex.unlock();
		//std::cout << "LM UNLOCK" << std::endl;
		for(int counter = 0 ; counter < cpyPtr ; counter++)
		{
			descp = cpy[counter];
			rxLap.humChannel = descp->humChannel;
			rxLap.humSignal = descp->humSignal;
			rxLap.humLap = descp->humLap;
			rxLap.humTs = descp->humTs;
			rxLap.numChannel = descp->numChannel;
			rxLap.numSignal = descp->numSignal;
			rxLap.numLap = descp->numLap;
			rxLap.numTs = descp->numTs;
			gObs->RetrunLapDescriptor(descp);
			rxLapList.push_back(rxLap);

		}
		gObs->listToMonitorWaitMutex.lock();
		gObs->monitorLaps.splice(gObs->monitorLaps.end(), rxLapList);
		gObs->listToMonitorWaitMutex.unlock();
		gObs->listToMonitorRdy = true;
		gObs->listToMonitorWait.notify_one();
		//std::cout << "LM SEND" << std::endl;
		rxLapList.clear();
	}
}
