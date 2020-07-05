/*
 * FifoReader.cpp
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#include "FifoReader.h"
using namespace std;


FifoReader::FifoReader(GlobalObjects* g)
{
	gObs = g;

}

FifoReader::~FifoReader()
{
	// TODO Auto-generated destructor stub
}
void FifoReader::Start(void)
{

	std::ifstream fifo;
	std::string line;
	fifo.open("/tmp/ubt", ifstream::in);

	if(! fifo.is_open() )
	{
		std::cout << " error : cannot open fifo " << std :: endl;
	    return;
	}
	while(gObs->isClosing == false)
	{
        while (std::getline(fifo, line))
        {

    		if(gObs->isClosing == true)
    		{
    			return;
    		}

    		if(gObs->fifoLinesPtr >= MAX_UBT_LINES) { continue; } //overflow!
            gObs->fifoToLineWaitMutex.lock();
            gObs->fifoLines[gObs->fifoLinesPtr++] = line;
            gObs->fifoToLineWaitMutex.unlock();
    		gObs->fifoToLineRdy = true;
    		gObs->fifoToLineWait.notify_one();
    		//std::cout << "FR SEND" << std::endl;


        }
        if (fifo.eof())
        {
            fifo.clear();  // Clear the EOF bit to enable further reading
        }
	}
	fifo.close();

}
