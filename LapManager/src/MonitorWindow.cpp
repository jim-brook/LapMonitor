/*
 * MonitorWindow.cpp
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */
#include <ncurses.h>
#include "MonitorWindow.h"
using namespace std;



MonitorWindow::MonitorWindow(GlobalObjects* g)
{
	gObs = g;
}

MonitorWindow::~MonitorWindow()
{
	// TODO Auto-generated destructor stub
}


void MonitorWindow::Start(void)
{
	std::list<GlobalObjects::LapDescriptor>::iterator it;
	std::list<GlobalObjects::LapDescriptor>::iterator iter;
	std::list<GlobalObjects::LapDescriptor> laps;
	std::unordered_map<int, bool> foundLaps;
	std::list<GlobalObjects::LapDescriptor> lapsForCurses;

	initscr();
	scrollok(stdscr,TRUE);

	while(gObs->isClosing == false)
	{
		if(gObs->listToMonitorRdy == false)
		{
			//std::cout << "MW SLEEP" << std::endl;
			gObs->ListToMonitorWaiter();
		}
		//std::cout << "MW AWAKE" << std::endl;
		gObs->listToMonitorRdy = false;
		if(gObs->isClosing == true)
		{
			endwin();
			return;
		}
		laps.clear();

		//std::cout << "MW LOCK" << std::endl;
		gObs->listToMonitorWaitMutex.lock();
		std::copy(gObs->monitorLaps.begin(), gObs->monitorLaps.end(), std::back_inserter(laps));
		gObs->monitorLaps.clear();
		gObs->listToMonitorWaitMutex.unlock();
		//std::cout << "MW UNLOCK" << std::endl;

		for (it = laps.begin(); it != laps.end(); ++it)
		{
			if(foundLaps.count(it->numLap) == 0)
			{
				GlobalObjects::LapDescriptor lap;
				lap.humTs = it->humTs;
				lap.numTs = it->numTs;
				lap.numSignal = it->numSignal;
				lap.humSignal = it->humSignal;
				lap.numMinSignal = it->numSignal;
				lap.numMaxSignal = it->numSignal;
				lap.humMinSignal = it->humSignal;
				lap.humMaxSignal = it->humSignal;
				lap.humChannel = it->humChannel;
				lap.numChannel = it->numChannel;
				lap.humLap = it->humLap;
				lap.numLap = it->numLap;
				lap.firstSeen = it->humTs;
				lap.lastSeen = it->humTs;
				lap.displayable = true;
				lap.count = 0;
				foundLaps[lap.numLap] = true;
				lapsForCurses.push_back(lap);
			}
			for (iter = lapsForCurses.begin(); iter != lapsForCurses.end(); ++iter)
			{
				if(it->numLap == iter->numLap)
				{
					iter->count++;
					if(iter->numMinSignal > it->numSignal)
					{
						iter->numMinSignal = it->numSignal;
						iter->humMinSignal = it->humSignal;
					}
					else if(iter->numMaxSignal < it->numSignal)
					{
						iter->numMaxSignal = it->numSignal;
						iter->humMaxSignal = it->humSignal;
					}

					if(iter->numTs < it->numTs)
					{
						iter->lastSeen = it->humTs;
					}
					iter->humTs = it->humTs;
					iter->numTs = it->numTs;
					iter->numSignal = it->numSignal;
					iter->humSignal = it->humSignal;
					iter->humChannel = it->humChannel;
					iter->numChannel = it->numChannel;
					iter->displayable = true;
					break;
				}
			}
		}
		time_t curr;
		time(&curr);
		for (iter = lapsForCurses.begin(); iter != lapsForCurses.end(); ++iter)
		{
			double diff = difftime(curr, iter->numTs);
			if(diff > static_cast<double>(gObs->purgeTime * ONE_MINUTE))
			{
				iter->displayable = false;
			}
		}
		//std::cout << "MW SEND" << std::endl;
		switch(gObs->sortMethod)
		{
			case RSSI_METHOD:
				lapsForCurses.sort([](GlobalObjects::LapDescriptor const  a, GlobalObjects::LapDescriptor const  b) {return (a.numSignal > b.numSignal);});
				break;
			case LAP_METHOD:
				lapsForCurses.sort([](GlobalObjects::LapDescriptor const  a, GlobalObjects::LapDescriptor const  b) {return (a.numLap > b.numLap);});
				break;
			case RX_TS_METHOD:
				lapsForCurses.sort([](GlobalObjects::LapDescriptor const  a, GlobalObjects::LapDescriptor const  b) {return (a.numTs < b.numTs);});
				break;

			default:
				lapsForCurses.sort([](GlobalObjects::LapDescriptor const  a, GlobalObjects::LapDescriptor const  b) {return (a.numSignal > b.numSignal);});

		}

		clear();
		int rowsDisplayed = 0;
		printw("LAP\t         Time\t        RSSI\tMin     Max     Channel  First Seen    Last Seen   Count\n" );
		for (it = lapsForCurses.begin(); it != lapsForCurses.end(); ++it)
		{
			if(!it->displayable) { continue; }
			std::string count = to_string(it->count);
			printw("%s\t%s\t%s\t%s\t%s\t%s\t%s     %s    %s\n", it->humLap.data(),it->humTs.data(), it->humSignal.data(), it->humMinSignal.data(), it->humMaxSignal.data(), it->humChannel.data(), it->firstSeen.data(), it->lastSeen.data(), count.data());
			rowsDisplayed++;
			if(rowsDisplayed >= LINES - 2)
			{
				break;
			}
		}
		refresh();
	}
}
