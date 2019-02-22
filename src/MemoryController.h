/*
 * MemoryController.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef MEMORYCONTROLLER_H_
#define MEMORYCONTROLLER_H_

#include <string>

#include "AddressTranslator.h"
#include "RequestScheduler.h"
#include "CommandGenerator.h"
#include "CommandScheduler.h"
#include "SystemQueue.h"
#include "DataManager.h"
#include "MemoryDevice.h"

#include "Request.h"
#include "BusPacket.h"


namespace MCsimulator
{

class MemoryDevice;
class MemoryController
{
public:
	/*
	 * The channel memory controller simulation container
	 * Different schedulers and queue system can be constructed
	 */
	MemoryController(int channel, std::string configFile, MemoryDevice* memoryDevice);
	virtual ~MemoryController();
	void AddRequest(Request* request);
	Request* ReturnRequest( void );
	void Tick( void );
private:
	int channel;
	int long clock;
	AddressTranslator* addressTranslator;
	RequestScheduler* requestScheduler;
	CommandGenerator* commandGenerator;
	CommandScheduler* commandScheduler;
	SystemQueue<Request>* requestQueues;
	SystemQueue<BusPacket>* commandQueues;
	DataManager* dataManager;
	MemoryDevice* memoryDevice;

	// System Components Interface
	std::vector<Request*> incomingRequestBuffer;
	std::vector<Request*> scheduledRequestBuffer;
	std::vector<BusPacket*> generatedCommandBuffer;
	BusPacket* scheduledCommand;

	void AssignConfigurationKey(std::string configFile, std::string & addressMap, std::string & requestQueue, std::string & requestScheduler,
			std::string & commandGenerator, std::string & commandQueue, std::string & commandScheduler);
};

} /* namespace MCsimulator */

#endif /* MEMORYCONTROLLER_H_ */
