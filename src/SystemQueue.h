/*
 * SystemQueue.h
 *
 *  Created on: Dec 6, 2017
 *      Author: dlguo
 */

#ifndef SYSTEMQUEUE_H_
#define SYSTEMQUEUE_H_

#include "Global.h"
#include "Request.h"
#include "BusPacket.h"

#include <vector>
#include <map>

using namespace std;

namespace MCsimulator
{
template <typename T>

class SystemQueue
{

public:
	SystemQueue(int channel, std::string queueKey);
	virtual ~SystemQueue();
	DRAM_Level GetType();
	int GetSize(unsigned queueIndex);
	T* GetElement( unsigned queueIndex, unsigned elementIndex );
	void RemoveElement( unsigned queueIndex, unsigned elementIndex );

	int GetRequestorSize( unsigned requestorID, unsigned queueIndex);
	T* GetRequestorElement( unsigned requestorID, unsigned queueIndex, unsigned elementIndex );
	void RemoveRequestorElement( unsigned requestorID, unsigned queueIndex, unsigned elementIndex );
	//void Tick(std::vector<T*>* incomingElementBuffer);
	void Tick(std::vector<T*>* incomingElementBuffer);
	void Tick();
	bool IsQueueReady(unsigned queueIndex, T* element);
	void UpdateCommandTimeTable(unsigned queueIndex, T* element);

private:
	int channel;
	DRAM_Level queueType;
	bool requestorLevelEnabled;
	std::vector<std::map<uint32_t, std::vector<T*>>> elementQueues;

	void EnqueueElement(T* element);

	/* Command Queue Specific
	 * There are 4 timing map for each command queue: intraBank, intraBG, intraRank, intraChannel
	 * if there is a ready check for a specific queue, the checked command must be 0 in the corresponding map
	 */
	typedef std::map<int, int> TimeTable;
	std::vector<std::map<int, TimeTable>> commandReadyTable;
	std::vector<std::map<int, TimeTable>> perQueueIntraBankTable;
	std::vector<std::map<int, TimeTable>> perQueueInterBankIntraBGTable;
	std::vector<std::map<int, TimeTable>> perQueueInterBGIntraRankTable;
	std::vector<std::map<int, TimeTable>> perQueueInterRankTable;

