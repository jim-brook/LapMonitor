/*
 * LineParser.h
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#ifndef LINEPARSER_H_
#define LINEPARSER_H_
#include "GlobalObjects.h"

#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <algorithm>

#define BASE_10 10
#define BASE_16 16

class LineParser
{
public:
	LineParser(GlobalObjects* g);
	virtual ~LineParser();
	void Start(void);

private:
	GlobalObjects* gObs;
};

#endif /* LINEPARSER_H_ */
