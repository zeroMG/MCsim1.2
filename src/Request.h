/*
 * Request.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include "Global.h"
#include "BusPacket.h"

namespace MCsimulator
{
	class Request
	{
	public:
		Request(int id, long address, bool type, int size, void* data);
		Request(BusPacket* data);
		virtual ~Request();
		int requestorID;
		long address;
		DRAM_Request type;
		int size;
		void* data;
		int physicalAddress[TOTAL_LEVEL];
	};

} /* namespace MCsimulator */

#endif /* REQUEST_H_ */
