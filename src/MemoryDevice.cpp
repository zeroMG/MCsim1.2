/*
 * MemoryDevice.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#include "MemoryDevice.h"

namespace MCsimulator
{

MemoryDevice::MemoryDevice()
{
	// TODO Auto-generated constructor stub
	channel = 0;
	clock = 0;
}

MemoryDevice::~MemoryDevice()
{
	// TODO Auto-generated destructor stub
}

void MemoryDevice::SetChannel(int channel)
{
	this->channel = channel;
}

void MemoryDevice::ReceiveData(BusPacket* data)
{
	return;
}
void MemoryDevice::RetrieveData(BusPacket* data)
{
	return;
}


void MemoryDevice::Tick(BusPacket* scheduledCommand)
{
	if (scheduledCommand != NULL)
	{
		ReceiveCommand(scheduledCommand);
	}
	else
	{
		//PRINT("++ DRAM: NONE");
	}

}
} /* namespace MCsimulator */
