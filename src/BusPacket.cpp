/*
 * BusPacket.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#include "BusPacket.h"

namespace MCsimulator
{

BusPacket::BusPacket(DRAM_Packet packetType, int id, int addr, int physicalAddr[TOTAL_LEVEL], void *data):
	command(packetType),
	requestorID(id),
	address(addr),
	data(data)
{
	// TODO Auto-generated constructor stub
	physicalAddress[RANK] = physicalAddr[RANK];
	physicalAddress[BANKGROUP] = physicalAddr[BANKGROUP];
	physicalAddress[BANK] = physicalAddr[BANK];
	physicalAddress[ROW] = physicalAddr[ROW];
	physicalAddress[COLUMN] = physicalAddr[COLUMN];
}

BusPacket::BusPacket(BusPacket* cmd)
{
	command = cmd->command;
	requestorID = cmd->requestorID;
	address = cmd->address;
	data = cmd->data;
}

BusPacket::~BusPacket()
{
	// TODO Auto-generated destructor stub
}

} /* namespace MCsimulator */
