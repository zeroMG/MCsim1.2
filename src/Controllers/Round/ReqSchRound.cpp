/*
 * ReqSchORP.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: dlguo
 */

#include "ReqSchRound.h"

namespace MCsimulator
{

ReqSch_Round::ReqSch_Round(int channel): RequestScheduler(channel)
{
	// TODO Auto-generated constructor stub
}

ReqSch_Round::~ReqSch_Round()
{
	// TODO Auto-generated destructor stub
}

void ReqSch_Round::ScheduleRequest(SystemQueue<Request>* requestQueue, SystemQueue<BusPacket>* commandQueue, std::vector<Request*>* scheduledRequestBuffer)
{
	for (std::map<int,int>::iterator requestor = requestorPriorityTable.begin(); requestor != requestorPriorityTable.end(); requestor++)
	{
		/// <RequestorID, DRAM_Level, requestIndex>: 0 = Channel
		Request* checkRequest = requestQueue->GetRequestorElement(requestor->first, 0, 0);
		if ( checkRequest != NULL)
		{
			scheduledRequestBuffer->push_back(checkRequest);
			requestQueue->RemoveRequestorElement(requestor->first, 0, 0);
		}
	}
}

} /* namespace MCsimulator */