	/*
	 *  These maps are used to track the previous command's type and memory levels
	 *  The information of new command is compared with the history to find the right timing map
	 */
	std::vector<std::map<int, int>> perQueueCmdHistory;
	typedef std::map<int, int> LvelTable;
	std::vector<std::map<int, LvelTable>> perQueueCmdLevelHistory;
};

template <typename T>
SystemQueue<T>::SystemQueue(int channel, std::string queueKey):
	channel(channel)
{
	requestorLevelEnabled = false;
	queueType = DRAM_Level::CHANNEL;

	unsigned queueSize = 1;
	std::string key = queueKey;
	std::string param;
	while (true)
	{
		if (key.empty())
		{
			break;
		}
		else
		{
			size_t index = key.find_first_of("_");
			if (index != std::string::npos)
			{
				param = key.substr(0, index);
				key = key.substr(index+1, key.size()-index);
			}
			else
			{
				param = key;
				key.clear();
			}
			// Assign value
			if ( param == "Rank" )
			{
				queueType = DRAM_Level::RANK;
				queueSize = memoryLevelTable[channel][DRAM_Level::RANK];
			}
			else if ( param == "BankGroup" )
			{
				queueType = DRAM_Level::BANKGROUP;
				queueSize = memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
						memoryLevelTable[channel][DRAM_Level::RANK];
			}
			else if ( param == "Bank" )
			{
				queueType = DRAM_Level::BANK;
				queueSize = memoryLevelTable[channel][DRAM_Level::BANK] *
						memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
						memoryLevelTable[channel][DRAM_Level::RANK];
			}
			else if ( param == "Requestor")
			{
				requestorLevelEnabled = true;
			}
			else
			{
				// Do nothing
			}
		}
	}
	for (unsigned index = 0; index < queueSize; index++ )
	{
		//// Create memory level element queues
		std::map<unsigned, std::vector<T*>> newQueue;
		elementQueues.push_back(newQueue);

		//// Create timing table for each memory level element queues
		TimeTable newTable;
		TimeTable intraBankTable;
		TimeTable interBankIntraBGTable;
		TimeTable interBGIntraRank;
		TimeTable intraRankTable;

		std::map<int, int> cmdTable;
		cmdTable[0] = DRAM_Packet::DATA;
		perQueueCmdHistory.push_back(cmdTable);
		std::map<int, LvelTable> lvlTable;
		perQueueCmdLevelHistory.push_back(lvlTable);

		for (int cmd = DRAM_Packet::RD; cmd <= DRAM_Packet::REF; cmd++)
		{
			newTable[cmd] = 0;
			intraBankTable[cmd] = 0;
			interBankIntraBGTable[cmd] = 0;
			interBGIntraRank[cmd] = 0;
			intraRankTable[cmd] = 0;
		}
		std::map<int, TimeTable> newTimeTable;
		newTimeTable[0] = newTable;
		commandReadyTable.push_back(newTimeTable);

		std::map<int, TimeTable> reqIntraBankTable;
		reqIntraBankTable[0] = intraBankTable;
		perQueueIntraBankTable.push_back(reqIntraBankTable);

		std::map<int, TimeTable> reqInterBankIntraBGTable;
		reqInterBankIntraBGTable[0] = interBankIntraBGTable;
		perQueueInterBankIntraBGTable.push_back(reqInterBankIntraBGTable);

		std::map<int, TimeTable> reqInterBGIntraRank;
		reqInterBGIntraRank[0] = interBGIntraRank;
		perQueueInterBGIntraRankTable.push_back(reqInterBGIntraRank);

		std::map<int, TimeTable> reqIntraRankTable;
		reqIntraRankTable[0] = intraRankTable;
		perQueueInterRankTable.push_back(reqIntraRankTable);
	}
}

template <typename T>
SystemQueue<T>::~SystemQueue()
{

}

template <typename T>
DRAM_Level SystemQueue<T>::GetType()
{
	return queueType;
}


template <typename T>
void SystemQueue<T>::EnqueueElement(T* element)
{
	int queueIndex = 0;
	switch (queueType)
	{
	case DRAM_Level::RANK:
		queueIndex = element->physicalAddress[DRAM_Level::RANK];
		break;
	case DRAM_Level::BANKGROUP:
		queueIndex = memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
					 element->physicalAddress[DRAM_Level::RANK] +
					 element->physicalAddress[DRAM_Level::BANKGROUP];
		break;
	case DRAM_Level::BANK:
		queueIndex = memoryLevelTable[channel][DRAM_Level::BANK] *
					 memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
					 element->physicalAddress[DRAM_Level::RANK] +
					 memoryLevelTable[channel][DRAM_Level::BANK] *
					 element->physicalAddress[DRAM_Level::BANKGROUP] +
					 element->physicalAddress[DRAM_Level::BANK];
		break;
	default:
		break;
	}
	if (requestorLevelEnabled)
	{

		elementQueues[queueIndex][element->requestorID].push_back(element);
		/// If the requestor queue is not created, create timeTable
		if (commandReadyTable[queueIndex].find(element->requestorID) == commandReadyTable[queueIndex].end())
		{
			TimeTable newTable;

			TimeTable intraBankTable;
			TimeTable interBankIntraBGTable;
			TimeTable interBGIntraRank;
			TimeTable intraRankTable;
			for (int cmd = DRAM_Packet::RD; cmd <= DRAM_Packet::REF; cmd++)
			{
				newTable[cmd] = 0;

				intraBankTable[cmd] = 0;
				interBankIntraBGTable[cmd] = 0;
				interBGIntraRank[cmd] = 0;
				intraRankTable[cmd] = 0;
			}
			commandReadyTable[queueIndex][element->requestorID] = newTable;

			perQueueIntraBankTable[queueIndex][element->requestorID] = intraBankTable;
			perQueueInterBankIntraBGTable[queueIndex][element->requestorID] = interBankIntraBGTable;
			perQueueInterBGIntraRankTable[queueIndex][element->requestorID] = interBGIntraRank;
			perQueueInterRankTable[queueIndex][element->requestorID] = intraRankTable;
		}
	}
	else
	{
		elementQueues[queueIndex][0].push_back(element);
	}
}

template <typename T>
int SystemQueue<T>::GetSize(unsigned queueIndex)
{
	return elementQueues[queueIndex][0].size();
}

template <typename T>
T* SystemQueue<T>::GetElement( unsigned queueIndex, unsigned elementIndex )
{
	T* searchElement = NULL;
	if ( queueIndex < elementQueues.size() )
	{
		if ( elementIndex < elementQueues[queueIndex][0].size() )
		{
			searchElement = elementQueues[queueIndex][0][elementIndex];
		}
	}
	return searchElement;
}

template <typename T>
void SystemQueue<T>::RemoveElement( unsigned queueIndex, unsigned elementIndex )
{
	elementQueues[queueIndex][0].erase(elementQueues[queueIndex][0].begin() + elementIndex );
}

template <typename T>
int SystemQueue<T>::GetRequestorSize( unsigned requestorID, unsigned queueIndex)
{
	return elementQueues[queueIndex][requestorID].size();
}


template <typename T>
T* SystemQueue<T>::GetRequestorElement( unsigned requestorID, unsigned queueIndex, unsigned elementIndex )
{
	T* searchElement = NULL;
	if ( queueIndex < elementQueues.size() )
	{
		if ( elementQueues[queueIndex].find(requestorID) != elementQueues[queueIndex].end() )
		{
			if ( elementIndex < elementQueues[queueIndex][requestorID].size() )
			{
				searchElement = elementQueues[queueIndex][requestorID][elementIndex];
			}
		}
	}
	return searchElement;
}

template <typename T>
void SystemQueue<T>::RemoveRequestorElement( unsigned requestorID, unsigned queueIndex, unsigned elementIndex )
{
	elementQueues[queueIndex][requestorID].erase(elementQueues[queueIndex][requestorID].begin() + elementIndex );
}

template <typename T>

void SystemQueue<T>::Tick(std::vector<T*>* incomingElementBuffer)
{
	//cout<<" Aya inja miyad"<<endl;
	while (true)
	{
		if ( incomingElementBuffer->empty() )
		{
			break;
		}
		else
		{
			T* element = incomingElementBuffer->front();
			EnqueueElement(element);
			incomingElementBuffer->erase(incomingElementBuffer->begin());
		}
	}
}


} /* namespace MCsimulator */

#endif /* SYSTEMQUEUE_H_ */
