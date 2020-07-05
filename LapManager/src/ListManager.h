/*
 * ListManager.h
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#ifndef LISTMANAGER_H_
#define LISTMANAGER_H_
#include "GlobalObjects.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>

class ListManager
{
public:
	ListManager(GlobalObjects* g);
	virtual ~ListManager();
	void Start(void);

private:
	GlobalObjects* gObs;
};

#endif /* LISTMANAGER_H_ */
