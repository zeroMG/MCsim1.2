/*
 * SystemFactory.cpp
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#include "SystemFactory.h"

//#include "RequestScheduler/ReqSchRTMem.h"
//#include "RequestScheduler/ReqSchMEDUSA.h"
//#include "CommandGenerator/ClosePageCommandGenerator.h"
//#include "CommandScheduler/ChannelFCFS.h"
//#include "CommandScheduler/CmdSchRTMem.h"


// Ramulator
#include "Ramulator/Ramulator.h"
#include "Ramulator/DRAM.h"
#include "Ramulator/DDR3.h"
#include "Ramulator/DDR4.h"

// AMC
//#include "Controllers/AMC/ReqSchAMC.h"
//#include "Controllers/AMC/CmdSchAMC.h"

// ORP
// #include "Controllers/ORP/CmdSchORP.h"
// #include "Controllers/ORP/ReqSchORP.h"
#include "Controllers/Round/ReqSchRound.h"
#include "Controllers/Round/CmdGenRound.h"
#include "Controllers/Round/CmdSchRound.h"

// DCmc
//#include "Controllers/DCmc/CmdSchDCmc.h"

// Reorder
//#include "Controllers/ReOrder/CmdSchReorder.h"

// Bundle
//#include "Controllers/ReqBundle/CmdGenBundle.h"
//#include "Controllers/ReqBundle/CmdSchBundle.h"

#include "Global.h"

namespace MCsimulator
{

SystemFactory::SystemFactory()
{
	// TODO Auto-generated constructor stub
}

SystemFactory::~SystemFactory()
{
	// TODO Auto-generated destructor stub
}

SystemFactory* SystemFactory::GetInstance()
{
	if ( !factory )
	{
		factory = new SystemFactory;
	}
	return factory;
}

RequestScheduler* SystemFactory::GetRequestScheduler(int channel, std::string key)
{
	RequestScheduler* tmpScheduler = NULL;
	if (key == "Round")
	{
		tmpScheduler = new ReqSch_Round(channel);
	}
	else
	{
		ERROR("Invalid Request Scheduler");
	}
	return tmpScheduler;
}
CommandGenerator* SystemFactory::GetCommandGenerator(int channel, std::string key)
{
	CommandGenerator* tmpGenerator = NULL;
	size_t index = key.find_first_of("+");
	if (index != std::string::npos)
	{
		string generator = key.substr(0, index);
		string interleaved = key.substr(index+1, key.size()-1);
		bool memoryInterleaved = false;
		if (interleaved == "Interleave")
		{
			memoryInterleaved = true;
		}
		else if (generator == "OpenPage")
		{
			tmpGenerator = new CmdGen_Round(channel);
		}
		else
		{
			ERROR("Invalid Command Generator");
		}
	}
	else
	{
		ERROR("Missing Command Generator Configuration");
	}
	return tmpGenerator;
}
CommandScheduler* SystemFactory::GetCommandScheduler(int channel, std::string key)
{
	CommandScheduler* tmpScheduler = NULL;
	if ( key == "Round" )
	{
		tmpScheduler = new CmdSch_Round(channel);
	}
	else
	{
		ERROR("Invalid Command Scheduler");
	}
	return tmpScheduler;
}

MemoryDevice* SystemFactory::GetMemoryDevice(int channel, std::string key,
		int rank, std::string generation, std::string speed, std::string orgnization)
{
	MemoryDevice* tmpDevice = NULL;
	if (key == "Ramulator")
	{
		string mem_size = generation + '_' + orgnization;
		string mem_speed = generation + '_' + speed;
		if (generation == "DDR3")
		{
			DDR3* ddr3 = new DDR3(mem_size, mem_speed);
			tmpDevice = new Ramulator<DDR3>(ddr3, rank);
		}
		else if ( generation == "DDR4" )
		{
			DDR4* ddr4 = new DDR4(mem_size, mem_speed);
			tmpDevice = new Ramulator<DDR4>(ddr4, rank);
		}
		else
		{

		}
	}
	else
	{
		ERROR("Invalid Memory Device");
	}
	return tmpDevice;
}






} /* namespace MCsimulator */
