/*
 * MonitorWindow.h
 *
 *  Created on: Jun 27, 2020
 *      Author: unknown
 */

#ifndef MONITORWINDOW_H_
#define MONITORWINDOW_H_
#include <list>
#include <algorithm>
#include <functional>
#include <array>
#include <iostream>
#include <unordered_map>
#include "GlobalObjects.h"

class MonitorWindow
{
public:
	MonitorWindow(GlobalObjects* g);
	virtual ~MonitorWindow();
	void Start(void);
private:
	GlobalObjects* gObs;


};

#endif /* MONITORWINDOW_H_ */
