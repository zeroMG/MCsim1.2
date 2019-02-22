/*
 * MCsim.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef MCSIM_H_
#define MCSIM_H_

#include <string>

namespace MCsimulator
{
	class MCsim
	{
	public:
		/* 
		 * The top level container for single-channel memory controllers
		 * the controllerMapping file contains controller configuration for each channel
		 * channels = line in controllerMapping
		 */ 
		MCsim(unsigned channels, std::string controllerMapping);
		virtual ~MCsim();

		/*
		 * The simulation clock. Tick each controller
		 */
		void Tick( void );

		// Register Requestors
		void RegisterRequestor(int requestorID, int priority);
		// access: 1=RD, 0=WR, size can depends on the data bus size, reject request is size is greater than bus size
		void AddRequest(int requestorID, int address, bool access, int size, void* data);
		// Requestor should retrieve memory requests manually every simulation cycle
		bool ReturnRequest(int channel, int& requestor, int& address, bool& access, void** data);

	private:
		unsigned long clock;
		void BuildDRAMSystem(int channel, std::string systemConnection);
		void DecodeDRAMDevice(std::string memoryDeviceConfig, std::string& typeKey,
				int& rank, std::string& deviceGeneration, std::string& deviceSpeed, std::string& deviceOrganization);
	};
} /* namespace MCsimulator */

#endif /* MCSIM_H_ */
