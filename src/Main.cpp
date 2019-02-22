//============================================================================
// Name        : Main.cpp
// Author      : MG
// Version     : 1.0
// Copyright   : Copyright (c) 2015, Real-Time System Research Group at University of Waterloo
// Description : Virtual Interconnect between MCsim (Multi-Channel DRAM System)
//								and Virtual Memory Requestors (CPU, GPU, DSP, DMA, FPGA)
//============================================================================

#include <iostream>
#include <getopt.h>
#include <map>
#include <vector>
#include <fstream>

#include "MCsim.h"
#include "Requestor.h"
#include "Global.h"

using namespace std;
using namespace MCsimulator;

std::map< int, Requestor* > requestorTable;

void usage(void)
{
	std::cout << "MCsim Usage: " << std::endl;
	std::cout << "./MCsim -n 8 -c 1 -m ../Configuration.ini -t 1 -p 0 -r ../MemTraces/a2time.trc " <<std::endl;
	std::cout << "\t-n, --Requestor=# \t\tspecify number of requestors to run the simulation for [default=8] "<<std::endl;
	std::cout << "\t-c, --Channel=# \t\tspecify number of channels "<<std::endl;
	std::cout << "\t-s, --RequestorConfig=FILENAME \tspecify multiple requestor tracefile to run  "<<std::endl;
	std::cout << "\t-m, --MemoryConfig=FILENAME \tspecify multiple config file that describes the memory system parameters  "<<std::endl;
	std::cout << "\t-t, --Cycles=# \t\tspecify number of cycles to run the simulation for [default=30] "<<std::endl;
}

bool configRequestor(int requestors, std::string requestorConfiguration)
{
	bool success = false;
	ifstream memFile;
	memFile.open(requestorConfiguration.c_str());
	std::cout<<"Requestor File = "<<requestorConfiguration<<std::endl;
	if ( memFile.is_open() )
	{
		for ( int req = 0; req < requestors; req++ )
		{
			string memTrace;
			getline(memFile, memTrace);

			if ( !memTrace.empty() )
			{
				requestorTable[req] = new Requestor(memTrace);
				success = true;
			}
			else
			{
				success = false;
				break;
			}
		}
	}
	return success;
}

int main( int argc, char **argv )
{
	int argument = 0;
	int requestors = 0;
	int channels = 0;
	std::string requestorConfiguration = "";
	std::string memoryConfiguration = "";
	int long totalCycle = 0;

	while (true)
	{
		static struct option options[] =
		{
			{"Requestors", required_argument, 0, 'n'},
			{"Channels", required_argument, 0, 'c'},
			{"ReqConfig", required_argument, 0, 'r'},
			{"MemConfig", required_argument, 0, 'm'},
			{"Cycles", required_argument, 0, 't'},
			{"Help", no_argument, 0, 'h'}
		};
		int option_index = 0;
		argument = getopt_long(argc, argv, "n:c:r:m:t:h", options, &option_index);
		if (argument == -1)
		{
			break;
		}
		switch (argument)
		{
		case 'h':
		case '?':
			usage();
			exit(0);
		case 'n':
			requestors = atoi(optarg);
			break;
		case 'c':
			channels = atoi(optarg);
			break;
		case 'r':
			requestorConfiguration = string(optarg);
			break;
		case 'm':
			memoryConfiguration = string(optarg);
			break;
		case 't':
			totalCycle = atoi(optarg);
			break;
		}
	}

	// Build Connection to MCsim System
	std::cout<<"Building MCsim System"<<std::endl;
	MCsim* mcSim = new MCsim(channels, memoryConfiguration);
	// Build Connection to Requestors
	std::cout<<"Building Requestors System"<<std::endl;
	configRequestor( requestors, requestorConfiguration );
	/*										*
	 *		Set the Requestor Criticality	*
	 *	 									*/
	for(int req = 0; req < requestors; req++)
	{
		mcSim->RegisterRequestor(req,1);
	}

	std::cout<<"\nSimulation Starts ..."<<std::endl;
	int clock = 0;
	bool simulationComplete = false;
	while ( !simulationComplete )
	{
		for (int req = 0; req < requestors; req++ )
		{
			requestorTable[req]->Tick();
			int address, size;
			bool access;
			void* data = NULL;
			if ( requestorTable[req]->GetRequest(address,access,size) )
			{
				mcSim->AddRequest(req, address, access, size, data);
			}
		}
		//PRINT("Clock: "<< clock);
		mcSim->Tick();

		for (int channel = 0; channel < channels; channel++)
		{
			int address, requestor;
			bool access;
			void* data = NULL;
			if (mcSim->ReturnRequest(channel, requestor, address, access, &data))
			{
				requestorTable[requestor]->ReturnRequest(address, access);
			}
		}
		clock++;
		if ( (totalCycle != 0) && (clock == totalCycle) )
		{
			simulationComplete = true;
		}
		else
		{
			/*
			 *  Requestor 0 is the evaluation target
			 */
			simulationComplete = requestorTable[0]->SimulationDone();
		}
	}
	std::cout<<"Simulation Ends @ "<<clock<<std::endl;
	return 0;
}

