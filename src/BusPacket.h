/*
 * BusPacket.h
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#ifndef BUSPACKET_H_
#define BUSPACKET_H_

#include "Global.h"

namespace MCsimulator
{

	class BusPacket
	{
	public:
		BusPacket(DRAM_Packet packetType, int id, int addr, int physicalAddr[TOTAL_LEVEL], void *data);
		BusPacket(BusPacket* cmd);
		virtual ~BusPacket();
		DRAM_Packet command;
		int requestorID;
		int address;
		int physicalAddress[TOTAL_LEVEL];
		void* data;
	};

} /* namespace MCsimulator */

#endif /* BUSPACKET_H_ */
