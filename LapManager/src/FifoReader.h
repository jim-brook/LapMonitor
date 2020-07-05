/*
 * FifoReader.h
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#ifndef FIFOREADER_H_
#define FIFOREADER_H_
#include "GlobalObjects.h"
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>


class FifoReader
{
public:
	FifoReader(GlobalObjects* g);
	virtual ~FifoReader();
	void Start(void);

private:
	GlobalObjects* gObs;
};

#endif /* FIFOREADER_H_ */
