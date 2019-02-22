/*
 * RequestScheduler.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#include "RequestScheduler.h"

namespace MCsimulator
{

RequestScheduler::RequestScheduler(int channel):
	channel(channel)
{
	// TODO Auto-generated constructor stub

}

RequestScheduler::~RequestScheduler()
{
	// TODO Auto-generated destructor stub
}

bool RequestScheduler::IsSchedulable(SystemQueue<BusPacket>* commandQueue)
{
	return true;
}

void RequestScheduler::ScheduleRequest(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer)
{
	return;
}

void RequestScheduler::Tick(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer)
{
	// Look at the status of both buffer system
	ScheduleRequest(requestQueue, commandQueue, scheduledRequestBuffer);
	//PRINT("++ ReqSch: Request Queue Size = "<<scheduledRequestBuffer->size());
}

} /* namespace MCsimulator */
