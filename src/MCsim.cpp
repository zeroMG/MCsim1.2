//============================================================================
// Name        : MCsim.cpp
// Author      : MG
// Version     : 1.0
// Copyright   : Copyright (c) 2015, Real-Time System Research Group at University of Waterloo
// Description : Multi-Channel DRAM Controller (MCsim) Interface, responsible for
//						1. Requestor Registration (priority)
//						2. Build the Memory Subsystem
//						3. Receive/Return Memory Request from/to Requestors
//============================================================================

#include <map>
#include <vector>
#include <string>
#include <fstream>

#include "MCsim.h"
#include "Global.h"
#include "Request.h"
#include "MemoryController.h"
#include "MemoryDevice.h"
#include "SystemFactory.h"

namespace MCsimulator
{
SystemFactory *SystemFactory::factory = NULL;
// Global Declare Tables
std::map< int, int[TOTAL_LEVEL] > memoryLevelTable;	 // <channel, memoryLevelArray>
std::map< int, int[TOTAL_TIMING] > memoryTimingTable; // <channel, timingConstraintArray>
std::map< int, std::vector< int > >  bankOpenRowTable; // <channel, bankVector>
std::map< int, int > requestorPriorityTable;

// MCsim Local Tables
// Memory Controller to interconnect channel interface
std::map<int, MemoryController*> memoryControllerTable;
// Memory System Table
std::map<int, MemoryDevice*> memoryDeviceTable;
// Channel Address Space Table
std::map< int, unsigned long> channelMemorySpaceTable;

MCsim::MCsim(unsigned channels, std::string controllerMapping)
	: clock(0)
{
	// Read the controller<->memory connection
	std::ifstream systemConnection;
	std::string connection;
	systemConnection.open(controllerMapping.c_str());
	if (systemConnection.is_open())
	{
		for( unsigned channel = 0; channel < channels; channel++ )
		{
			if( !systemConnection.eof() )
			{
				while (true)
				{
					getline(systemConnection, connection);
					if (connection.size() != 0)
					{
						// ";" is used to comment line out
						// the actual configuration line doesn't contain ";" characters
						if ( connection.find_first_of(";") == std::string::npos )
						{
							PRINT(connection);
							// Read controller and system string name
							BuildDRAMSystem(channel, connection);
							break;
						}
					}
				}
			}
			else
			{
				ERROR("The configuration information is not sufficient for channels construction!!! \n");
				abort();
			}

			// Construct multi-channel memory mapping
			unsigned int previousMemorySpace = 0;
			if (channel > 0)
			{
				previousMemorySpace = channelMemorySpaceTable[channel-1];
			}
			channelMemorySpaceTable[channel] = previousMemorySpace + memoryDeviceTable[channel]->GetDeviceSize()*1024*1024;
			PRINT("- Channel"<<channel<<": Building System Successfully "<<channelMemorySpaceTable[channel]);
		}
	}
}

MCsim::~MCsim() {
	// TODO Auto-generated destructor stub
}

void MCsim::RegisterRequestor(int requestorID, int priority)
{
	requestorPriorityTable[requestorID] = priority;
}

// access: 1=RD, 0=WR
void MCsim::AddRequest(int requestorID, int address, bool access, int size, void* data)
{
	Request *newRequest = new Request(requestorID, address, access, size, data);
	bool requestAdded = false;
	// Route to corresponding channel memory controller
	if ( !channelMemorySpaceTable.empty() )
	{
		for ( std::map<int, unsigned long>::iterator memorySize = channelMemorySpaceTable.begin(); memorySize != channelMemorySpaceTable.end(); memorySize++ )
		{
			int long channelMemorySpace = memorySize->second;
			int channel = memorySize->first;
			if (address < channelMemorySpace)
			{
				newRequest->physicalAddress[CHANNEL] = channel;
				memoryControllerTable[channel]->AddRequest(newRequest);
				//PRINT("++ MCsim: AddRequest "<<requestorID<<" "<<address<<" "<<access<<" "<<size);
				requestAdded = true;
			}
		}
	}
	if ( !requestAdded )
	{
		ERROR( "Requested Memory Address Exceeds the Maximum Memory Space");
		abort();
	}
}

bool MCsim::ReturnRequest(int channel, int& requestor, int& address, bool& access, void** data)
{
	bool dataReturn = false;
	Request* returnData = memoryControllerTable[channel]->ReturnRequest();
	if (returnData != NULL)
	{
		requestor = returnData->requestorID;
		address = returnData->address;
		access = returnData->type;
		dataReturn = true;
		delete returnData;
	}
	return dataReturn;
}

void MCsim::Tick( void )
{
	for (std::map<int, MemoryController*>::iterator it = memoryControllerTable.begin();
			it != memoryControllerTable.end(); it++)
	{
		it->second->Tick();
	}
	clock++;
}

void MCsim::BuildDRAMSystem(int channel, std::string systemConnection)
{
	size_t commentIndex = systemConnection.find_first_of("=");
	std::string memoryControllerConfig, memoryDeviceConfig;

	if (commentIndex != std::string::npos)
	{
		// Memory Controller is configured with a file name
		memoryControllerConfig = systemConnection.substr(0, commentIndex);
		// Memory Device is configured with a string, decoded here
		memoryDeviceConfig = systemConnection.substr(commentIndex+1, systemConnection.size() - commentIndex);

		// Construct the DRAM device
		if ( memoryDeviceConfig.size() != 0 )
		{
			std::string typeKey;
			int rank;
			std::string deviceGeneration;
			std::string deviceSpeed;
			std::string deviceOrganization;
			DecodeDRAMDevice(memoryDeviceConfig, typeKey, rank, deviceGeneration, deviceSpeed, deviceOrganization);
			PRINT("- Channel"<<channel<<": Building DRAM Device "<<typeKey);
			memoryDeviceTable[channel] = SystemFactory::GetInstance()->GetMemoryDevice(channel, typeKey, rank, deviceGeneration, deviceSpeed, deviceOrganization);
			memoryDeviceTable[channel]->SetChannel(channel);
		}
		else
		{
			ERROR( " No Memory Device Defined For Channel"<<channel);
		}

		// Construct the channel Memory Controller
		if ( memoryControllerConfig.size() != 0 )
		{
			PRINT("- Channel"<<channel<<": Building DRAM Controller "<<memoryControllerConfig);
			memoryControllerTable[channel] = new MemoryController(channel, memoryControllerConfig, memoryDeviceTable[channel]);
		}
		else
		{
			ERROR( " No Memory Controller Defined For Channel"<<channel);
		}
	}
	else
	{
		ERROR(" Missing Memory System Configuration For Channel"<<channel);
		abort();
	}
}

void MCsim::DecodeDRAMDevice(std::string memoryDeviceConfig, std::string& typeKey,
		int& rank, std::string& deviceGeneration, std::string& deviceSpeed, std::string& deviceOrganization)
{
	std::string deviceInfo = memoryDeviceConfig;

	size_t tmpIndex;
	std::string tmpString;
	int indexCounter = MCsimulator::DRAM_Device::TYPE_KEY;
	while (true)
	{
		tmpIndex = deviceInfo.find_first_of("_");
		tmpString = deviceInfo.substr(0, tmpIndex);
		switch( indexCounter )
		{
		case MCsimulator::DRAM_Device::TYPE_KEY:
			typeKey = tmpString;
			break;
		case MCsimulator::DRAM_Device::RANK_NUMBER:
			rank = std::stoul (tmpString,nullptr,0);
			break;
		case MCsimulator::DRAM_Device::DEVICE_GENERATION:
			deviceGeneration = tmpString;
			break;
		case MCsimulator::DRAM_Device::DEVICE_SPEED:
			deviceSpeed = tmpString;
			break;
		case MCsimulator::DRAM_Device::DEVICE_SIZE:
			deviceOrganization = tmpString;
			break;
		case MCsimulator::DRAM_Device::DEVICE_CHIP:
			deviceOrganization = deviceOrganization + "_" + tmpString;
			break;
		default:
			break;
		}
		if (tmpIndex != deviceInfo.size())
		{
			deviceInfo = deviceInfo.substr(tmpIndex+1, deviceInfo.size()-1);
		}
		if (deviceInfo.empty())
		{
			break;
		}
		indexCounter++;
		tmpString = "";
		tmpIndex = 0;
	}
}

} /* namespace MCsimulator */
