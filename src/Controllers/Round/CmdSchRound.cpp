	/*********************************************************************************
	*  Copyright (c) 2018-2019, 
	*  Reza Mirosanlou
	*  University of Waterloo               
	*  All rights reserved.
	*********************************************************************************/

	#include "CmdSchRound.h"
	#include <algorithm>    // std::max
	#include <math.h>       /* ceil */
	#include <cstdlib>
	using namespace std;
	namespace MCsimulator
	{
		


	CmdSch_Round::CmdSch_Round(int channel): CommandScheduler(channel)
	{
		//vector<pair<BusPacket*, unsigned int>> cmdFIFO;	// FIFO contains ready commands and queue index
		
		counter = 0;
		roundType = true; // true = Read and false = write
		skipCAS = false;
		cycleDone = false;
		currRound = 0;
		//first = true;
		lastACT = 0;
		countFAW = 0;
		oppslot = 0;
		//int check = 0;
		blockACT = false;
		swCAS = tCCD;
		
		for (unsigned int i = 0 ; i < 8 ; i++)
		{
			servicebuffer[i] = 0;
		}
		for (unsigned int i = 0; i < 8; i++)
		{
			consideredScheduled[i] = 0;
		}
		for (unsigned int i = 0; i < 8; i++)
		{
			Order.push_back(i);
		}
	}

	CmdSch_Round::~CmdSch_Round() 
	{
		tempqueue.clear();
		Order.clear();
		queuePending.clear();
	}

	void CmdSch_Round::ScheduleCommand(SystemQueue<BusPacket>* commandQueues, BusPacket* &scheduledCommand) 
	{
		
		//int check;
	
			
		
		tCCD = memoryTimingTable[channel][DRAM_Timing::tCCD];
		////////////cout<<"tCCD   "<<tCCD<<endl;
		tRRD = memoryTimingTable[channel][DRAM_Timing::tRRD];
		////////////cout<<"tRRD   "<<tRRD<<endl;
		tRCD = memoryTimingTable[channel][DRAM_Timing::tRCD];
		////////////cout<<"tRCD   "<<tRCD<<endl;
		tFAW = memoryTimingTable[channel][DRAM_Timing::tFAW];
		////////////cout<<"tFAW   "<<tFAW<<endl;
		tRTW = memoryTimingTable[channel][DRAM_Timing::tRTW];
		////////////cout<<"tRTW   "<<tRTW<<endl;
		tWL  = memoryTimingTable[channel][DRAM_Timing::tWL];
		////////////cout<<"tWL   "<<tWL<<endl;
		tWTR = memoryTimingTable[channel][DRAM_Timing::tWTR];
		////////////cout<<"tWTR   "<<tWTR<<endl;
		tBUS = memoryTimingTable[channel][DRAM_Timing::tBUS];
		tRP = memoryTimingTable[channel][DRAM_Timing::tRP];
		
		////////////cout<<"tBUS   "<<tBUS<<endl;
		tWtoR = tWL + tBUS + tWTR;
		checkCommand = NULL;
		checkCommand_1 = NULL;
		count_ACT = false;
		count_CAS = false;
		scheduledCommand = NULL;
		
		//Constructing the intra-ready tempqueue here!

					
		// PerRequestor is enabled and there is some requestors in this memory level
			
		for(unsigned int requestor = 0; requestor < requestorPriorityTable.size(); requestor++) // for all requestors from "num". getRequestorIndex() gives the number of requestors
		{
			if ( commandQueues->GetRequestorSize(requestor, 0) != 0 )
			{
				checkCommand = commandQueues->GetRequestorElement(requestor, 0,0);
				if(queuePending.find(checkCommand->requestorID) == queuePending.end()) 
				{										
					queuePending[checkCommand->requestorID] = false;										
				}
				if(queuePending[checkCommand->requestorID] == false)
				{
					if( commandQueues->IsQueueReady(0, checkCommand))
					{													
						tempqueue[checkCommand->requestorID]= checkCommand;														
						queuePending[checkCommand->requestorID] = true;
					}
				}
				checkCommand = NULL;
			}
		}

		if(counter == swACT)
		{
			//cout<<"thi is line 127	   "<<endl;
			//cout<<"the round is   "<<roundType<<endl;
			//cout<<"counter is "<<counter<<endl;
			//cout<<"SW cAS is "<<swCAS<<endl;
			//cout<<"LAST cAS is "<<lastCAS<<endl;
			//cout<<"**********************"<<endl;
			// //cout<<"thi is line 114   "<<endl;
			// //cout<<"**********************"<<endl;
			for(unsigned int i = 0; i < Order.size(); i++)
			{	
				if(!jump_5)	
				{		
					int RR = Order.at(i);
					if(tempqueue.find(RR) == tempqueue.end())
					{							
						continue;
					}
					else
					{
						checkCommand = tempqueue[RR];
						if((checkCommand->command == ACT_R) || (checkCommand->command == ACT_W))
						{			
							if(consideredScheduled[RR] == 1)
							{
								if(servicebuffer[RR] == 0)
								{		
									//cout<<"thi is line 152	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;							
									swACT = swACT + 1;	
									swCAS = swCAS + 1;								
									checkCommand = NULL;
									count_ACT = true;	
									jump_5 = true;	
									jump_6 = true;									
								}
							}
						}	
					}	
					checkCommand = NULL;																						
				}
			}
			jump_5 =false;

			if(!count_ACT)
			{
				blockACT = true;
			}
			else
			{
				blockACT = false;
			}					
		}
		// //cout<<"thi is line 158	   "<<endl;
		// //cout<<"**********************"<<endl;
		if (counter == swCAS)
		{
			//cout<<"thi is line 185	   "<<endl;
			//cout<<"the round is   "<<roundType<<endl;
			//cout<<"counter is "<<counter<<endl;
			//cout<<"SW cAS is "<<swCAS<<endl;
			//cout<<"LAST cAS is "<<lastCAS<<endl;
			//cout<<"**********************"<<endl;
			//To check whether we are scheduled something but did not serviced. In this Case we just push SW CAS 1 point further.
			for(unsigned int i = 0; i < Order.size(); i++)
			{	
				if(!jump_5)	
				{		
					int RR = Order.at(i);
					if(tempqueue.find(RR) == tempqueue.end())
					{
						continue;
					}
					else
					{
						checkCommand = tempqueue[RR];
						if((checkCommand->command == RD) || (checkCommand->command == WR))
						{					
							if(consideredScheduled[RR] == 1)
							{
								if(servicebuffer[RR] == 0)
								{
									//cout<<"thi is line 209	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									swCAS = swCAS + 1;
									jump_4 = true;
									jump_5 = true;
									jump_7 = true;
									checkCommand = NULL;											
								}
							}
						}	
					}	
					checkCommand = NULL;																						
				}
			}	
			jump_5 =false;	

			if (!jump_4)
			{
				if (roundType == true)
				{
					for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
					{			
						checkCommand = it->second;
						if (checkCommand->command == WR) 
						// RD to WR (Open)
						{	
							//cout<<"thi is line 238	   "<<endl;
							//cout<<"the round is   "<<roundType<<endl;
							//cout<<"counter is "<<counter<<endl;
							//cout<<"SW cAS is "<<swCAS<<endl;
							//cout<<"LAST cAS is "<<lastCAS<<endl;
							//cout<<"**********************"<<endl;
							for(uint64_t i = 0 ; i < 8; i++)
							{
								servicebuffer[i] = 0;
							}
							for (unsigned int i = 0; i < 8; i++)
							{
								consideredScheduled[i] = 0;
							}
							roundType = false; // WR									
							lastACT = 0;
							
							//counter = 0; 
							first = true;
							blockACT = false;
							countACT = 0;
							countFAW = 0;
							countCAS = 0;
							jump_1 = true;							
						}
					}	
					if(!jump_1)
					{
						
						for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
						{			
							checkCommand = it->second;
							if (checkCommand->command == ACT_W)
							{	
								//cout<<"thi is line 271	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
								//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
								}	
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								roundType = false;										
								lastACT = 0;
								blockACT = false;										
								//counter = 0;
								countACT = 0;
								countFAW = 0;
								first = true;
								countCAS = 0;
								jump_2 = true;
							}
						}
					}
					if (!jump_1)
					{
						if(!jump_2)
						{
							//cout<<"thi is line 306	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
							for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
							{			
								checkCommand = it->second;
								if (checkCommand->command == RD) 
								{	
									//cout<<"thi is line 318	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;																		
									BypassReset = false;																				
									jump_3 = true;							
								}
							}	

							for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
							{			
								checkCommand = it->second;
								if (checkCommand->command == ACT_R)
								{				
									//cout<<"thi is line 334	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"requestor id"<<checkCommand->requestorID;
									//cout<<"**********************"<<endl;							
									BypassReset = false;
									jump_3 = true;
								}
							}
							
							if (jump_3)
							{	
								//cout<<"thi is line 323	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
								//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;								
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
								}	
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								blockACT = false;
								counter = 0;
								lastACT = 0;
								lastCAS = -tCCD;
								swCAS = 0;
								swACT = 0;	
								countACT = 0;									
								countCAS = 0;	
								countFAW = 0;
								jump_3 = false;
																		
							}
							else 
							{
								BypassReset = true;
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
									countCAS = 0;	
									countACT = 0;
									countFAW = 0;
								}
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								blockACT = false;	
							}						
						}
					}																													
				}
				else if (roundType == false )
				{
					for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
					{			
						checkCommand = it->second;
						if (checkCommand->command == RD) 
						{	
							//cout<<"thi is line 374	   "<<endl;
							//cout<<"the round is   "<<roundType<<endl;
							//cout<<"counter is "<<counter<<endl;
							//cout<<"SW cAS is "<<swCAS<<endl;
							//cout<<"LAST cAS is "<<lastCAS<<endl;
							//cout<<"**********************"<<endl;
							for(uint64_t i = 0 ; i < 8; i++)
							{
								servicebuffer[i] = 0;
							}
							for (unsigned int i = 0; i < 8; i++)
							{
								consideredScheduled[i] = 0;
							}
							roundType = true;	
							lastACT = 0;
							blockACT = false;
							//counter = 0; 
							first = true;
							countACT = 0;
							countCAS = 0;	
							countFAW = 0;
							jump_1 = true;		
												
						}
					}	
					if(!jump_1)
					{
						
						for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
						{			
							checkCommand = it->second;
							if (checkCommand->command == ACT_R)
							{
								//cout<<"thi is line 407	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
							//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;			
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
								}	
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								roundType = true;										
								//counter = 0;
								lastACT = 0;
								blockACT = false;
								first = true;
								countACT = 0;
								countCAS = 0;
								countFAW = 0;
								jump_2 = true;	
								
							}
						}
					}
					if (!jump_1)
					{
						if(!jump_2)
						{
							for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
							{			
								checkCommand = it->second;
								if (checkCommand->command == WR) 
								{																				
									BypassReset = false;																				
									jump_3 = true;	
																
								}
							}	

							for(std::map<unsigned int, BusPacket*>::iterator it=tempqueue.begin(); it!=tempqueue.end(); it++) // for all requestors from "num". getRequestorIndex() gives the number of requestors	
							{			
								checkCommand = it->second;
								if (checkCommand->command == ACT_W)
								{											
									BypassReset = false;
									jump_3 = true;	
									
								}
							}
							
							if (jump_3)
							{	
								//cout<<"thi is line 461	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
								//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;									
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
								}	
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								blockACT = false;
								lastACT = 0;
								lastCAS = -tCCD;
								swCAS = 0;
								swACT = 0;
								counter = 0;
								countCAS = 0;	
								countACT = 0;
								countFAW = 0;
								jump_3 = false;	
																		
							}
							else 
							{
								BypassReset = true;
								for(uint64_t i = 0 ; i < 8; i++)
								{
									servicebuffer[i] = 0;
									countCAS = 0;	
									countFAW = 0;
									countACT = 0;
										
								}
								for (unsigned int i = 0; i < 8; i++)
								{
									consideredScheduled[i] = 0;
								}
								blockACT = false;	
							}
						}
					}
				}											
			}
			jump_1 = false;
			jump_2 = false;
			jump_4 = false;																
		}
		else 
		{
		}
		// //cout<<"thi is line 458	   "<<endl;
		// //cout<<"**********************"<<endl;
		for(unsigned int i = 0; i < Order.size(); i++)
		{			
			int RR = Order.at(i);
			
			if(tempqueue.find(RR) == tempqueue.end())
			{
				continue;
			}
			else
			{
				checkCommand = tempqueue[RR];
				if (checkCommand->command == RD)
				{
					if(roundType == true)
					{
						if(servicebuffer[RR] == 0)
						{					
							if (consideredScheduled[RR] == 0)
							{
								consideredScheduled[RR] = 1;
								if (first)
								{
									//cout<<"thi is line 539	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									countCAS++;
									if(lastCAS < 0)
									{
										lastCAS = -lastCAS;											
									}
									
									lastCAS = tWtoR - (counter - lastCAS);
									if(lastCAS < 0){
										lastCAS = 0;
									}										
									counter = 0;
									first = false;
									swCAS = lastCAS + tCCD;
									swACT = 0;
									
								}
								else 
								{
									//cout<<"thi is line 562	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									countCAS++;
									lastCAS = lastCAS + tCCD;
									swCAS = swCAS + tCCD;		
									swACT = swCAS - tRCD;
									if(swACT < 0){
										swACT = 0;
									}													
								}	
							}
						}	
					}												
				checkCommand = NULL;
				}
				else if (checkCommand->command == WR)
				{
					// //cout<<"thi is line 518	   "<<endl;
					// //cout<<"**********************"<<endl;
					if (roundType == false)
					{
						if(servicebuffer[RR] == 0)
						{							
							if (consideredScheduled[RR] == 0)
							{									
								consideredScheduled[RR] = 1;
								if (first)
								{
									//cout<<"thi is line 593	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									countCAS++;
									if(lastCAS < 0)
									{
										lastCAS = -lastCAS;
									}
									lastCAS = tRTW - (counter - lastCAS);
									if(lastCAS < 0){
										lastCAS = 0;
									}
									counter = 0;
									first = false;
									swCAS = lastCAS + tCCD;
									swACT = 0;
								}
								else 
								{
									//cout<<"thi is line 614	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									countCAS++;
									lastCAS = lastCAS + tCCD;											
									swCAS = swCAS + tCCD;	
									swACT = swCAS - tRCD;	
									if(swACT < 0){
										swACT = 0;
									}									
								}									
							}
						}	
					}
				checkCommand = NULL;
				}
				else if (checkCommand->command == ACT_R)
				{
					if (!blockACT)
					{
						if (roundType == true)
						{
							if(servicebuffer[RR] == 0)
							{
								if(consideredScheduled[RR] == 0)
								{
									//cout<<"thi is line 691	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									for(unsigned int i = 0; i < Order.size(); i++)
									{											
										int RR1 = Order.at(i);
										if(tempqueue.find(RR1) == tempqueue.end())
										{
											continue;
										}
										else
										{
											checkCommand_1 = tempqueue[RR1];	
											if (checkCommand_1->command == RD)
											{
												if(servicebuffer[RR1] == 0)
												{
													if (consideredScheduled[RR1] == 0)
													{
														
														consideredScheduled[RR1] = 1;
														if (first)
														{
															//cout<<"thi is line 668	   "<<endl;
															//cout<<"the round is   "<<roundType<<endl;
															//cout<<"counter is "<<counter<<endl;
															//cout<<"SW cAS is "<<swCAS<<endl;
															//cout<<"LAST cAS is "<<lastCAS<<endl;
															//cout<<"**********************"<<endl;
															countCAS++;
															if(lastCAS < 0)
															{
																lastCAS = -lastCAS;
															}
															lastCAS = tWtoR - (counter - lastCAS);
															if(lastCAS < 0){
																lastCAS = 0;
															}
															counter = 0;
															first = false;
															swCAS = lastCAS + tCCD;	
															swACT = 0;																
														}
														else 
														{
															//cout<<"thi is line 689	   "<<endl;
															//cout<<"the round is   "<<roundType<<endl;
															//cout<<"counter is "<<counter<<endl;
															//cout<<"SW cAS is "<<swCAS<<endl;
															//cout<<"LAST cAS is "<<lastCAS<<endl;
															//cout<<"**********************"<<endl;
															countCAS++;
															lastCAS = lastCAS + tCCD;
															swCAS = swCAS + tCCD;
															swACT = swCAS - tRCD;	
															if(swACT < 0){
																swACT = 0;
															}	
														}	
													}																									
												}
											}	
										}																																					
									}
									//cout<<"Enable FIRST   "<<first<<endl;
									// //cout<<"thi is line 620	   "<<endl;
									// //cout<<"**********************"<<endl;	
									consideredScheduled[RR] = 1;
									if(first)
									{
										//cout<<"thi is line 712	   "<<endl;
										//cout<<"the round is   "<<roundType<<endl;
										//cout<<"counter is "<<counter<<endl;
										//cout<<"SW cAS is "<<swCAS<<endl;
										//cout<<"LAST cAS is "<<lastCAS<<endl;
										//cout<<"**********************"<<endl;
										lastACT = 0;
										lastCAS = tRCD;
										counter = 0;
										first = false;
										countACT++;
										swCAS = lastCAS + tCCD;
										swACT = swCAS - tRCD;
										if(swACT < 0){
										swACT = 0;
										}
										
									}
									else if ((!first) && ((countACT+1) == 5))
									{
										//cout<<"thi is line 731	   "<<endl;
										//cout<<"the round is   "<<roundType<<endl;
										//cout<<"counter is "<<counter<<endl;
										//cout<<"SW cAS is "<<swCAS<<endl;
										//cout<<"LAST cAS is "<<lastCAS<<endl;
										//cout<<"**********************"<<endl;
										countFAW++;
										countACT = 0;
										
										int temp = lastACT + (tFAW - 3*tRRD); 
										if(counter > temp)
										{
											lastACT = counter;
										}
										else
										{
											lastACT = temp; 
										}
										int x = lastACT + tRCD;
										int y = lastCAS + tCCD;	
										if(x >= y)
										{
											lastCAS = x;
										}
										else
										{
											lastCAS = y;
										}
										swCAS = lastCAS + tCCD;										
										
										swACT = swCAS - tRCD;
										if(swACT < 0){
										swACT = 0;
										}
									}
									else
									{
										countACT++;
										int temp = lastACT + tRRD;
										if(counter >= temp)
										{
											lastACT = counter;
										}
										else
										{
											lastACT = temp; 
										}
										
										int x = lastACT + tRCD;
										int y = lastCAS + tCCD;	
										if(x >= y)
										{
											lastCAS = x;
										}
										else
										{
											lastCAS = y;
										}
										swCAS = lastCAS + tCCD;
										swACT = swCAS - tRCD;
										if(swACT < 0){
										swACT = 0;
										}
									}
								}												
							}
						}
					}	
					checkCommand = NULL;
				}	
				else if (checkCommand->command == ACT_W)
				{
					if(!blockACT)
					{
						if (roundType == false)
						{
							if(servicebuffer[RR] == 0)
							{
								if(consideredScheduled[RR] == 0)
								{
									for(unsigned int i = 0; i < Order.size(); i++)
									{											
										int RR1 = Order.at(i);
										if(tempqueue.find(RR1) == tempqueue.end())
										{
											continue;
										}
										else
										{
											checkCommand_1 = tempqueue[RR1];	
											if (checkCommand_1->command == WR)
											{
												if(servicebuffer[RR1] == 0)
												{
													if (consideredScheduled[RR1] == 0)
													{
														consideredScheduled[RR1] = 1;
														if (first)
														{
															//cout<<"thi is line 834	   "<<endl;
															//cout<<"the round is   "<<roundType<<endl;
															//cout<<"counter is "<<counter<<endl;
															//cout<<"SW cAS is "<<swCAS<<endl;
															//cout<<"LAST cAS is "<<lastCAS<<endl;
															//cout<<"**********************"<<endl;
															countCAS++;
															if(lastCAS < 0)
															{
																lastCAS = -lastCAS;
															}
															lastCAS = tRTW - (counter - lastCAS);
															if(lastCAS < 0){
																lastCAS = 0;
															}
															counter = 0;
															swCAS = lastCAS + tCCD;
															first = false;
															swACT = 0;
														}
														else 
														{
															//cout<<"thi is line 855	   "<<endl;
															//cout<<"the round is   "<<roundType<<endl;
															//cout<<"counter is "<<counter<<endl;
															//cout<<"SW cAS is "<<swCAS<<endl;
															//cout<<"LAST cAS is "<<lastCAS<<endl;
															//cout<<"**********************"<<endl;
															countCAS++;
															lastCAS = lastCAS + tCCD;																	
															swCAS = swCAS + tCCD;
															swACT = swCAS - tRCD;	
															if(swACT < 0){
															swACT = 0;
															}															
														}	
													}																									
												}
											}
										}																																					
									}	
									consideredScheduled[RR] = 1;
									
									if(first)
									{
										
										lastACT = 0;
										lastCAS = tRCD;
										counter = 0;
										first = false;
										countACT++;
										swCAS = lastCAS + tCCD;
										swACT = swCAS - tRCD;
										if(swACT < 0){
										swACT = 0;
										}
									}
									
									else if ((!first) && ((countACT+1) == 5))
									{
										countFAW++;
										countACT = 0;
										
										int temp = lastACT + (tFAW - 3*tRRD); 
										if(counter > temp)
										{
											lastACT = counter;
										}
										else
										{
											lastACT = temp; 
										}
										int x = lastACT + tRCD;
										int y = lastCAS + tCCD;	
										if(x >= y)
										{
											lastCAS = x;
										}
										else
										{
											lastCAS = y;
										}
										swCAS = lastCAS + tCCD;
										swACT = swCAS - tRCD;	
										if(swACT < 0){
										swACT = 0;
										}									
										
									}
									else
									{
										countACT++;
										int temp = lastACT + tRRD;
										if(counter >= temp)
										{
											lastACT = counter;
										}
										else
										{
											lastACT = temp; 
										}

										int x = lastACT + tRCD;
										int y = lastCAS + tCCD;	
										if(x >= y)
										{
											lastCAS = x;
										}
										else
										{
											lastCAS = y;
										}
										swCAS = lastCAS + tCCD;
										swACT = swCAS - tRCD;
										if(swACT < 0){
										swACT = 0;
										}
									}			
								}											
							}
						}
					}	
					checkCommand = NULL;
				}
				else if (checkCommand->command == PRE)
				{		
					//cout<<"thi is line 958	   "<<endl;
					//cout<<"the round is   "<<roundType<<endl;
					//cout<<"counter is "<<counter<<endl;
					//cout<<"SW cAS is "<<swCAS<<endl;
					//cout<<"LAST cAS is "<<lastCAS<<endl;
					//cout<<"**********************"<<endl;
					if (roundType == true)
					{
						
						if(servicebuffer[RR] == 0)
						{							
							for(unsigned int i = 0; i < Order.size(); i++)
							{
					
								unsigned int RR1 = Order.at(i);
								if (tempqueue.find(RR1) == tempqueue.end())
								{
									continue;
								}
								else
								{
									checkCommand_1 = tempqueue[RR1];
									if (checkCommand_1->command == RD)
									{
										if(servicebuffer[RR1] == 0)
										{
											if (consideredScheduled[RR1] == 0)
											{
												consideredScheduled[RR1] = 1;															
												if (first)
												{
													countCAS++;
													lastCAS = tWtoR - (counter - lastCAS);
													if(lastCAS < 0){
														lastCAS = 0;
													}
													counter = 0;
													first = false;
													swCAS = lastCAS + tCCD;
													swACT = 0;
												}
												else 
												{
													countCAS++;
													lastCAS = lastCAS + tCCD;														
													swCAS = swCAS + tCCD;	
													swACT = swCAS - tRCD;	
													if(swACT < 0){
														swACT = 0;
													}														
												}
											}																									
										}
									}
								}																																														
							}
							for(unsigned int i = 0; i < Order.size(); i++)
							{
								int RR1 = Order.at(i);

								if (tempqueue.find(RR1) == tempqueue.end())
								{
									continue;
								}
								else
								{
									checkCommand_1 = tempqueue[RR1];
									if (checkCommand_1->command == ACT_R)
									{
										if(!blockACT)
										{
											if(servicebuffer[RR1] == 0)
											{
												if(consideredScheduled[RR1] == 0)
												{
													//cout<<"thi is line 1055	   "<<endl;
													//cout<<"the round is   "<<roundType<<endl;
													//cout<<"counter is "<<counter<<endl;
													//cout<<"SW cAS is "<<swCAS<<endl;
													//cout<<"LAST cAS is "<<lastCAS<<endl;
													//cout<<"**********************"<<endl;
													//update ACT
													consideredScheduled[RR1] = 1;
													
													if(first)
													{
														lastACT = 0;
														lastCAS = tRCD;
														counter = 0;
														first = false;
														countACT++;
														swCAS = lastCAS + tCCD;
														swACT = swCAS - tRCD;
														if(swACT < 0){
															swACT = 0;
														}
													
													}
													else if ((!first) && ((countACT+1) == 5))
													{
														countFAW++;
														countACT = 0;
														
														int temp = lastACT + (tFAW - 3*tRRD); 
														if(counter > temp)
														{
															lastACT = counter;
														}
														else
														{
															lastACT = temp; 
														}
														int x = lastACT + tRCD;
														int y = lastCAS + tCCD;	
														if(x >= y)
														{
															lastCAS = x;
														}
														else
														{
															lastCAS = y;
														}
														swCAS = lastCAS + tCCD;										
														
														swACT = swCAS - tRCD;
														if(swACT < 0){
															swACT = 0;
														}
													}
													else
													{
														countACT++;
														int temp = lastACT + tRRD;
														if(counter >= temp)
														{
															lastACT = counter;
														}
														else
														{
															lastACT = temp; 
														}
													

														int x = lastACT + tRCD;
														int y = lastCAS + tCCD;	
														if(x >= y)
														{
															lastCAS = x;
														}
														else
														{
															lastCAS = y;
														}
														swCAS = lastCAS + tCCD;
														swACT = swCAS - tRCD;
														if(swACT < 0){
															swACT = 0;
														}
													}	
												}																																			
											}
										}	
									}
								}																																			
							}																															
						}
					}
					else if (roundType == false)
					{
						if(servicebuffer[RR] == 0)
						{
							if(consideredScheduled[RR] == 0)
							{
								for(unsigned int i = 0; i < Order.size(); i++)
								{

									int RR1 = Order.at(i);
									if (tempqueue.find(RR1) == tempqueue.end())	
									{
										continue;
									}
									else
									{
										checkCommand_1 = tempqueue[RR1];	
										if (checkCommand_1->command == WR)
										{
											if(servicebuffer[RR1] == 0)
											{
												if (consideredScheduled[RR1] == 0)
												{
													//cout<<"thi is line 1170	   "<<endl;
													//cout<<"the round is   "<<roundType<<endl;
													//cout<<"counter is "<<counter<<endl;
													//cout<<"SW cAS is "<<swCAS<<endl;
													//cout<<"LAST cAS is "<<lastCAS<<endl;
													//cout<<"**********************"<<endl;
													consideredScheduled[RR1] = 1;
													if (first)
													{
														countCAS++;
														lastCAS = tRTW - (counter - lastCAS);
														if(lastCAS < 0){
															lastCAS = 0;
														}
														counter = 0;
														first = false;
														swCAS = lastCAS + tCCD;	
														swACT = 0;
													}
													else 
													{
														countCAS++;
														lastCAS = lastCAS + tCCD;																
														swCAS = swCAS + tCCD;
														swACT = swCAS - tRCD;	
														if(swACT < 0){
															swACT = 0;
														}															
													}	
												}																									
											}
										}	
									}																										
								}
								
								for(unsigned int i = 0; i < Order.size(); i++)
								{
									int RR1 = Order.at(i);

									if(tempqueue.find(RR1) == tempqueue.end())
									{
										continue;
									}
									else
									{
										checkCommand_1 = tempqueue[RR1];	
										if (checkCommand_1->command == ACT_W)
										{
											// //cout<<"thi is line 1071	   "<<endl;
											// //cout<<"**********************"<<endl;
											if(!blockACT)
											{	
												if(servicebuffer[RR1] == 0)
												{
													if(consideredScheduled[RR1] == 0)
													{
														//update ACT
														consideredScheduled[RR1] = 1;
														//cout<<"thi is line 1232	   "<<endl;
														//cout<<"the round is   "<<roundType<<endl;
														//cout<<"counter is "<<counter<<endl;
														//cout<<"SW cAS is "<<swCAS<<endl;
														//cout<<"LAST cAS is "<<lastCAS<<endl;
														//cout<<"**********************"<<endl;
														if(first)
														{
															
															lastACT = 0;
															lastCAS = tRCD;
															counter = 0;
															first = false;
															countACT++;
															swCAS = lastCAS + tCCD;
															swACT = swCAS - tRCD;
															if(swACT < 0){
																swACT = 0;
															}
														}
														else if ((!first) && ((countACT+1) == 5))
														{
															countFAW++;
															countACT = 0;
															
															int temp = lastACT + (tFAW - 3*tRRD); 
															if(counter > temp)
															{
																lastACT = counter;
															}
															else
															{
																lastACT = temp; 
															}
															int x = lastACT + tRCD;
															int y = lastCAS + tCCD;	
															if(x >= y)
															{
																lastCAS = x;
															}
															else
															{
																lastCAS = y;
															}
															swCAS = lastCAS + tCCD;										
															
															swACT = swCAS - tRCD;
															if(swACT < 0){
																swACT = 0;
															}
														}
														else
														{
															countACT++;
															int temp = lastACT + tRRD;
															if(counter >= temp)
															{
																lastACT = counter;
															}
															else
															{
																lastACT = temp; 
															}
															int x = lastACT + tRCD;
															int y = lastCAS + tCCD;	
															if(x >= y)
															{
																lastCAS = x;
															}
															else
															{
																lastCAS = y;
															}
															swCAS = lastCAS + tCCD;
															swACT = swCAS - tRCD;
															if(swACT < 0){
																swACT = 0;
															}
														}	
													}																																			
												}
											}
										}		
									}																																				
								}
							}		
						}
					}							
				}
			}												
		}
		if(BypassReset == true)
		{
			swCAS = counter + 1;
		}
		// //cout<<"thi is line 1174	   "<<endl;
		// //cout<<"**********************"<<endl;
		if(!tempqueue.empty())
		{
			for(unsigned int i = 0; i < Order.size(); i++)
			{
				
				int RR = Order.at(i);

				if(tempqueue.find(RR) == tempqueue.end())
				{
					continue;
				}
				else
				{				
					checkCommand = tempqueue[RR];
					if (checkCommand->command == RD)
					{
						if(roundType == true)
						{
							if(servicebuffer[RR] == 0)
							{
								if(IsIssuable(checkCommand))
								{	
									Order.erase(Order.begin() + i);
									Order.push_back(RR);
									servicebuffer[RR] = 1;
									scheduledCommand = checkCommand;
									queuePending[scheduledCommand->requestorID] = false;
									tempqueue.erase(RR);								
									counter++;
									if(jump_7)
									{
										lastCAS = counter;
										swCAS = lastCAS + tCCD;
									}
									cout<<"******************RD***********************"<<endl;
									//cout<<"thi is line 1328	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"counter is "<<counter<<endl;
									//cout<<"SW cAS is "<<swCAS<<endl;
									//cout<<"LAST cAS is "<<lastCAS<<endl;
									//cout<<"**********************"<<endl;
									jump_6 = false;
									jump_7 = false;	
									BypassReset = false;
									commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
									commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
									return;
								}	
							}	
						}												
					checkCommand = NULL;
					}
					else if (checkCommand->command == WR)
					{
						//cout<<"thi is line 1223	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"LAST cAS is "<<lastCAS<<endl;
						//cout<<"**********************"<<endl;
						if (roundType == false)
						{
							if(servicebuffer[RR] == 0)
							{								
								if(IsIssuable(checkCommand))
								{				
									Order.erase(Order.begin() + i);
									Order.push_back(RR);
									servicebuffer[RR] = 1;
									scheduledCommand = checkCommand; 
									tempqueue.erase(RR);
									queuePending[scheduledCommand->requestorID] = false;	
									counter++;
									cout<<"******************WR***********************"<<endl;
									if(jump_7)
									{
										lastCAS = counter;
										swCAS = lastCAS + tCCD;
									}
									jump_6 = false;
									jump_7 = false;	
									BypassReset = false;
									commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
									commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
									return;
								}
							}	
						}
					checkCommand = NULL;
					}
					else if (checkCommand->command == ACT_R)
					{
						//cout<<"thi is line 1255	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"LAST cAS is "<<lastCAS<<endl;
						//cout<<"**********************"<<endl;
						if (roundType == true)
						{
							if(servicebuffer[RR] == 0)
							{
								for(unsigned int i = 0; i < Order.size(); i++)
								{											
									int RR1 = Order.at(i);
									if(tempqueue.find(RR1) == tempqueue.end())
									{
										continue;
									}
									else
									{
										checkCommand_1 = tempqueue[RR1];	
										if (checkCommand_1->command == RD)
										{
											if(servicebuffer[RR1] == 0)
											{
												if(IsIssuable(checkCommand_1))
												{
													//cout<<"thi is line 1297	   "<<endl;
													//cout<<"the round is   "<<roundType<<endl;
													//cout<<"LAST cAS is "<<lastCAS<<endl;
													//cout<<"**********************"<<endl;
													Order.erase(Order.begin() + i);
													cout<<"******************RD***********************"<<endl;
													Order.push_back(RR1);
													servicebuffer[RR1] = 1;
													scheduledCommand = checkCommand_1;
													tempqueue.erase(RR1);
													//consideredScheduled[RR1] = 0;
													queuePending[scheduledCommand->requestorID] = false;	
													counter++;
													
													if(jump_7)
													{
														lastCAS = counter;
														swCAS = lastCAS + tCCD;
													}
													jump_6 = false;
													jump_7 = false;	
													BypassReset = false;
													commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
													commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
													return;	
												}																									
											}
										}	
									}																																			
								}	
								//cout<<"thi is line 1469	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"LAST cAS is "<<lastCAS<<endl;
						//cout<<"**********************"<<endl;																				
								if(!blockACT)
								{
									//cout<<"thi is line 1475	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"LAST cAS is "<<lastCAS<<endl;
						//cout<<"**********************"<<endl;
									if(IsIssuable(checkCommand))
									{
										//cout<<"thi is line 1328	   "<<endl;
										//cout<<"the round is   "<<roundType<<endl;
										//cout<<"**********************"<<endl;
										scheduledCommand = checkCommand;
										tempqueue.erase(RR);
										queuePending[scheduledCommand->requestorID] = false;										
										cout<<"******************ACT R***********************"<<endl;
										if(jump_6)
										{
											lastACT = counter;
											lastCAS = lastACT + tRCD;
											swCAS = lastCAS + tCCD;
										}
										if(jump_7)
										{
											lastCAS = counter;
											swCAS = lastCAS + tCCD;
										}
										jump_6 = false;
										jump_7 = false;	
										BypassReset = false;
										counter++;
										commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
										commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
										return;	
									}
								}																																	
							}								
						}
					checkCommand = NULL;	
					}	
					else if (checkCommand->command == ACT_W)
					{
						//cout<<"thi is line 1360	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"the counter is   "<<counter<<endl;
						//cout<<"the swCAS is   "<<swCAS<<endl;
							//cout<<"LAST cAS is "<<lastCAS<<endl;
						//cout<<"**********************"<<endl;
						if(!blockACT)
						{
							if (roundType == false)
							{
								if(servicebuffer[RR] == 0)
								{
									for(unsigned int i = 0; i < Order.size(); i++)
									{
										int RR1 = Order.at(i);
										if(tempqueue.find(RR1) == tempqueue.end())
										{
											continue;
										}	
										else
										{
											checkCommand_1 = tempqueue[RR1];	
											if (checkCommand_1->command == WR)
											{
											
												if(servicebuffer[RR1] == 0)
												{
													if(IsIssuable(checkCommand_1))
													{
														Order.erase(Order.begin() + i);
														Order.push_back(RR1);
														servicebuffer[RR1] = 1;
														scheduledCommand = checkCommand_1;
														tempqueue.erase(RR1);
														cout<<"******************WR***********************"<<endl;
														//consideredScheduled[RR1] = 0;
														queuePending[scheduledCommand->requestorID] = false;	
											
														counter++;
														
														if(jump_7)
														{
															lastCAS = counter;
															swCAS = lastCAS + tCCD;
														}
														jump_6 = false;
														jump_7 = false;
														BypassReset = false;
														commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
														commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);	
														return;	
													}																								
												}
											}
										}																																			
									}
									//cout<<"thi is line 1411	   "<<endl;
									//cout<<"the round is   "<<roundType<<endl;
									//cout<<"**********************"<<endl;
									if (IsIssuable(checkCommand))
									{
										scheduledCommand = checkCommand;
										tempqueue.erase(RR);
										queuePending[scheduledCommand->requestorID] = false;	
										if(jump_6)
										{
											lastACT = counter;
											lastCAS = lastACT + tRCD;
											swCAS = lastCAS + tCCD;
										}
										cout<<"******************ACT W***********************"<<endl;
										if(jump_7)
										{
											lastCAS = counter;
										swCAS = lastCAS + tCCD;
										}
										jump_6 = false;
										jump_7 = false;	
										BypassReset = false;
										counter++;
										commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
										commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
										return;	
									}
								}
							}
						}	
						checkCommand = NULL;
					}
					else if (checkCommand->command == PRE)
					{
						//cout<<"thi is line 1444	   "<<endl;
						//cout<<"the round is   "<<roundType<<endl;
						//cout<<"**********************"<<endl;
						if (roundType == true)
						{
							for(unsigned int i = 0; i < Order.size(); i++)
							{
								int RR1 = Order.at(i);
								
								if(tempqueue.find(RR1) == tempqueue.end())
								{
									continue;
								}
								else
								{
									checkCommand_1 = tempqueue[RR1];	
									if (checkCommand_1->command == RD)
									{
										//cout<<"thi is line 1461	   "<<endl;
										//cout<<"the round is   "<<roundType<<endl;
										//cout<<"**********************"<<endl;
										if(servicebuffer[RR1] == 0)
										{
											if(IsIssuable(checkCommand_1))
											{					
												Order.erase(Order.begin() + i);
												Order.push_back(RR1);
												scheduledCommand = checkCommand_1;
												tempqueue.erase(RR1);
												servicebuffer[RR1] = 1;
												queuePending[scheduledCommand->requestorID] = false;
												cout<<"******************RD***********************"<<endl;
												counter++;
												if(jump_7)
												{
													lastCAS = counter;
													swCAS = lastCAS + tCCD;
												}
												jump_6 = false;
												jump_7 = false;	
												BypassReset = false;
												commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
												commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
												return;	
											}																								
										}
									}	
								}																								
							}
							
							for(unsigned int i = 0; i < Order.size(); i++)
							{
								int RR1 = Order.at(i);

								if(tempqueue.find(RR1) == tempqueue.end())
								{
									continue;
								}
								else
								{
									checkCommand_1 = tempqueue[RR1];	
									if (checkCommand_1->command == ACT_R)
									{
										if(!blockACT)
										{
											//cout<<"thi is line 1507	   "<<endl;
											//cout<<"the round is   "<<roundType<<endl;
											//cout<<"**********************"<<endl;
											if(servicebuffer[RR1] == 0)
											{

												if(IsIssuable(checkCommand_1))
												{
													scheduledCommand = checkCommand_1;
													tempqueue.erase(RR1);
													queuePending[scheduledCommand->requestorID] = false;	
													counter++;
													if(jump_6)
													{
														lastACT = counter;
														lastCAS = lastACT + tRCD;
														swCAS = lastCAS + tCCD;
														
													}
													cout<<"******************ACT R***********************"<<endl;
													if(jump_7)
													{
														lastCAS = counter;
														swCAS = lastCAS + tCCD;
													}
													jump_6 = false;
													jump_7 = false;	
													BypassReset = false;
													
													commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
													commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
													return;		
												}																								
											}
										}	
									}	
								}																																	
							}
				
							if(IsIssuable(checkCommand))
							{
								//cout<<"thi is line 1545	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"**********************"<<endl;
								scheduledCommand = checkCommand;
								tempqueue.erase(RR);
								queuePending[scheduledCommand->requestorID] = false;
								counter++;	
								jump_6 = false;
								jump_7 = false;	
								cout<<"******************PRE***********************"<<endl;
								////cout<<"requestor id"<<checkCommand->requestorID;
								BypassReset = false;
								commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
								commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
								return;
							}								
						}
						else if (roundType == false)
						{
							if(servicebuffer[RR] == 0)
							{
								for(unsigned int i = 0; i < Order.size(); i++)
								{
									int RR1 = Order.at(i);

									if(tempqueue.find(RR1) == tempqueue.end())
									{
										continue;
									}
									else
									{
										checkCommand_1 = tempqueue[RR1];	
										if (checkCommand_1->command == WR)
										{
											//cout<<"thi is line 1575	   "<<endl;
											//cout<<"the round is   "<<roundType<<endl;
											//cout<<"**********************"<<endl;
											if(servicebuffer[RR1] == 0)
											{
												if(IsIssuable(checkCommand_1))
												{
													Order.erase(Order.begin() + i);
													Order.push_back(RR1);												
													scheduledCommand = checkCommand_1;
													tempqueue.erase(RR1);
													servicebuffer[RR1] = 1;
													queuePending[scheduledCommand->requestorID] = false;
													counter++;
													cout<<"******************WR***********************"<<endl;
													if(jump_7)
													{
														lastCAS = counter;
														swCAS = lastCAS + tCCD;
													}
													jump_6 = false;
													jump_7 = false;	
													BypassReset = false;
													commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
													commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
													return;											
												}																								
											}
										}	
									}																										
								}
								for(unsigned int i = 0; i < Order.size(); i++)
								{
									int RR1 = Order.at(i);
									if(tempqueue.find(RR1) == tempqueue.end())
									{
										continue;
									}
									else
									{
										checkCommand_1 = tempqueue[RR1];	
										if (checkCommand_1->command == ACT_W)
										{
											if(!blockACT)
											{
												//cout<<"thi is line 1620	   "<<endl;
												//cout<<"the round is   "<<roundType<<endl;
												//cout<<"**********************"<<endl;
												if(servicebuffer[RR1] == 0)
												{
													if(IsIssuable(checkCommand_1))
													{
														scheduledCommand = checkCommand_1;
														tempqueue.erase(RR1);
														queuePending[scheduledCommand->requestorID] = false;	
														counter++;
														if(jump_6)
														{
															lastACT = counter;
															lastCAS = lastACT + tRCD;
															swCAS = lastCAS + tCCD;
														}
														if(jump_7)
														{
															lastCAS = counter;
															swCAS = lastCAS + tCCD;
														}
														cout<<"******************ACT_W ***********************"<<endl;
														jump_6 = false;
														jump_7 = false;	
														BypassReset = false;
														commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
														commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
														return;		
													}																								
												}
											}	
										}	/* code */
									}																									
								}
								//cout<<"thi is line 1653	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"**********************"<<endl;
								if(IsIssuable(checkCommand))
								{
									scheduledCommand = checkCommand;
									tempqueue.erase(RR);
									queuePending[scheduledCommand->requestorID] = false;
									counter++;
									cout<<"******************PRE***********************"<<endl;
									jump_6 = false;
									jump_7 = false;	
									BypassReset = false;
									commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
									commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
									return;
								}
							}
						}
					checkCommand = NULL;																						
					}
				}												
			}
			
			//if there is nothing ready in the Q, we can simply issue the opportunistically
			
			// if(counter < (swCAS - tRCD))
			// {
			// 	if((jump_6 == false) && (jump_7 == false))
			// 	{	
					
					for(unsigned int i = 0; i < Order.size(); i++)
					{				
						int RR = Order.at(i);
						if(tempqueue.find(RR) == tempqueue.end())
						{
							
							continue;
						}
						else
						{
							checkCommand = tempqueue[RR];
						
							if (checkCommand->command == RD)
							{				
								//cout<<"thi is line 1697	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
								//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;		
								if (roundType == true)
								{
									
									if(servicebuffer[RR] == 1)
									{	
										oppslot++;										
										if(IsIssuable(checkCommand))
										{ 
											Order.erase(Order.begin() + i);
											Order.push_back(RR);
											scheduledCommand = checkCommand;
											tempqueue.erase(RR);
											queuePending[scheduledCommand->requestorID] = false;	
											countROPP++;	
											counter++;		
											BypassReset = false;		
											//cout<<"thi is line 1717	   "<<endl;
											//cout<<"the round is   "<<roundType<<endl;
											//cout<<"counter is "<<counter<<endl;
											//cout<<"SW cAS is "<<swCAS<<endl;
											//cout<<"LAST cAS is "<<lastCAS<<endl;
											//cout<<"**********************"<<endl;
											cout<<"******************RD OPP***********************"<<endl;
											commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
											commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);							
											return;
										}															
									}
								}
							checkCommand = NULL;
							}
							else if (checkCommand->command == WR)
							{
								//cout<<"thi is line 1725	   "<<endl;
								//cout<<"the round is   "<<roundType<<endl;
								//cout<<"counter is "<<counter<<endl;
								//cout<<"SW cAS is "<<swCAS<<endl;
								//cout<<"LAST cAS is "<<lastCAS<<endl;
								//cout<<"**********************"<<endl;
								if (roundType == false)
								{
									
									if(servicebuffer[RR] == 1)
									{
										oppslot++;
										if(IsIssuable(checkCommand))
										{
											
											Order.erase(Order.begin() + i);
											Order.push_back(RR);
											//lastCAS = lastCAS + tCCD;	
											scheduledCommand = checkCommand;
											tempqueue.erase(RR);
											queuePending[scheduledCommand->requestorID] = false;	
											countWOPP++;
											BypassReset = false;
											counter++;
											cout<<"******************WR OPP***********************"<<endl;
											commandQueues->UpdateCommandTimeTable(0, scheduledCommand);
											commandQueues->RemoveRequestorElement(scheduledCommand->requestorID, 0, 0);
											return;
										}
																									
									}
								
								}
							
							checkCommand = NULL;
							}	
						}																	
					}
				//}	
			//}
			jump_6 = false;
			jump_7 = false;						
		}	
		cout<<"******************NOTHING***********************"<<endl;						
		// //cout<<"Write opp = "<<countWOPP<<endl;
		// //cout<<"Read opp = "<<countROPP<<endl;
		// //cout<<"Potential OPP request count  "<<oppslot<<endl;
		counter++;
		BypassReset = false;
		
	return;		
	}		
			
	}