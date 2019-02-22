/*
 * CommandScheduler.cpp
 *
 *  Created on: Dec 7, 2017
 *      Author: dlguo
 */

#include <vector>
#include <math.h>

#include "CommandScheduler.h"
#include "BankOpenRowTable.h"

namespace MCsimulator
{

CommandScheduler::CommandScheduler(int channel):
	channel(channel)
{
	// TODO Auto-generated constructor stub
	unsigned rankNum = memoryLevelTable[channel][DRAM_Level::RANK];
	unsigned bkGpNum = memoryLevelTable[channel][DRAM_Level::BANKGROUP];
	unsigned bankNum = memoryLevelTable[channel][DRAM_Level::BANK];

	for (unsigned rank = 0; rank < rankNum; rank++)
	{
		channelACTTable[rank] = std::vector<int>();
		for (unsigned bankG = 0; bankG < bkGpNum; bankG++)
		{
			for (unsigned bank = 0; bank < bankNum; bank++)
			{
				std::map<int,int> cmdTable;
				for(unsigned cmd = DRAM_Packet::RD; cmd < DRAM_Packet::REF; cmd++)
				{
					cmdTable[cmd] = 0;
				}
				channelTimeTable.push_back(cmdTable);
			}
		}
	}
}

CommandScheduler::~CommandScheduler()
{
	// TODO Auto-generated destructor stub
}

bool CommandScheduler::IsIssuable(BusPacket* command)
{
	bool issuable = false;
	int tableIndex = memoryLevelTable[channel][DRAM_Level::BANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
			command->physicalAddress[DRAM_Level::RANK] +
			memoryLevelTable[channel][DRAM_Level::BANK] * command->physicalAddress[DRAM_Level::BANKGROUP] +
			command->physicalAddress[DRAM_Level::BANK];
	if (channelTimeTable[tableIndex][command->command] == 0)
	{
		issuable = true;
	}
	else
	{
		//PRINT("------ next command issuable = "<<channelTimeTable[tableIndex][command->command]);
	}
	return issuable;
}

void CommandScheduler::RefreshAllBanks()
{
	for(unsigned index = 0; index < bankOpenRowTable[channel].size(); index++)
	{
		bankOpenRowTable[channel][index] = -1;
	}
	return;
}

//	Inter-Rank:	Update for CAS
void CommandScheduler::UpdateInterRank(unsigned rankMin, unsigned rankMax, int type)
{
	int rankCAS = memoryTimingTable[channel][tRTR] + memoryTimingTable[channel][tBUS];
	for (unsigned index = 0; index < channelTimeTable.size(); index++)
	{
		if (index < rankMin || index > rankMax)
		{
			switch (type)
			{
			case RD:
			case RDA:
			case WR:
			case WRA:
				channelTimeTable[index][RD] = GetTimeDelay(channelTimeTable[index][RD], rankCAS);
				channelTimeTable[index][RDA] = GetTimeDelay(channelTimeTable[index][RDA], rankCAS);
				channelTimeTable[index][WR] = GetTimeDelay(channelTimeTable[index][WR], rankCAS);
				channelTimeTable[index][WRA] = GetTimeDelay(channelTimeTable[index][WRA], rankCAS);
				break;
			case REF:
				channelTimeTable[index][ACT_W] = memoryTimingTable[channel][tRFC];
				channelTimeTable[index][ACT_R] = memoryTimingTable[channel][tRFC];
				break;
			default:
				break;
			}
		}
	}
}

//	Inter-BankGroup:	 Update for ACT and CAS with shorter timing constraint
void CommandScheduler::UpdateIntraRankInterBankGroup(unsigned rankMin, unsigned rankMax, unsigned bkGpMin, unsigned bkGpMax, int type)
{
	for (unsigned index = rankMin; index <= rankMax; index++)
	{
		if (index < bkGpMin || index > bkGpMax)
		{
			switch (type)
			{
			default:
				break;
			}
		}
	}
}

//	Intra-BankGroup: Inter-bank and Intra-bank
void CommandScheduler::UpdateIntraRankIntraBankGroup(unsigned bkGpMin, unsigned bkGpMax, unsigned bankIndex, int type, unsigned rank)
{
	for (unsigned index = bkGpMin; index <= bkGpMax; index++)
	{
		switch(type)
		{
		case RD:
		case RDA:
			channelTimeTable[index][RD] = GetTimeDelay(channelTimeTable[index][RD], memoryTimingTable[channel][tCCD]);
			channelTimeTable[index][RDA] = GetTimeDelay(channelTimeTable[index][RDA], memoryTimingTable[channel][tCCD]);
			channelTimeTable[index][WR] = GetTimeDelay(channelTimeTable[index][WR], memoryTimingTable[channel][tRTW]);
			channelTimeTable[index][WRA] = GetTimeDelay(channelTimeTable[index][WRA], memoryTimingTable[channel][tRTW]);
			if (index == bankIndex)
			{
				channelTimeTable[index][PRE] = GetTimeDelay(channelTimeTable[index][PRE], memoryTimingTable[channel][tRTP]);
			}
			break;
		case WR:
		case WRA:
			channelTimeTable[index][RD] = GetTimeDelay(channelTimeTable[index][RD], memoryTimingTable[channel][tWTR]);
			channelTimeTable[index][RDA] = GetTimeDelay(channelTimeTable[index][RDA], memoryTimingTable[channel][tWTR]);
			channelTimeTable[index][WR] = GetTimeDelay(channelTimeTable[index][WR], memoryTimingTable[channel][tCCD]);
			channelTimeTable[index][WRA] = GetTimeDelay(channelTimeTable[index][WRA], memoryTimingTable[channel][tCCD]);
			if (index == bankIndex)
			{
				channelTimeTable[index][PRE] = GetTimeDelay(channelTimeTable[index][PRE], memoryTimingTable[channel][tWR]);
			}
			break;
		case ACT_R:
			if (index == bankIndex)
			{
				channelTimeTable[index][RD] = GetTimeDelay(channelTimeTable[index][RD], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][RDA] = GetTimeDelay(channelTimeTable[index][RDA], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][WR] = GetTimeDelay(channelTimeTable[index][WR], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][WRA] = GetTimeDelay(channelTimeTable[index][WRA], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R], memoryTimingTable[channel][tRC]);
				channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W], memoryTimingTable[channel][tRC]);
			}
			else
			{
				if (channelACTTable[rank].size() == 4)
				{
					// four-activation window
					channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R], channelACTTable[rank].front());
					channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W], channelACTTable[rank].front());
