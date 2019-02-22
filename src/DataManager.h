/*
 * DataManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: dlguo
 */

#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include <vector>

#include "MemoryDevice.h"
#include "Request.h"
#include "BusPacket.h"

namespace MCsimulator
{

class DataManager
{
public:
	DataManager(int latencyRD, int latencyWR, int latencyBUS);
	virtual ~DataManager();
	void Tick(BusPacket* scheduledCommand, MemoryDevice* memoryDevice);
	Request* ReceiveData();
private:
	int clock;
	int latencyRead;
	int latencyWrite;
	std::vector<int> writeDataCounter;
	std::vector<int> readDataCounter;

	std::vector<BusPacket*> pendingWriteDataQueue;
	std::vector<BusPacket*> pendingReadDataQueue;
	std::vector<Request*> completeDataQueue;
};

} /* namespace MCsimulator */

#endif /* DATAMANAGER_H_ */
