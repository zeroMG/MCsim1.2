/*
 * MemoryTimingTable.h
 *
 *  Created on: Dec 10, 2018
 *      Author: dlguo
 */

#ifndef MEMORYTIMINGTABLE_H_
#define MEMORYTIMINGTABLE_H_

#include <map>
#include <vector>

#include "Global.h"

namespace MCsimulator
{
	class MemoryTimingTable
	{
	public:
		static MemoryTimingTable* getInstance()
        {
            static MemoryTimingTable* instance = new MemoryTimingTable();
            return instance;
        }
		
        /*
         * set and get clock 
         */
        bool setClock(unsigned channel, double clk);
        double getClock(unsigned channel);

        /*
         * @brief   : Called by DRAM device simulator to update device timing table
         * @return  : Return false if the constraint is not valid
         */ 
		bool setTimingConstraint(unsigned channel, unsigned timing_param, unsigned timing_constraint);

        /*
         * @brief   : Retrieve the timing constraint
         * @return  : Return the timing constraint based on the parameter
         */         
        unsigned getTimingConstraint(unsigned channel, unsigned timing_param);

	private:
        MemoryTimingTable();
        virtual ~MemoryTimingTable();

        struct MemoryTimingParamContainer
        {
            MemoryTimingParamContainer()
                : tCLK(0.0)
                , tRC(0)
                , tRAS(0)
                , tWR(0)
                , tRP(0)
                , tRCD(0)
                , tRL(0)
                , tWL(0)
                , tRTP(0)
                , tRRD(0)
                , tFAW(0)
                , tRTR(0)
                , tRTW(0)
                , tWTR(0)
                , tCCD(0)
                , tBUS(0)
                , tREFI(0)
                , tRFC(0){} 

            double tCLK;
            unsigned tRC;
            unsigned tRAS;
            unsigned tWR;
            unsigned tRP;
            unsigned tRCD;
            unsigned tRL;
            unsigned tWL;
            unsigned tRTP;
            unsigned tRRD;
            unsigned tFAW;
            unsigned tRTR;
            unsigned tRTW;
            unsigned tWTR;
            unsigned tCCD;
            unsigned tBUS;
            unsigned tREFI;
            unsigned tRFC;
        };
        std::vector< MemoryTimingParamContainer* > channelMemoryTimingParamTable;
	};

} /* namespace MCsimulator */

#endif /* MEMORYTIMINGTABLE_H_ */
