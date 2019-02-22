/*
 * Request.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#include "Request.h"

namespace MCsimulator
{

Request::Request(int id, long address, bool type, int size, void* data):
		requestorID(id),
		address(address),
		size(size),
		data(data)
{
	// TODO Auto-generated constructor stub
	if (type == 1)
	{
		Request::type = READ_DATA;
	}
	else
	{
		Request::type = WRITE_DATA;
	}
}

Request::Request(BusPacket* returnData)
{
	requestorID = returnData->requestorID;
	address = returnData->address;
	size = 1;
	data = returnData->data;
	Request::type = RETURN_DATA;
}

Request::~Request()
{
	// TODO Auto-generated destructor stub
}

} /* namespace MCsimulator */
