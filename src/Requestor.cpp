//============================================================================
// Name        : Requestor.cpp
// Author      : MG
// Version     : 1.0
// Copyright   : Copyright (c) 2015, Real-Time System Research Group at University of Waterloo
// Description : Virtual memory requestor (memory traces based engine)
//						Limitation: In-Order memory execution model
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>

#include "Requestor.h"

Requestor::Requestor(std::string memoryTrace)
{
	// TODO Auto-generated constructor stub
	if ( !memoryTrace.empty() )
	{
		memTrace.open(memoryTrace.c_str());
		if (memTrace.is_open())
		{
			std::cout<<"requestorTrace = "<<memoryTrace<<std::endl;
		}
		else
		{
			std::cout<<"Cannot Open requestorTrace "<<memoryTrace<<std::endl;
			exit(1);
		}
	}
	pendingAccess = NULL;
	clock = 0;
	accessBlock = false;
	requestCycleCountdown = 0;
	memClock = 1;  // 1ns clock speed for this simple requestor memory traces
}

Requestor::~Requestor()
{
	// TODO Auto-generated destructor stub
	while ( !pendingQueue.empty() )
	{
		Access* tmpAccess = pendingQueue.back();
		pendingQueue.pop_back();
		delete tmpAccess;
	}
	delete pendingAccess;
}

void Requestor::setMemClock(float mem_clock_ns)
{
	if (mem_clock_ns <= 1)
	{
		std::cout<<"Invalid memory device clock speed"<<std::endl;
	}
	else
	{
		memClock = mem_clock_ns;
	}
}

bool Requestor::SimulationDone()
{
	bool simDone = false;
	if ( memTrace.eof() )
	{
		// std::cout << totalCompTime << std::endl;
		simDone = true;
	}
	return simDone;
}

void Requestor::Tick(void)
{
	if (!accessBlock && pendingAccess == NULL)
	{
		if ( memTrace.is_open() )
		{
			std::string request;
			getline(memTrace, request);
			// std::cout<<request<<std::endl;
			if ( !request.empty() )
			{
				pendingAccess = new Access();
				std::stringstream ss;
				int index;
				std::string key;
				index = request.find_first_of(" ");
				key = request.substr(0, index);
				ss << std::hex << key;
				ss >> pendingAccess->address;
				request = request.substr(index+1, request.size()-index);
				index = request.find_first_of(" ");
				key = request.substr(0, index);
				if (key == "READ")
				{
					pendingAccess->access = true;
				}
				else
				{
					pendingAccess->access = false;
				}
				request = request.substr(index+1, request.size()-index);
				// std::cout << request << std::endl;
				requestCycleCountdown = std::strtoul(request.c_str(), nullptr, 0);
				requestCycleCountdown = requestCycleCountdown / memClock + 1;
				// std::cout << requestCycleCountdown << std::endl;
				totalCompTime+=requestCycleCountdown;
			}
		}
	}
	clock++;
}

bool Requestor::GetRequest(int &address, bool &access, int &size)
{
	if (pendingAccess != NULL && !accessBlock)
	{
		if (requestCycleCountdown > 0)
		{
			requestCycleCountdown--;
		}
		if (requestCycleCountdown == 0)
		{
			address = pendingAccess->address;
			access = pendingAccess->access;
			size = 64;//pendingAccess->size;
	//		data = pendingAccess->data;
	//		pendingQueue.push_back(pendingAccess);
	//		pendingAccess = NULL;
			accessBlock = true;
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

void Requestor::ReturnRequest(int address, bool access)
{
	if (pendingAccess != NULL)
	{
		pendingAccess = NULL;
		accessBlock = false;
	}
	else
	{
		std::cout << "Invalid Return Data" << std::endl;
		abort();
	}
}