//					PRINT(" Update after t ACT Window: next other bank ACT = "<<channelTimeTable[index][ACT]<<
//						  " constraint = "<<channelACTTable[rank].front());
				}
				else
				{
					channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R],memoryTimingTable[channel][tRRD]);
					channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W],memoryTimingTable[channel][tRRD]);
				}
			}
			break;
		case ACT_W:
			if (index == bankIndex)
			{
				channelTimeTable[index][RD] = GetTimeDelay(channelTimeTable[index][RD], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][RDA] = GetTimeDelay(channelTimeTable[index][RDA], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][WR] = GetTimeDelay(channelTimeTable[index][WR], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][WRA] = GetTimeDelay(channelTimeTable[index][WRA], memoryTimingTable[channel][tRCD]);
				channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W], memoryTimingTable[channel][tRC]);
				channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R], memoryTimingTable[channel][tRC]);
			}
			else
			{
				if (channelACTTable[rank].size() == 4)
				{
					// four-activation window
					channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W], channelACTTable[rank].front());
					channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R], channelACTTable[rank].front());
//					PRINT(" Update after t ACT Window: next other bank ACT = "<<channelTimeTable[index][ACT]<<
//						  " constraint = "<<channelACTTable[rank].front());
				}
				else
				{
					channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W],memoryTimingTable[channel][tRRD]);
					channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R],memoryTimingTable[channel][tRRD]);
				}
			}
			break;	
		case PRE:
			//cout<<"Salaammm bar toooo"<<endl;
			if (index == bankIndex)
			{
				channelTimeTable[index][ACT_R] = GetTimeDelay(channelTimeTable[index][ACT_R], memoryTimingTable[channel][tRP]);
				channelTimeTable[index][ACT_W] = GetTimeDelay(channelTimeTable[index][ACT_W], memoryTimingTable[channel][tRP]);
			}
			break;
		case REF:
			channelTimeTable[index][ACT_R] = memoryTimingTable[channel][tRFC];
			channelTimeTable[index][ACT_W] = memoryTimingTable[channel][tRFC];
			break;
		}
	}
}


int CommandScheduler::GetTimeDelay(int delay, int constraint)
{
	int tmpDelay = delay;
	if (delay < constraint)
	{
		tmpDelay = constraint;
	}
	return tmpDelay;
}

