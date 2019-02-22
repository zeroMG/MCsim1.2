/*
 * MemoryDevice.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef MEMORYDEVICE_H_
#define MEMORYDEVICE_H_

#include "BusPacket.h"

namespace MCsimulator
{

class MemoryDevice
{
public:
	MemoryDevice();
	virtual ~MemoryDevice();
	virtual int GetDeviceSize() = 0;
	void SetChannel(int channel);
	void ReceiveData(BusPacket* data);
	void RetrieveData(BusPacket* data);
	void Tick(BusPacket* scheduledCommand);
protected:
	virtual void ReceiveCommand(BusPacket* command) = 0;;
	virtual void BuildDeviceTable() = 0;
	// virtual void BuildDeviceSpecificTable() = 0;
	int channel;
	long clock;

};

} /* namespace MCsimulator */

#endif /* MEMORYDEVICE_H_ */
