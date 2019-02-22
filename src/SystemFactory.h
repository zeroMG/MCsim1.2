/*
 * SystemFactory.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef SYSTEMFACTORY_H_
#define SYSTEMFACTORY_H_

#include <string>

#include "RequestScheduler.h"
#include "CommandGenerator.h"
#include "CommandScheduler.h"
#include "MemoryDevice.h"

namespace MCsimulator
{
	class SystemFactory
	{
	public:
		static SystemFactory* GetInstance();
		RequestScheduler* GetRequestScheduler(int channel, std::string key);
		CommandGenerator* GetCommandGenerator(int channel, std::string key);
		CommandScheduler* GetCommandScheduler(int channel, std::string key);
		MemoryDevice* GetMemoryDevice(int channel, std::string key,
				int rank, std::string generation, std::string speed, std::string orgnization);

	private:
		SystemFactory();
		virtual ~SystemFactory();
		static SystemFactory* factory;
	};
} /* namespace MCsimulator */

#endif /* SYSTEMFACTORY_H_ */
