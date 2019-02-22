/*
 * CommandScheduler.h
 *
 *  Created on: Dec 7, 2017
 *      Author: dlguo
 */

#ifndef COMMANDSCHEDULER_H_
#define COMMANDSCHEDULER_H_

#include "SystemQueue.h"
#include "BusPacket.h"

#include <map>
#include <vector>

namespace MCsimulator
{

class CommandScheduler
{
public:
	CommandScheduler(int channel);
	virtual ~CommandScheduler();
	void Tick(SystemQueue<BusPacket>* commandQueues, BusPacket* &scheduledCommand);
protected:
	int channel;
	virtual bool IsIssuable(BusPacket* command);
	virtual void ScheduleCommand(SystemQueue<BusPacket>* commandQueues, BusPacket* &scheduledCommand) = 0;
private:
	std::vector<std::map<int,int> > channelTimeTable;  // flatten the bank level timeTable
	std::map< int, std::vector<int> > channelACTTable;	// per rank level
	int GetTimeDelay(int delay, int constraint);
	void TickTimeTable();
	void UpdateTimingTable(BusPacket* command);
	void UpdateInterRank(unsigned rankMin, unsigned rankMax, int type);
	void UpdateIntraRankInterBankGroup(unsigned rankMin, unsigned rankMax, unsigned bkGpMin, unsigned bkGpMax, int type);
	void UpdateIntraRankIntraBankGroup(unsigned min, unsigned max, unsigned index, int type, unsigned rank);
	void RefreshAllBanks();
};

} /* namespace MCsimulator */

#endif /* COMMANDSCHEDULER_H_ */
