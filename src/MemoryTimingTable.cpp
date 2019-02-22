

#include "MemoryTimingTable.h"

namespace MCsimulator
{
MemoryTimingTable::MemoryTimingTable()
{
    if ( !memoryLevelTable.empty() )
    {
        for (auto it = memoryLevelTable.begin(); it != memoryLevelTable.end(); it++)
        {
            channelMemoryTimingParamTable.push_back(new MemoryTimingParamContainer());
        }
    }
}

MemoryTimingTable::~MemoryTimingTable()
{
    while(!channelMemoryTimingParamTable.empty())
    {
        MemoryTimingParamContainer* tmpContainer = channelMemoryTimingParamTable.back();
        channelMemoryTimingParamTable.pop_back();
        delete tmpContainer;
        tmpContainer = NULL;
    }
}

bool MemoryTimingTable::setClock(unsigned channel, double clk)
{
    if (channelMemoryTimingParamTable.size() > channel)
    {    
        channelMemoryTimingParamTable[channel]->tCLK = clk;
        return true;
    }
    else
    {
        return false;
    }
}

double MemoryTimingTable::getClock(unsigned channel)
{
    if (channelMemoryTimingParamTable.size() > channel)
    {
        return channelMemoryTimingParamTable[channel]->tCLK;
    }
    else
    {
        return 0;
    }
}

bool MemoryTimingTable::setTimingConstraint(unsigned channel, unsigned timing_param, unsigned timing_constraint)
{
    bool set = false;
    if (channelMemoryTimingParamTable.size() > channel)
    {
        set = true;
        switch(timing_param)
        {
            case DRAM_Timing::tRC:
                channelMemoryTimingParamTable[channel]->tRC = timing_constraint;
                break;
            case DRAM_Timing::tRAS:
                channelMemoryTimingParamTable[channel]->tRAS = timing_constraint;
                break;
            case DRAM_Timing::tWR:
                channelMemoryTimingParamTable[channel]->tWR = timing_constraint;
                break;
            case DRAM_Timing::tRP:
                channelMemoryTimingParamTable[channel]->tRP = timing_constraint;
                break;
            case DRAM_Timing::tRCD:
                channelMemoryTimingParamTable[channel]->tRCD = timing_constraint;
                break;
            case DRAM_Timing::tRL:
                channelMemoryTimingParamTable[channel]->tRL = timing_constraint;
                break;
            case DRAM_Timing::tWL:
                channelMemoryTimingParamTable[channel]->tWL = timing_constraint;
                break;
            case DRAM_Timing::tRTP:
                channelMemoryTimingParamTable[channel]->tRTP = timing_constraint;
                break;
            case DRAM_Timing::tRRD:
                channelMemoryTimingParamTable[channel]->tRRD = timing_constraint;
                break;
            case DRAM_Timing::tFAW:
                channelMemoryTimingParamTable[channel]->tFAW = timing_constraint;
                break;  
            case DRAM_Timing::tRTR:
                channelMemoryTimingParamTable[channel]->tRTR = timing_constraint;
                break;
            case DRAM_Timing::tRTW:
                channelMemoryTimingParamTable[channel]->tRTW = timing_constraint;
                break;
            case DRAM_Timing::tWTR:
                channelMemoryTimingParamTable[channel]->tWTR = timing_constraint;
                break;
            case DRAM_Timing::tCCD:
                channelMemoryTimingParamTable[channel]->tCCD = timing_constraint;
                break;
            case DRAM_Timing::tBUS:
                channelMemoryTimingParamTable[channel]->tBUS = timing_constraint;
                break;
            case DRAM_Timing::tREFI:
                channelMemoryTimingParamTable[channel]->tREFI = timing_constraint;
                break;
            case DRAM_Timing::tRFC:
                channelMemoryTimingParamTable[channel]->tRFC = timing_constraint;
                break;
            default:
                ERROR("Invalid DRAM Timing Parameter")
                abort();
        }
    }
    return set;
}

unsigned MemoryTimingTable::getTimingConstraint(unsigned channel, unsigned timing_param)
{
    unsigned constraint = 0;
    if (channelMemoryTimingParamTable.size() > channel)
    {
        switch(timing_param)
        {
            case DRAM_Timing::tRC:
                constraint = channelMemoryTimingParamTable[channel]->tRC;
                break;
            case DRAM_Timing::tRAS:
                constraint = channelMemoryTimingParamTable[channel]->tRAS;
                break;
            case DRAM_Timing::tWR:
                constraint = channelMemoryTimingParamTable[channel]->tWR;
                break;
            case DRAM_Timing::tRP:
                constraint = channelMemoryTimingParamTable[channel]->tRP;
                break;
            case DRAM_Timing::tRCD:
                constraint = channelMemoryTimingParamTable[channel]->tRCD;
                break;
            case DRAM_Timing::tRL:
                constraint = channelMemoryTimingParamTable[channel]->tRL;
                break;
            case DRAM_Timing::tWL:
                constraint = channelMemoryTimingParamTable[channel]->tWL;
                break;
            case DRAM_Timing::tRTP:
                constraint = channelMemoryTimingParamTable[channel]->tRTP;
                break;
            case DRAM_Timing::tRRD:
                constraint = channelMemoryTimingParamTable[channel]->tRRD;
                break;
            case DRAM_Timing::tFAW:
                constraint = channelMemoryTimingParamTable[channel]->tFAW;
                break;  
            case DRAM_Timing::tRTR:
                constraint = channelMemoryTimingParamTable[channel]->tRTR;
                break;
            case DRAM_Timing::tRTW:
                constraint = channelMemoryTimingParamTable[channel]->tRTW;
                break;
            case DRAM_Timing::tWTR:
                constraint = channelMemoryTimingParamTable[channel]->tWTR;
                break;
            case DRAM_Timing::tCCD:
                constraint = channelMemoryTimingParamTable[channel]->tCCD;
                break;
            case DRAM_Timing::tBUS:
                constraint = channelMemoryTimingParamTable[channel]->tBUS;
                break;
            case DRAM_Timing::tREFI:
                constraint = channelMemoryTimingParamTable[channel]->tREFI;
                break;
            case DRAM_Timing::tRFC:
                constraint = channelMemoryTimingParamTable[channel]->tRFC;
                break;
            default:
                ERROR("Invalid DRAM Timing Parameter")
                abort();
        }
    }
    return constraint;
}
}