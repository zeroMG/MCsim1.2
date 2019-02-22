/*
 * CmdSchORP.h
 *
 *  Created on: Feb 11, 2018
 *      Author: dlguo
 */

#ifndef COMMANDSCHEDULER_CMDSCHROUND_H_
#define COMMANDSCHEDULER_CMDSCHROUND_H_


#include "CommandScheduler.h"

namespace MCsimulator
{

class CmdSch_Round: public CommandScheduler
{
public:
	CmdSch_Round(int channel);
	virtual ~CmdSch_Round();
private:
	void ScheduleCommand(SystemQueue<BusPacket>* commandQueues, BusPacket* &scheduledCommand);
	// Pending Command indicator based on id
	std::map<uint32_t, bool> queuePending;
	std::vector<uint32_t> REQ_FIFO;
	std::vector<BusPacket*> CMD_FIFO;
	std::vector<unsigned int> Order;
		int  servicebuffer[16];
		int  consideredScheduled[16];
		// Pending Command indicator based on requestorID
		std::map<unsigned int, BusPacket*> tempqueue;
		bool roundType; // true = Read and false = write
		bool skipCAS;
		bool cycleDone;
		bool currRound;
		int oppslot;
		int lastACT;
		int lastCAS;
		int countWOPP;
		int countROPP;
		unsigned int expectCAS;
		unsigned int countFAW;
		unsigned int countACT;
		unsigned int countCAS;
		int counter;
		int swCAS;
		int swACT;
		bool blockACT;
		bool jump_1;
		bool jump_2;
		bool jump_3;
		bool jump_4;
		bool jump_5;
		bool jump_6;
		bool jump_7;
		bool first;
		bool BypassReset;
		int tCCD;
		int tRTR;
		int tRCD;
		int tFAW;
		int tRRD;
		int tRTW;
		int tWL;
		int tWTR;
		int tBUS;
		int tRP;
		int tWtoR;
		BusPacket* checkCommand;
		BusPacket* checkCommand_1;
		bool count_ACT;
		bool count_CAS;
		long long int close_read_count;
		long long int close_write_count;
		long long int open_read_count;
		long long int open_write_count;
};

} /* namespace MCsimulator */

#endif /* COMMANDSCHEDULER_CMDSCHORP_H_ */
