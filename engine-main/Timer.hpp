#pragma once

#ifndef _INC_TIMER_
#define _INC_TIMER_

#include <Windows.h>

class Timer {
	LARGE_INTEGER freq;
	LARGE_INTEGER startVal;
	LARGE_INTEGER curVal;

	bool isRunning;
public:
	Timer();
	void start();
	void stop();
	void update();
	unsigned long int getMilliSeconds();
};

#endif