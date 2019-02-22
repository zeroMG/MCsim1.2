/*
 * Global.h
 *
 *  Created on: Nov 26, 2017
 *      Author: dlguo
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <iostream>
#include <map>
#include <vector>

#define ERROR(str) std::cout<<"[ERROR ("<<__FILE__<<":"<<__LINE__<<")]: "<< str <<std::endl; exit(1);

#ifdef ENABLE_PRINT
	#define PRINT(str) std::cout<< str <<std::endl;
#else
	#define PRINT(str)
#endif


/*
 * 	Usage case: If a new device is developed, the cmd and time can be add to
 *       the global definition, for any devices that do not use these new properties
 *       the default value is set 0.
 */

namespace MCsimulator
{
enum DRAM_Request
{
	READ_DATA,
	WRITE_DATA,
	RETURN_DATA
};

enum DRAM_Packet
{
	RD, RDA, WR, WRA,		// CAS access
	ACT_R, ACT_W, PRE, PREA,			// Open/Close operation
	REF,					// Refresh
	PDE, PDX, SRE, SRX,		// Power Related Command
	DATA,
	TOTAL_TYPE
};

enum DRAM_Timing
{
	tCLK,
	tRC, tRAS, tWR, tRP, tRCD, tRL, tWL, tRTP,  ///<  Intra-Bank Constraints
	tRRD, tFAW,                                 ///<  Inter-Bank Constraints
	tRTR, tRTW, tWTR, tCCD, tBUS,               ///<  General Constraints
	tREFI, tRFC,
	TOTAL_TIMING
};

enum DRAM_Level
{
	CHANNEL, RANK, BANKGROUP, BANK, ROW, COLUMN, DATABUS,
	TOTAL_LEVEL
};

enum DRAM_Device // Type_Rank_Generation_Speed_Organization = Ramulator_DDR3_1333H_2G_8
{
	TYPE_KEY,
	RANK_NUMBER,
	DEVICE_GENERATION,
	DEVICE_SPEED,
	DEVICE_SIZE,
	DEVICE_CHIP
};

extern std::map< int, int[TOTAL_LEVEL] > memoryLevelTable;	 // <channel, memoryLevelArray>

//	If the timing constraint is not available for a device, shown as 0
//
//
extern std::map< int, int[TOTAL_TIMING] > memoryTimingTable; // <channel, timingConstraintArray>
extern std::map< int, std::vector< int > >  bankOpenRowTable; // <channel, bankVector> // -1 indicates close bank
extern std::map< int, int > requestorPriorityTable;
//
//
//extern std::vector<Request*> completeReadBuffer;
//extern std::vector<Request*> completeWriteBuffer;
}



#endif /* GLOBAL_H_ */
