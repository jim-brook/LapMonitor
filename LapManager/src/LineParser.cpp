/*
 * LineParser.cpp
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#include "LineParser.h"
using namespace std;
using std::chrono::system_clock;

LineParser::LineParser(GlobalObjects* g)
{
	gObs = g;
}

LineParser::~LineParser() {
	// TODO Auto-generated destructor stub
}

void LineParser::Start(void)
{
	std::string line;
	std::string delim = ":";
	GlobalObjects::LapDescriptor* lapDesc;
	std::string cpy[MAX_LAP_DESCRIPTORS];
	int cpyPtr = 0;
	std::vector<size_t> startPositions;
	std::vector<size_t> endPositions;
	GlobalObjects::LapDescriptor* processedDescs[MAX_LAP_DESCRIPTORS];
	int processedDescsPtr = 0;
	std::size_t startPosition = 0;
	std::size_t endPosition = 0;
	std::string startMarker = "=";
	std::string endMarker = " ";

	while(gObs->isClosing == false)
	{
		if(gObs->fifoToLineRdy == false)
		{
			//std::cout << "LP SLEEP" << std::endl;
			gObs->FifoToLineWaiter();
		}
		//std::cout << "LP AWAKE" << std::endl;
		gObs->fifoToLineRdy = false;
		if(gObs->isClosing == true)
		{
			return;
		}

		//std::cout << "LP LOCK" << std::endl;
		gObs->fifoToLineWaitMutex.lock();

		std::copy(gObs->fifoLines, gObs->fifoLines + gObs->fifoLinesPtr, cpy);
		cpyPtr = gObs->fifoLinesPtr;
		gObs->fifoLinesPtr = 0;

		gObs->fifoToLineWaitMutex.unlock();

		//std::cout << "LP UNLOCK" << std::endl;
		for(int counter = 0 ; counter < cpyPtr ; counter++)
		{
			line = cpy[counter];
			lapDesc = gObs->GetLapDescriptor();
			if(lapDesc == nullptr)
			{
				cout << "Massive Fail Happening Now!" << endl;
				break;
			}

			size_t pos = line.find(startMarker, 0);
			while(pos != string::npos)
			{
				startPositions.push_back(pos);
			    pos = line.find(startMarker, pos+1);
			}
			pos = line.find(endMarker, 0);
			while(pos != string::npos)
			{
				endPositions.push_back(pos);
			    pos = line.find(endMarker,pos+1);
			}
			startPositions.pop_back();
			startPositions.pop_back();
			endPositions.pop_back();
			//endPositions.pop_back();
			startPosition = startPositions.back();
			endPosition = endPositions.back();
			startPosition++;
			std::string signal = line.substr(startPosition, endPosition - startPosition);

			startPositions.pop_back();
			startPositions.pop_back();
			startPositions.pop_back();
			startPositions.pop_back();
			endPositions.pop_back();
			endPositions.pop_back();
			endPositions.pop_back();
			endPositions.pop_back();
			startPosition = startPositions.back();
			endPosition = endPositions.back();
			startPosition++;
			std::string lap = line.substr(startPosition, endPosition - startPosition);

			startPositions.pop_back();
			endPositions.pop_back();
			startPosition = startPositions.back();
			endPosition = endPositions.back();
			startPosition++;
			std::string channel = line.substr(startPosition,  endPosition - startPosition);

			if(channel.find(" ") != string::npos)
			{
				endPositions.pop_back();
			}

			startPositions.pop_back();
			endPositions.pop_back();
			startPosition = startPositions.back();
			endPosition = endPositions.back();
			startPosition++;
			std::string time = line.substr(startPosition, endPosition - startPosition);

			startPositions.clear();
			endPositions.clear();

			lapDesc->humSignal = signal;
			lapDesc->numSignal = std::stoi(signal, nullptr, BASE_10);

			lapDesc->humChannel = channel;
			lapDesc->numChannel = std::stoi(channel, nullptr, BASE_10);

			lapDesc->numLap = std::stoi(lap, nullptr, BASE_16);
			pos = 2;
			lap.insert(pos,delim);
			pos += 3;
			lap.insert(pos,delim);
			lapDesc->humLap = lap;

			lapDesc->numTs = std::stol (time, nullptr, BASE_10);
			std::time_t result = lapDesc->numTs;
			lapDesc->humTs = std::asctime(std::localtime(&result));
			lapDesc->humTs.erase(std::remove(lapDesc->humTs.begin(), lapDesc->humTs.end(), '\n'), lapDesc->humTs.end());
			lapDesc->humTs.erase (0, 10);
			lapDesc->humTs.erase (9, 13);
			//std::cout<< "LP: " << lapDesc->humTs << "\t" << lapDesc->humLap << "\t" << lapDesc->humSignal << "\t" << lapDesc->humChannel << std::endl;


			if(gObs->lineToListDescriptorsPtr >= MAX_LAP_DESCRIPTORS) {continue;} //overflow!
			processedDescs[processedDescsPtr++] = lapDesc;
    		//cout << "LP->LM" << endl;
		}
        gObs->lineToListWaitMutex.lock();
        for(int counter = 0 ; counter < processedDescsPtr ; counter++)
        {
        	gObs->lineToListDescriptors[gObs->lineToListDescriptorsPtr++] = processedDescs[counter];
        }
        gObs->lineToListWaitMutex.unlock();
		gObs->lineToListRdy = true;
		gObs->lineToListWait.notify_one();
		//std::cout << "LP SEND" << std::endl;
		cpyPtr = 0;
		processedDescsPtr = 0;
	}
}