void CommandScheduler::TickTimeTable()
{
	
	for (unsigned index = 0; index < channelTimeTable.size(); index++)
	{
		for (std::map<int,int>::iterator it = channelTimeTable[index].begin(); it != channelTimeTable[index].end(); it++)
		{
			//cout<<"Aya injaa tick mikhore22222222222????????????????"<<endl;
			//cout<<"it-> second issssssssssss"<<it->second<<endl;
			if ( it->second > 0)
			{
				//cout<<"Aya injaa tick mikhore33333333333????????????????"<<endl;
				it->second--;
			}
		}
	}
	if ( !channelACTTable.empty() )
	{
		for (unsigned rk = 0; rk < channelACTTable.size(); rk++)
		{
			if (!channelACTTable[rk].empty())
			{
				for ( unsigned index = 0; index < channelACTTable[rk].size(); index++)
				{
					channelACTTable[rk][index]--;
				}
				if (channelACTTable[rk].front() == 0)
				{
					channelACTTable[rk].erase(channelACTTable[rk].begin());
				}
			}
		}
	}
}




void CommandScheduler::UpdateTimingTable(BusPacket* command)
{
	///< Find the corresponding bank level
	int tableIndex = memoryLevelTable[channel][DRAM_Level::BANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP] *
			command->physicalAddress[DRAM_Level::RANK] +
			memoryLevelTable[channel][DRAM_Level::BANK] * command->physicalAddress[DRAM_Level::BANKGROUP] +
			command->physicalAddress[DRAM_Level::BANK];

	int rankBoundMin = memoryLevelTable[channel][DRAM_Level::BANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP]
						* command->physicalAddress[DRAM_Level::RANK];
	int rankBoundMax = memoryLevelTable[channel][DRAM_Level::BANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP]
						* (command->physicalAddress[DRAM_Level::RANK] + 1) - 1;

	int bkGpBoundMin = rankBoundMin + memoryLevelTable[channel][DRAM_Level::BANK]
						* command->physicalAddress[DRAM_Level::BANKGROUP];
	int bkGpBoundMax = rankBoundMin + memoryLevelTable[channel][DRAM_Level::BANK]
						* (command->physicalAddress[DRAM_Level::BANKGROUP] + 1) - 1;

	// Update different level time tables
	UpdateInterRank(rankBoundMin, rankBoundMax, command->command);
	UpdateIntraRankInterBankGroup(rankBoundMin, rankBoundMax, bkGpBoundMin, bkGpBoundMax, command->command);
	UpdateIntraRankIntraBankGroup(bkGpBoundMin, bkGpBoundMax, tableIndex, command->command, command->physicalAddress[DRAM_Level::RANK]);
}


void CommandScheduler::Tick(SystemQueue<BusPacket>* commandQueues, BusPacket* &scheduledCommand)
{
	scheduledCommand = NULL;
	ScheduleCommand(commandQueues, scheduledCommand);
	if (scheduledCommand != NULL)
	{
		if ( scheduledCommand->command == ACT_R )
		{
			channelACTTable[scheduledCommand->physicalAddress[DRAM_Level::RANK]].push_back(memoryTimingTable[channel][tFAW]);
			BankOpenRowTable::getInstance()->updateCurrentOpenRow(channel, 
																  scheduledCommand->physicalAddress[DRAM_Level::RANK],
																  scheduledCommand->physicalAddress[DRAM_Level::BANKGROUP], 
																  scheduledCommand->physicalAddress[DRAM_Level::BANK], 
																  scheduledCommand->physicalAddress[DRAM_Level::ROW]);
		}
		else if ( scheduledCommand->command == ACT_W )
		{
			channelACTTable[scheduledCommand->physicalAddress[DRAM_Level::RANK]].push_back(memoryTimingTable[channel][tFAW]);
			BankOpenRowTable::getInstance()->updateCurrentOpenRow(channel, 
																  scheduledCommand->physicalAddress[DRAM_Level::RANK],
																  scheduledCommand->physicalAddress[DRAM_Level::BANKGROUP], 
																  scheduledCommand->physicalAddress[DRAM_Level::BANK], 
																  scheduledCommand->physicalAddress[DRAM_Level::ROW]);
		}
		else if ( scheduledCommand->command == PRE )
		{
			// Close row in the bank
			BankOpenRowTable::getInstance()->updateCurrentOpenRow(channel, 
																  scheduledCommand->physicalAddress[DRAM_Level::RANK],
																  scheduledCommand->physicalAddress[DRAM_Level::BANKGROUP], 
																  scheduledCommand->physicalAddress[DRAM_Level::BANK], 
																  -1);			
		}
		//PRINT("++ CmdSch: CMD "<<scheduledCommand->command);
		UpdateTimingTable(scheduledCommand);
	}
	else
	{
		//PRINT("++ CmdSch: NONE ");
	}
	commandQueues->Tick();
	TickTimeTable();
}

} /* namespace MCsimulator */
