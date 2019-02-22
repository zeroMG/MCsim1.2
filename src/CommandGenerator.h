/*
 * CommandGenerator.h
 *
 *  Created on: Dec 1, 2017
 *      Author: dlguo
 */

#ifndef COMMANDGENERATOR_H_
#define COMMANDGENERATOR_H_

#include "Request.h"
#include "BusPacket.h"

#include <vector>

namespace MCsimulator
{

class CommandGenerator
{
public:
	CommandGenerator(int channel);//bool interleavedMemory
	virtual ~CommandGenerator();
	void Tick(std::vector<Request*>* const scheduledRequestBuffer,
			std::vector<BusPacket*>* const generatedCommandBuffer);
protected:
	virtual void GenerateCommand(Request* request,
			std::vector<BusPacket*>* const generatedCommandBuffer);
	int channel;
};

} /* namespace MCsimulator */

#endif /* COMMANDGENERATOR_H_ */
