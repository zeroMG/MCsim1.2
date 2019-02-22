/*
 * BankOpenRowTable.h
 *
 *  Created on: Dec 10, 2018
 *      Author: dlguo
 */

#ifndef BANKOPENROWTABLE_H_
#define BANKOPENROWTABLE_H_

#include <map>
#include <vector>



namespace MCsimulator
{
	class BankOpenRowTable
	{
	public:
		static BankOpenRowTable* getInstance()
        {
            static BankOpenRowTable* instance = new BankOpenRowTable();
            return instance;
        }
		
        /*
         * @brief   : Return the row that is currently open in the bank row buffer
         * @return  : Return -1 if the row is closed
         */ 
		int currentOpenRow(int channel, int rank, int bankGroup, int bank);

        /*
         * @brief   : Return the row that will be fetched into the bank row buffer 
         *            based on previously generated commands
         * @return  : Return -1 if the row is closed
         */         
        int pendingOpenRow(int channel, int rank, int bankGroup, int bank);

        bool updateCurrentOpenRow(int channel, int rank, int bankGroup, int bank, int row);
        bool updatePendingOpenRow(int channel, int rank, int bankGroup, int bank, int row);

	private:
        BankOpenRowTable();
        virtual ~BankOpenRowTable();
        std::map<int, std::vector<int>> bankCurrentOpenRowTable;
        std::map<int, std::vector<int>> bankPendingOpenRowTable;
        int channel;

	};

} /* namespace MCsimulator */

#endif /* BANKOPENROWTABLE_H_ */
