/*
 * CmdGenORP.h
 *
 *  Created on: Feb 11, 2018
 *      Author: dlguo
 */

#ifndef COMMANDGENERATOR_CMDGENROUND_H_
#define COMMANDGENERATOR_CMDGENROUND_H_

#include "CommandGenerator.h"

namespace MCsimulator
{

class CmdGen_Round: public CommandGenerator
{
public:
	CmdGen_Round(int channel);
	virtual ~CmdGen_Round();
private:
	void GenerateCommand(Request* request, std::vector<BusPacket*>* const generatedCommandBuffer);
};

} /* namespace MCsimulator */

#endif /* COMMANDGENERATOR_CMDGENORP_H_ */
