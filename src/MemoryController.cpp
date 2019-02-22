/*
 * MemoryController.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#include <fstream>
#include <string>

#include "MemoryController.h"
#include "SystemFactory.h"

namespace MCsimulator
{

MemoryController::MemoryController(int channel, std::string configFile, MemoryDevice* memoryDevice):
	channel(channel),
	memoryDevice(memoryDevice)
{
	// TODO Auto-generated constructor stub
	std::string addressMappingKey;
	std::string requestQueueKey;
	std::string requestSchedulerKey;
	std::string commandGeneratorKey;
	std::string commandQueueKey;
	std::string commandSchedulerKey;
	// Decoding
	AssignConfigurationKey(configFile, addressMappingKey, requestQueueKey, requestSchedulerKey,
			commandGeneratorKey, commandQueueKey, commandSchedulerKey);

	addressTranslator = new AddressTranslator(addressMappingKey);
	PRINT("- - Channel"<<channel<<": Building MC Address Translator");
	requestQueues = new SystemQueue<Request>(channel, requestQueueKey);
	PRINT("- - Channel"<<channel<<": Building MC Request Queue = "<<requestQueueKey);
	requestScheduler = SystemFactory::GetInstance()->GetRequestScheduler(channel,requestSchedulerKey);
	PRINT("- - Channel"<<channel<<": Building MC Request Scheduler = "<<requestSchedulerKey);
	commandGenerator = SystemFactory::GetInstance()->GetCommandGenerator(channel,commandGeneratorKey);
	PRINT("- - Channel"<<channel<<": Building MC Command Generator = "<<commandGeneratorKey);
	commandQueues = new SystemQueue<BusPacket>(channel, commandQueueKey);
	PRINT("- - Channel"<<channel<<": Building MC Command Queue = "<<commandQueueKey);
	commandScheduler = SystemFactory::GetInstance()->GetCommandScheduler(channel,commandSchedulerKey);
	PRINT("- - Channel"<<channel<<": Building MC Command Scheduler = "<<commandSchedulerKey);
	dataManager = new DataManager(memoryTimingTable[channel][tRL], memoryTimingTable[channel][tWL], memoryTimingTable[channel][tBUS]);
	PRINT("- - Channel"<<channel<<": Building MC Data Manager");

	// Initialize bank row table
	int totalBank = memoryLevelTable[channel][DRAM_Level::BANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP] * memoryLevelTable[channel][DRAM_Level::RANK];
	std::vector<int> rowTable;
	for ( int bank = 0; bank < totalBank; bank++ )
	{
		rowTable.push_back(0);
	}
	bankOpenRowTable[channel] = rowTable;
}

MemoryController::~MemoryController()
{
	// TODO Auto-generated destructor stub
}


void MemoryController::AddRequest(Request* request)
{
	// Pending Request for clock increment
	incomingRequestBuffer.push_back(request);
}

Request* MemoryController::ReturnRequest( void )
{
	return dataManager->ReceiveData();
}

void MemoryController::Tick(void)
{
	// ----- Translate request and insert to request Queue interface
	addressTranslator->Tick(incomingRequestBuffer);

	// ----- Put all the incoming requests from addressTranslator into corresponding buffer system
	requestQueues->Tick(&incomingRequestBuffer);

	// ----- Schedule ready request from the request Queue, it needs to know the status of command queue
	requestScheduler->Tick(requestQueues, commandQueues, &scheduledRequestBuffer);

	// ------ Convert schedule requests into commands based on generation policy and insert to command Queue interface
    commandGenerator->Tick(&scheduledRequestBuffer, &generatedCommandBuffer);

	// ----- Put all generated commands from the command generator into corresponding buffer system
	commandQueues->Tick(&generatedCommandBuffer);

	// ----- Schedule command from the command queue and send to output port scheduleCommand. Update all internal tables
	commandScheduler->Tick(commandQueues, scheduledCommand);

	// ----- Send command to memory device
	memoryDevice->Tick(scheduledCommand);

	// ----- Update data manager for either read or write data and communicate with the device
	dataManager->Tick(scheduledCommand, memoryDevice);
	clock++;
}

void MemoryController::AssignConfigurationKey(std::string configFile, std::string & addressMap, std::string & requestQueue, std::string & requestScheduler,
		std::string & commandGenerator, std::string & commandQueue, std::string & commandScheduler)
{
	std::ifstream config;
	config.open(configFile.c_str());

	if (config.is_open() )
	{
		while ( !config.eof() )
		{
			std::string key, value, line;
			getline(config, line);
			if (line.size() == 0)
			{
				continue;
			}
			size_t index = line.find_first_of(";");
			if (index != std::string::npos )
			{
				continue;
			}

			size_t equalIndex = line.find_first_of("=");
			if ( equalIndex == std::string::npos)
			{
				ERROR(" MISSING ASSIGNMENT FOR CONFIGURATION \n");
				abort();
			}
			key = line.substr(0, equalIndex);
			value = line.substr(equalIndex+1, line.size()-equalIndex);
			if ( key == "ADDRESS_MAPPING")
			{
				addressMap = value;
			}
			else if ( key == "REQUEST_QUEUE")
			{
				requestQueue = value;
			}
			else if ( key == "REQUEST_SCHEDULER")
			{
				requestScheduler = value;
			}
			else if ( key == "COMMAND_GENERATOR")
			{
				commandGenerator = value;
			}
			else if ( key == "COMMAND_QUEUE")
			{
				commandQueue = value;
			}
			else if ( key == "COMMAND_SCHEDULER")
			{
				commandScheduler = value;
			}
			else
			{
			}
		}
	}
}


} /* namespace MCsimulator */
