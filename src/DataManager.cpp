/*
 * DataManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: dlguo
 */

#include "DataManager.h"

namespace MCsimulator
{

DataManager::DataManager(int latencyRD, int latencyWR, int latencyBUS)
{
	latencyRead = latencyRD + latencyBUS;
	latencyWrite = latencyWR + latencyBUS;
	// TODO Auto-generated constructor stub
	clock = 0;
}

DataManager::~DataManager()
{
	// TODO Auto-generated destructor stub
}

Request* DataManager::ReceiveData()
{
	Request* returnData = NULL;
	if ( !completeDataQueue.empty() )
	{
		returnData = completeDataQueue.front();
		completeDataQueue.erase(completeDataQueue.begin());
		//PRINT("Return REQ" <<returnData->requestorID<< " Complete Data\n");
	}
	return returnData;
}

void DataManager::Tick(BusPacket* scheduledCommand, MemoryDevice* memoryDevice)
{
	if (scheduledCommand != NULL)
	{

		if ( scheduledCommand->command == DRAM_Packet::WR ||
				scheduledCommand->command == DRAM_Packet::WRA)
		{
			writeDataCounter.push_back(latencyWrite);
			BusPacket* newData = new BusPacket(scheduledCommand);
			pendingWriteDataQueue.push_back(newData);
		}
		else if ( scheduledCommand->command == DRAM_Packet::RD ||
				scheduledCommand->command == DRAM_Packet::RDA)
		{
			readDataCounter.push_back(latencyRead);
			BusPacket* newData = new BusPacket(scheduledCommand);
			pendingReadDataQueue.push_back(newData);
		}
	}
	/// Process write data
	if (!writeDataCounter.empty())
	{
		for(unsigned i = 0; i<writeDataCounter.size(); i++)
		{
			writeDataCounter[i]--;
		}
		if (writeDataCounter.front() == 0)
		{
			///// Send data out
			BusPacket* data = pendingWriteDataQueue.front();
			memoryDevice->ReceiveData(data);

			//PRINT("++ Data: Write Data Complete\n");

			Request* returnRequest = new Request(data);
			completeDataQueue.push_back(returnRequest);
			pendingWriteDataQueue.erase(pendingWriteDataQueue.begin());
			writeDataCounter.erase(writeDataCounter.begin());
			delete data;
		}
		else
		{
			//PRINT("++ Data: Next Write data = "<<writeDataCounter.front());
		}
	}
	/// Process read data
	if (!readDataCounter.empty())
	{
		for(unsigned i = 0; i<readDataCounter.size(); i++)
		{
			readDataCounter[i]--;
		}
		if (readDataCounter.front() == 0)
		{
			///// Send data out
			BusPacket* data = pendingReadDataQueue.front();
			memoryDevice->RetrieveData(data);

			//PRINT("++ Data: Read Data Complete\n");

			Request* returnRequest = new Request(data);
			completeDataQueue.push_back(returnRequest);
			pendingReadDataQueue.erase(pendingReadDataQueue.begin());
			readDataCounter.erase(readDataCounter.begin());
			delete data;
		}
		else
		{
			//PRINT("++ Data: Next Read data = "<<readDataCounter.front());
		}
	}
	clock++;
}

} /* namespace MCsimulator */
