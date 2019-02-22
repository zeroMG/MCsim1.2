

#include "SystemQueue.h"

using namespace MCsimulator;
using namespace std;
namespace MCsimulator{
template <>
bool SystemQueue<BusPacket>::IsQueueReady(unsigned queueIndex, BusPacket* element)
{
	// cout<<"Line 1  "<<endl;
	// cout<<"the command is "<<element->command<<endl;
	bool ready = false;
	int requestorIndex = 0;
	if (requestorLevelEnabled)
	{
		//cout<<"Line 16  "<<endl;
		requestorIndex = element->requestorID;
	}
	
	if (element->physicalAddress[BANK] == perQueueCmdLevelHistory[queueIndex][requestorIndex][BANK] )
	{
		//cout<<"Line 2  "<<endl;
		//cout<<"the counter for tRP is showing     "<<perQueueIntraBankTable[queueIndex][element->requestorID][element->command]<<endl;
		if (perQueueIntraBankTable[queueIndex][element->requestorID][element->command] == 0)
		{
			//cout<<"Line 3  "<<endl;
			ready = true;
		}
	}
	else if ( element->physicalAddress[BANKGROUP] == perQueueCmdLevelHistory[queueIndex][requestorIndex][BANKGROUP] )
	{
		//cout<<"Line 4  "<<endl;
		if (perQueueInterBankIntraBGTable[queueIndex][element->requestorID][element->command] == 0)
		{
			//cout<<"Line 5  "<<endl;
			ready = true;
		}
	}
	else if ( element->physicalAddress[RANK] == perQueueCmdLevelHistory[queueIndex][requestorIndex][RANK] )
	{
		//cout<<"Line 6  "<<endl;
		if (perQueueInterBGIntraRankTable[queueIndex][element->requestorID][element->command] == 0)
		{
			//cout<<"Line 7  "<<endl;
			ready = true;
		}
	}
	else
	{
		//cout<<"Line 8  "<<endl;
		if (perQueueInterRankTable[queueIndex][element->requestorID][element->command] == 0)
		{
			//cout<<"Line 9  "<<endl;
			ready = true;
		}
	}
	//cout<<"Line 10  "<<endl;
	return ready;
}



template <>
void SystemQueue<BusPacket>::UpdateCommandTimeTable(unsigned queueIndex, BusPacket* element)
{
	//cout<<"Line 11  "<<endl;
	int requestorIndex = 0;
	if (requestorLevelEnabled)
	{
		//cout<<"Line 12  "<<endl;
		requestorIndex = element->requestorID;
	}
	//cout<<"salam    "<<queueIndex<<endl;
	perQueueCmdLevelHistory[queueIndex][requestorIndex][BANK] = element->physicalAddress[BANK];
	//cout<<"salam  222222222  "<<endl;
	perQueueCmdLevelHistory[queueIndex][requestorIndex][BANKGROUP] = element->physicalAddress[BANKGROUP];
	perQueueCmdLevelHistory[queueIndex][requestorIndex][RANK] = element->physicalAddress[RANK];
	
	switch (element->command)
	{
	case ACT_R:
		// Same bank
		//cout<<"Line 13  "<<endl;
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRC];
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRC];
		perQueueIntraBankTable[queueIndex][requestorIndex][PRE] = memoryTimingTable[channel][tRAS];//Change tRC to tRAS
		perQueueIntraBankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRCD];
		// Different bank, same bankgroup
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRRD];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRRD];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRRD];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRRD];
		break;
	case ACT_W:
		// Same bank
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRC];
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRC];
		perQueueIntraBankTable[queueIndex][requestorIndex][PRE] = memoryTimingTable[channel][tRAS];
		perQueueIntraBankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRCD];
		// Different bank, same bankgroup
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRRD];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRRD];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRRD];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRRD];
		break;	
	case PRE:
		// Same bank
		//cout<<"Line 14  "<<endl;
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_R] = memoryTimingTable[channel][tRP];
		perQueueIntraBankTable[queueIndex][requestorIndex][ACT_W] = memoryTimingTable[channel][tRP];
		break;
	case RD:
	case RDA:
		// Same bank
		perQueueIntraBankTable[queueIndex][requestorIndex][PRE] = memoryTimingTable[channel][tRTP];
		perQueueIntraBankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tCCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tCCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRTW];
		perQueueIntraBankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRTW];
		// Different bank, same bankgroup
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tCCD];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tCCD];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRTW];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRTW];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tCCD];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tCCD];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRTW];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRTW];
		// Different Rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		break;
	case WR:
	case WRA:
		// Same bank
		perQueueIntraBankTable[queueIndex][requestorIndex][PRE] = memoryTimingTable[channel][tWL] +
				memoryTimingTable[channel][tBUS] + memoryTimingTable[channel][tWR];
		perQueueIntraBankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tWTR];
		perQueueIntraBankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tWTR];
		perQueueIntraBankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tCCD];
		perQueueIntraBankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tCCD];
		// Different bank, same bankgroup
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tWTR];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tWTR];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tCCD];
		perQueueInterBankIntraBGTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tCCD];
		// Different bankgroup, same rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tWTR];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tWTR];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tCCD];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tCCD];
		// Different Rank
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RD] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][RDA] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WR] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		perQueueInterBGIntraRankTable[queueIndex][requestorIndex][WRA] = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
		break;
		break;
	default:
		break;
	}
	
	return;
}


template <>
void SystemQueue<BusPacket>::Tick()

{
	//cout<<" Am I heree nowwwwwwwwwwwwwwwwwwww"<<endl;
	for (unsigned queue = 0; queue < perQueueCmdHistory.size(); queue++)
	{
		for (auto it = perQueueIntraBankTable[queue].begin(); it != perQueueIntraBankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterBankIntraBGTable[queue].begin(); it != perQueueInterBankIntraBGTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterBGIntraRankTable[queue].begin(); it != perQueueInterBGIntraRankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterRankTable[queue].begin(); it != perQueueInterRankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
	}
}


template <>
void SystemQueue<BusPacket>::Tick(std::vector<BusPacket*>* incomingElementBuffer)

{
	// Check the buffer condition, time out...
	cout<<"Do we come here for tick????????????????????????????????   "<<endl;
	while (true)
	{
		if ( incomingElementBuffer->empty() )
		{
			break;
		}
		else
		{
			BusPacket* element = incomingElementBuffer->front();
			EnqueueElement(element);
			incomingElementBuffer->erase(incomingElementBuffer->begin());
		}
	}

	for (unsigned queue = 0; queue < perQueueCmdHistory.size(); queue++)
	{
		for (auto it = perQueueIntraBankTable[queue].begin(); it != perQueueIntraBankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterBankIntraBGTable[queue].begin(); it != perQueueInterBankIntraBGTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterBGIntraRankTable[queue].begin(); it != perQueueInterBGIntraRankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
		for (auto it = perQueueInterRankTable[queue].begin(); it != perQueueInterRankTable[queue].end(); it++)
		{
			for (std::map<int, int>::iterator table = it->second.begin(); table != it->second.end(); table++)
			{
				if (table->second != 0)
				{
					table->second--;
				}
			}
		}
	}
}

}