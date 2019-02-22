/*
 * RequestScheduler.h
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#ifndef REQUESTSCHEDULER_H_
#define REQUESTSCHEDULER_H_

#include "SystemQueue.h"

#include <vector>

namespace MCsimulator
{

class RequestScheduler
{
public:
	RequestScheduler(int channel);
	virtual ~RequestScheduler();
	void Tick(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer);
protected:
	virtual bool IsSchedulable(SystemQueue<BusPacket>* commandQueue);
	virtual void ScheduleRequest(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer);
	int channel;
};

} /* namespace MCsimulator */

#endif /* REQUESTSCHEDULER_H_ */
