/*
 * AddressTranslator.cpp
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#include "AddressTranslator.h"
using namespace std;
namespace MCsimulator
{

AddressTranslator::AddressTranslator(std::string AddressMapping)
{
	// TODO Auto-generated constructor stub
	std::string mappingScheme = AddressMapping;
	size_t index = 0;
	std::string level = "";
	while( true )
	{
		index = mappingScheme.find_first_of(":");
		if (index != std::string::npos)
		{
			level = mappingScheme.substr(0, index);
			mappingScheme = mappingScheme.substr(index+1, mappingScheme.size()-index);
		}
		else if (!mappingScheme.empty())
		{
			level = mappingScheme;
			mappingScheme.clear();
		}
		else
		{
			// Do Nothing
		}
		BuildMappingScheme(level);
		if (mappingScheme.empty())
		{
			break;
		}
	}
}

AddressTranslator::~AddressTranslator()
{
	// TODO Auto-generated destructor stub
}

void AddressTranslator::TranslateAddress(Request* request)
{
	if (!mappingOrder.empty())
	{
		int tmpAddressA = request->address;
		int tmpAddressB = request->address;
		for (unsigned index = 0; index < mappingOrder.size(); index++ )
		{
			int tmpAddress = 0;
			if (memoryLevelTable[request->physicalAddress[DRAM_Level::CHANNEL]][mappingOrder[index]] > 0)
			{
				int logIndex = Log2(memoryLevelTable[request->physicalAddress[DRAM_Level::CHANNEL]][mappingOrder[index]]);
				tmpAddressA = tmpAddressA >>logIndex;
				tmpAddressA = tmpAddressA <<logIndex;
				tmpAddress = tmpAddressA ^ tmpAddressB;
				tmpAddressB = tmpAddressA >> logIndex;
				tmpAddressA = tmpAddressB;
			}
			request->physicalAddress[mappingOrder[index]] = tmpAddress;
		}
#ifdef SIMULATE_SINGLE_BANK_PRIVATIZATION
		request->physicalAddress[DRAM_Level::BANK] = request->requestorID;
#endif

		PRINT("++ AddrTrans: Rank "<<request->physicalAddress[DRAM_Level::RANK]<<" BankGroup "<<request->physicalAddress[DRAM_Level::BANKGROUP]
			<<" Bank "<<request->physicalAddress[DRAM_Level::BANK]<<" Row "<<request->physicalAddress[DRAM_Level::ROW]<<" Column "
			<<request->physicalAddress[DRAM_Level::COLUMN])
	}
	else
	{
		ERROR("No Mapping Scheme is Built");
	}
}

void AddressTranslator::Tick( std::vector<Request*> incomingRequestBuffer )
{
	if (!incomingRequestBuffer.empty())
	{
		for (unsigned index = 0; index < incomingRequestBuffer.size(); index++)
		{
			TranslateAddress(incomingRequestBuffer[index]);
		}
	}
}

void AddressTranslator::BuildMappingScheme(std::string level)
{
	cout<<"in hast:  "<<level<<endl;
	if (level == "RK")
	{
		mappingOrder.push_back(DRAM_Level::RANK);
	}
	else if (level == "BG")
	{
		mappingOrder.push_back(DRAM_Level::BANKGROUP);
	}
	else if (level == "BK")
	{
		mappingOrder.push_back(DRAM_Level::BANK);
	}
	else if (level == "RW")
	{
		mappingOrder.push_back(DRAM_Level::ROW);
	}
	else if (level == "CL")
	{
		mappingOrder.push_back(DRAM_Level::COLUMN);
	}
	else
	{
		ERROR("No Such DRAM Level");
	}
}

// Convert decimal level into binary index
int AddressTranslator::Log2(int level)
{
	int logbase2 = 0;
	int value = level;
	int orig = value;
	value>>=1;
	while (value>0)
	{
		value >>= 1;
		logbase2++;
	}
	if ((int)1<<logbase2<orig)
	{
		logbase2++;
	}
	return logbase2;
}

} /* namespace MCsimulator */
