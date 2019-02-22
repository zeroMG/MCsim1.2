

#ifndef RAMULATOR_H
#define RAMULATOR_H

#include "../MemoryDevice.h"
#include "../Global.h"
//#include "../BusPacket.h"


#include "DRAM.h"
#include "DDR3.h"
#include "DDR4.h"
// #include "Ramulator/DSARP.h"
// #include "Ramulator/GDDR5.h"
// #include "Ramulator/LPDDR3.h"
// #include "Ramulator/LPDDR4.h"
// #include "Ramulator/WideIO.h"
// #include "Ramulator/WideIO2.h"
// #include "Ramulator/HBM.h"
// #include "Ramulator/SALP.h"
// #include "Ramulator/ALDRAM.h"
// #include "Ramulator/TLDRAM.h"

using namespace ramulator;
namespace MCsimulator
{
template <typename T>
class Ramulator: public MemoryDevice
{
public:
	Ramulator( T* spec, int rank );
	int GetDeviceSize()
	{
		return ramulator->spec->org_entry.size;
	}
private:
	DRAM<T>* ramulator;
	typename T::Command ramType;
	std::vector<int> ramAddr;
	void ReceiveCommand(BusPacket* command);
	void BuildDeviceTable();
	void BuildDeviceSpecificTable();
	void TranslateCommand(BusPacket* command);
	void TranslateAddress(BusPacket* command);
};

template <typename T>
Ramulator<T>::Ramulator( T* spec, int rank ): MemoryDevice()
{
	ramulator = new DRAM<T>(spec, T::Level::Channel);
	BuildDeviceTable();
	BuildDeviceSpecificTable();
}

template <typename T>
void Ramulator<T>::ReceiveCommand(BusPacket* command)
{
	//PRINT("++ DRAM: CMD "<<command->command);
	// Working connection
	if (ramulator->check(ramType, ramAddr.data(), clock))
	{
			ramulator->update(ramType, ramAddr.data(), clock);
	}
	else
	{
		ERROR("Violate Ramulator Schedule Check")
	}
}

template <typename T>
void Ramulator<T>::BuildDeviceTable()
{
	/// Build Memory Timing Constraint Table
	memoryTimingTable[channel][tBUS]	= ramulator->spec->speed_entry.nBL;
	memoryTimingTable[channel][tCLK]	= ramulator->spec->speed_entry.tCK;
	/// Intra-Bank Timings
	memoryTimingTable[channel][tRC]	= ramulator->spec->speed_entry.nRC;
	memoryTimingTable[channel][tRAS]	= ramulator->spec->speed_entry.nRAS;
	memoryTimingTable[channel][tWR]	= ramulator->spec->speed_entry.nWR;
	memoryTimingTable[channel][tRP] 	= ramulator->spec->speed_entry.nRP;
	memoryTimingTable[channel][tRCD]	= ramulator->spec->speed_entry.nRCD;
	memoryTimingTable[channel][tRL]	= ramulator->spec->speed_entry.nCL;
	memoryTimingTable[channel][tWL]	= ramulator->spec->speed_entry.nCWL;
	memoryTimingTable[channel][tRTP]	= ramulator->spec->speed_entry.nRTP;
	// Inter-Bank Timings
	// memoryTimingTable[channel][tRRD]	= ramulator->spec->speed_entry.nRRD;
	memoryTimingTable[channel][tFAW]	= ramulator->spec->speed_entry.nFAW;
	// General Timings
	memoryTimingTable[channel][tRTW]	= ramulator->spec->speed_entry.nCL + ramulator->spec->speed_entry.nBL +
					  2 - ramulator->spec->speed_entry.nCWL;
//	memoryTimingTable[channel][tWTR]	= ramulator->spec->speed_entry.nWTR;
	memoryTimingTable[channel][tCCD]	= ramulator->spec->speed_entry.nBL;
	memoryTimingTable[channel][tBUS]	= ramulator->spec->speed_entry.nBL;
	memoryTimingTable[channel][tREFI]	= ramulator->spec->speed_entry.nREFI;
	memoryTimingTable[channel][tRFC]	= ramulator->spec->speed_entry.nRFC;

	memoryLevelTable[channel][DRAM_Level::RANK] = 1;//device->spec->GetRank();
	memoryLevelTable[channel][DRAM_Level::BANKGROUP] = 1;
	memoryLevelTable[channel][DRAM_Level::BANK] = ramulator->spec->org_entry.count[int(T::Level::Bank)];
	memoryLevelTable[channel][DRAM_Level::ROW] = ramulator->spec->org_entry.count[int(T::Level::Row)];
	memoryLevelTable[channel][DRAM_Level::COLUMN] = ramulator->spec->org_entry.count[int(T::Level::Column)];
	memoryLevelTable[channel][DRAM_Level::DATABUS] = ramulator->spec->org_entry.dq * memoryTimingTable[channel][tBUS] * 2; ///< DataBusWidth ==> *8

	if(int(T::Level::Bank) != int(T::Level::Rank)+1)
	{
		memoryLevelTable[channel][DRAM_Level::BANKGROUP] = ramulator->spec->org_entry.count[int(T::Level::Bank)-1];	///< BankGroup
	}
	for(int index = 0; index < int(T::Level::MAX); index++)
	{
		ramAddr.push_back(-1);
	}
}

// template <typename T>
// void Ramulator<T>::BuildDeviceSpecificTable()
// {
// 	return;
// }

template <typename T>
void Ramulator<T>::TranslateCommand(BusPacket* command)
{
	switch ( command->command )
	{
	case ACT_R:
		ramType = T::Command::ACT_R;
		break;
	case ACT_W:
		ramType = T::Command::ACT_W;
		break;	
	case RD:
		ramType = T::Command::RD;
		break;
	case RDA:
		ramType = T::Command::RDA;
		break;
	case WR:
		ramType = T::Command::WR;
		break;
	case WRA:
		ramType = T::Command::WRA;
		break;
	case PRE:
		ramType = T::Command::PRE;
		break;
	case PREA:
		ramType = T::Command::PREA;
		break;
	case REF:
		ramType = T::Command::REF;
		break;
	default:
		ERROR("Channel %d : NOT A SUPPORTED COMMAND");
		abort();
	}
	return;
}

template <typename T>
void Ramulator<T>::TranslateAddress(BusPacket* command)
{
	ramAddr[int(T::Level::Channel)] = 0;
	ramAddr[int(T::Level::Rank)] = command->physicalAddress[RANK];
	if(int(T::Level::Bank) == int(T::Level::Rank)+1)
	{
		ramAddr[int(T::Level::Bank)] = command->physicalAddress[BANK];
	}
	else
	{
		ramAddr[int(T::Level::Bank)-1] = command->physicalAddress[BANKGROUP];
		ramAddr[int(T::Level::Bank)] = command->physicalAddress[BANK];
	}
	ramAddr[int(T::Level::Row)] = command->physicalAddress[ROW];
	ramAddr[int(T::Level::Column)] = command->physicalAddress[COLUMN];
}
}

#endif
