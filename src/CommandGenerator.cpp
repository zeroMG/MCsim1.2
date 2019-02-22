/*
 * CommandGenerator.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: dlguo
 */

#include "CommandGenerator.h"

#include "BankOpenRowTable.h"
#include "Global.h"

#include <string>
#include <vector>

namespace MCsimulator
{

CommandGenerator::CommandGenerator(int channel):
	channel(channel)
//	interleavedMemory(interleavedMemory)
{
	// TODO Auto-generated constructor stub
}

CommandGenerator::~CommandGenerator()
{
	// TODO Auto-generated destructor stub
}

void CommandGenerator::GenerateCommand(Request* request,
		std::vector<BusPacket*>* const generatedCommandBuffer)//Request* request, std::vector<BusPacket*> & generatedCommandBuffer
{
// FOR OPEN PAGE CHECKING
//	int bank = request->physicalAddress[RANK] * memoryLevelTable[channel][DRAM_Level::BANKGROUP] * memoryLevelTable[channel][DRAM_Level::BANK]
//			+ request->physicalAddress[BANKGROUP] * memoryLevelTable[channel][DRAM_Level::BANK]
//			+ request->physicalAddress[BANK];
//	// -1 indicates close bank
//	if (bankOpenRowTable[channel][bank] == -1)
//	{
//
//	}
	return;
}

void CommandGenerator::Tick(std::vector<Request*>* const scheduledRequestBuffer,
		std::vector<BusPacket*>* const generatedCommandBuffer)
{
	BankOpenRowTable* pendingRowTable = BankOpenRowTable::getInstance();
	while (!scheduledRequestBuffer->empty())
	{
		Request* request = scheduledRequestBuffer->front();
		GenerateCommand(request, generatedCommandBuffer);
		if ( pendingRowTable != NULL )
		{
			pendingRowTable->updatePendingOpenRow(channel, request->physicalAddress[DRAM_Level::RANK], request->physicalAddress[DRAM_Level::BANKGROUP],
												  request->physicalAddress[DRAM_Level::BANK], request->physicalAddress[DRAM_Level::ROW]);
		}
		scheduledRequestBuffer->erase(scheduledRequestBuffer->begin());
	}
	//PRINT("++ CmdGen: Command Buffer = "<<generatedCommandBuffer->size()<<"----"<<scheduledRequestBuffer->size());
}


} /* namespace MCsimulator */
