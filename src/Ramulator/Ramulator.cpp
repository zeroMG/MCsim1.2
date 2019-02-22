/*
 * Ramulator.cpp
 *
 *  Created on: Feb 3, 2018
 *      Author: dlguo
 */

// Define the device specific parameters


#include "Ramulator.h"

namespace MCsimulator
{

template <>
void Ramulator<DDR3>::BuildDeviceSpecificTable()
{
	memoryTimingTable[channel][tRRD]	= ramulator->spec->speed_entry.nRRD;
	memoryTimingTable[channel][tWTR]	= ramulator->spec->speed_entry.nWTR;
}

template <>
void Ramulator<DDR4>::BuildDeviceSpecificTable()
{
//	memoryTimingTable[channel][tRRD]	= ramulator->spec->speed_entry.nRRD;
//	memoryTimingTable[channel][tWTR]	= ramulator->spec->speed_entry.nWTR;
}

}

//// Backup Functions
//template <>
//void Ramulator<DDR3>::TranslateCommand(BusPacket* cmd)
//{
//
//}
//
//template <>
//void Ramulator<DDR3>::TranslateAddress(BusPacket* cmd)
//{
//
//}
//
//template <>
//void Ramulator<DDR4>::TranslateCommand(BusPacket* cmd)
//{
//
//}
//
//template <>
//void Ramulator<DDR4>::TranslateAddress(BusPacket* cmd)
//{
//
//}

