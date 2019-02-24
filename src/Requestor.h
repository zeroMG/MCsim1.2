/*
 * Requestor.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef REQUESTOR_H_
#define REQUESTOR_H_

#include <string>
#include <vector>
#include <fstream>

class Requestor
{
public:
	Requestor(std::string memoryTrace);
	virtual ~Requestor();
	void setMemClock(float mem_clock_ns);
	bool GetRequest(int &address, bool &access, int &size);
	void ReturnRequest(int address, bool access);
	bool SimulationDone();
	void Tick();
private:
	int long clock;
	std::ifstream memTrace;
	struct Access
	{
		int address;
		bool access;
		int size;
		void* data;
	};
	Access* pendingAccess;
	std::vector<Access*> pendingQueue;
	bool accessBlock;
	unsigned requestCycleCountdown;
	float memClock;
	unsigned totalCompTime;  // cycles between requests
	
};

#endif /* REQUESTOR_H_ */
