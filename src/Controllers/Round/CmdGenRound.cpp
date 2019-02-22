/*
 * CmdGenORP.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: dlguo
 */

#include "CmdGenRound.h"
#include "BankOpenRowTable.h"

namespace MCsimulator
{

CmdGen_Round::CmdGen_Round(int channel): CommandGenerator(channel)
{
	// TODO Auto-generated constructor stub

}

CmdGen_Round::~CmdGen_Round()
{
	// TODO Auto-generated destructor stub
}

void CmdGen_Round::GenerateCommand(Request* request, std::vector<BusPacket*>* const generatedCommandBuffer)
{
	int size = request->size/memoryLevelTable[channel][DATABUS];

	if (BankOpenRowTable::getInstance()->currentOpenRow(channel, request->physicalAddress[RANK], 
			request->physicalAddress[BANKGROUP], request->physicalAddress[BANK]) != request->physicalAddress[ROW])
	{
		BusPacket* cmdPRE = new BusPacket(DRAM_Packet::PRE, request->requestorID, request->address, request->physicalAddress, request->data);
		generatedCommandBuffer->push_back(cmdPRE);
		if (request->type == DRAM_Request::READ_DATA)
		{
			BusPacket* cmdACT = new BusPacket(DRAM_Packet::ACT_R, request->requestorID, request->address, request->physicalAddress, request->data);
			generatedCommandBuffer->push_back(cmdACT);
		}
		else
		{
			BusPacket* cmdACT = new BusPacket(DRAM_Packet::ACT_W, request->requestorID, request->address, request->physicalAddress, request->data);
			generatedCommandBuffer->push_back(cmdACT);
		}
	}

	for ( int i = 0; i < size; i++ )
	{
		BusPacket* cmdCAS = NULL;
		if (request->type == DRAM_Request::READ_DATA)
		{
			cmdCAS = new BusPacket(DRAM_Packet::RD, request->requestorID, request->address, request->physicalAddress, request->data);
		}
		else
		{
			cmdCAS = new BusPacket(DRAM_Packet::WR, request->requestorID, request->address, request->physicalAddress, request->data);
		}
		generatedCommandBuffer->push_back(cmdCAS);
		request->physicalAddress[COLUMN] += 8; /// Burst = 8
	}
}

} /* namespace MCsimulator */
