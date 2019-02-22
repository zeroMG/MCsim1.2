/*
 * BankOpenRowTable.cpp
 *
 *  Created on: Dec 10, 2018
 *      Author: dlguo
 */

#include "BankOpenRowTable.h"
#include "Global.h"

namespace MCsimulator
{
BankOpenRowTable::BankOpenRowTable()
{
    if ( !memoryLevelTable.empty() )
    {
        for (auto it = memoryLevelTable.begin(); it != memoryLevelTable.end(); it++)
        {
            int ch = it->first;
            bankCurrentOpenRowTable[ch] = std::vector<int>();
            bankPendingOpenRowTable[ch] = std::vector<int>();
            
            int totalBankIndex = memoryLevelTable[ch][RANK]* 
                                    memoryLevelTable[ch][BANKGROUP]*
                                    memoryLevelTable[ch][BANK];
            for (int bk = 0; bk < totalBankIndex; bk++)
            {
                bankCurrentOpenRowTable[ch].push_back(-1);
                bankPendingOpenRowTable[ch].push_back(-1);
            }
        }
    }
}

BankOpenRowTable::~BankOpenRowTable()
{
    // do nothing
    return;
}

int BankOpenRowTable::currentOpenRow(int channel, int rank, int bankGroup, int bank)
{
    int openRow = -1;
    if (bankCurrentOpenRowTable.find(channel) != bankCurrentOpenRowTable.end())
    {
        int bankIndex = memoryLevelTable[channel][BANK] * memoryLevelTable[channel][BANKGROUP]* rank +
                   memoryLevelTable[channel][BANK] * bankGroup + bank;
        openRow = bankCurrentOpenRowTable[channel][bankIndex];
    }
    return openRow;
}
     
int BankOpenRowTable::pendingOpenRow(int channel, int rank, int bankGroup, int bank)
{
    int openRow = -1;
    if (bankPendingOpenRowTable.find(channel) != bankPendingOpenRowTable.end())
    {
        int bankIndex = memoryLevelTable[channel][BANK] * memoryLevelTable[channel][BANKGROUP]* rank +
                   memoryLevelTable[channel][BANK] * bankGroup + bank;
        openRow = bankPendingOpenRowTable[channel][bankIndex];
    }
    return openRow;
}

bool BankOpenRowTable::updateCurrentOpenRow(int channel, int rank, int bankGroup, int bank, int row)
{
    bool rowUpdated = false;
    if (bankCurrentOpenRowTable.find(channel) != bankCurrentOpenRowTable.end())
    {
        int bankIndex = memoryLevelTable[channel][BANK] * memoryLevelTable[channel][BANKGROUP]* rank +
                   memoryLevelTable[channel][BANK] * bankGroup + bank;
        bankCurrentOpenRowTable[channel][bankIndex] = row;
        rowUpdated = true;
    }
    return rowUpdated;
}

bool BankOpenRowTable::updatePendingOpenRow(int channel, int rank, int bankGroup, int bank, int row)
{
    bool rowUpdated = false;
    if (bankPendingOpenRowTable.find(channel) != bankPendingOpenRowTable.end())
    {
        int bankIndex = memoryLevelTable[channel][BANK] * memoryLevelTable[channel][BANKGROUP]* rank +
                   memoryLevelTable[channel][BANK] * bankGroup + bank;
        bankPendingOpenRowTable[channel][bankIndex] = row;
        rowUpdated = true;
    }   
    return rowUpdated;
}



} /* namespace MCsimulator */
