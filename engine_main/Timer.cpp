#include "Timer.hpp"

Timer::Timer() {
	QueryPerformanceFrequency(&freq);
	isRunning = false;
}

void Timer::start() {
	isRunning = true;
	QueryPerformanceCounter(&startVal);
}

void Timer::stop() {
	isRunning = false;
}

void Timer::update() {
	if(isRunning) QueryPerformanceCounter(&curVal);
}

unsigned long int Timer::getMilliSeconds() {
	return (unsigned long)(curVal.QuadPart - startVal.QuadPart) / (freq.QuadPart / 1000);
}


