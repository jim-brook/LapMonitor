//============================================================================
// Name        : LapManager.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <boost/program_options.hpp>
#include "GlobalObjects.h"
#include "LineParser.h"
#include "FifoReader.h"
#include "ListManager.h"
#include "MonitorWindow.h"
#include <signal.h>
#include <thread>
#include <mutex>
using namespace std;

namespace po = boost::program_options;

GlobalObjects* go;


void TermHandler(int sig)
{
	go->isClosing = true;

	go->fifoToLineRdy = true;
	go->lineToListRdy = true;
	go->listToMonitorRdy = true;

	go->fifoToLineWait.notify_one();
	go->lineToListWait.notify_one();
	go->listToMonitorWait.notify_one();

}

int main(int ac, char* av[])
{
	char * myfifo = new char [12];
	strcpy(myfifo, "/tmp/ubt");
	mkfifo(myfifo, 0666);
	std::string opts;
	int sortOption = RSSI_METHOD;
	int pTime = DEFAULT_MIN_TO_PURGE;

	try
	{

		po::options_description desc("Allowed options");
		desc.add_options()
			("sort", po::value<string>(), "set sort: rssi, lap, rxts")
			("purge", po::value<int>(), "n minutes to purge list");

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);


		if (vm.count("sort"))
		{
			opts = vm["sort"].as<string>();
			if(opts == "rssi")
			{
				sortOption =  RSSI_METHOD;
			}
			else if(opts == "lap")
			{
				sortOption = LAP_METHOD;
			}
			else if(opts == "rxts")
			{
				sortOption = RX_TS_METHOD;
			}
		}
		else
		{
			cout << "Garbage! Bad command line.\n";
			return(-1);
		}
		if (vm.count("purge"))
		{
			pTime = vm["purge"].as<int>();
		}
		else
		{
			cout << "Garbage! Bad command line.\n";
			return(-1);
		}
	}
	catch(exception& e)
	{

		cout << "Garbage! Bad command line.\n";
		return(-1);

	}
	go = new GlobalObjects();
	go->purgeTime = pTime;
	go->sortMethod = sortOption;
	signal(SIGQUIT, TermHandler);
	signal(SIGINT,TermHandler);

	LineParser* lp = new LineParser(go);
	FifoReader* fr = new FifoReader(go);
	ListManager* lm = new ListManager(go);
	MonitorWindow* mw = new MonitorWindow(go);



	std::thread monitorWindowT(&MonitorWindow::Start, mw);
	std::thread listManagerT(&ListManager::Start, lm);
	std::thread lineParserT(&LineParser::Start, lp);
	std::thread fifoReaderT(&FifoReader::Start, fr);


	monitorWindowT.join();
	listManagerT.join();
	lineParserT.join();
	fifoReaderT.join();

	std::cout << "\nBye!\n" << std::endl;
	go->~GlobalObjects();
	return(0);
}
