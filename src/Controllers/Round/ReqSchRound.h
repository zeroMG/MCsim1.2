/*
 * ReqSchORP.h
 *
 *  Created on: Feb 11, 2018
 *      Author: dlguo
 */

#ifndef REQUESTSCHEDULER_REQSCHROUND_H_
#define REQUESTSCHEDULER_REQSCHROUND_H_

#include "RequestScheduler.h"

#include <map>

namespace MCsimulator
{

class ReqSch_Round: public RequestScheduler
{
public:
	ReqSch_Round(int channel);
	virtual ~ReqSch_Round();
protected:
	void ScheduleRequest(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer);
};

} /* namespace MCsimulator */

#endif /* REQUESTSCHEDULER_REQSCHORP_H_ */
