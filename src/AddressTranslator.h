/*
 * AddressTranslator.h
 *
 *  Created on: Nov 30, 2017
 *      Author: dlguo
 */

#ifndef ADDRESSTRANSLATOR_H_
#define ADDRESSTRANSLATOR_H_

#include <vector>
#include <string>

#include "Request.h"
#include "Global.h"

namespace MCsimulator
{
	class AddressTranslator
	{
	public:
		AddressTranslator(std::string AddressMapping);
		virtual ~AddressTranslator();
		void Tick(std::vector<Request*> incomingRequestBuffer);

	private:
		void TranslateAddress(Request* request);
		void BuildMappingScheme(std::string level);
		int Log2(int level);
		std::vector<DRAM_Level> mappingOrder;
	};

} /* namespace MCsimulator */

#endif /* ADDRESSTRANSLATOR_H_ */
