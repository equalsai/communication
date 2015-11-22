/*
 * cardReaderInterface.cpp
 *
 *  Created on: 2015-10-15
 *      Author: Administrator
 */

#include "GwiCardReader.h"
#include "Interface/IDevice.h"



extern "C" IDevice* createDevice(const char* id, void* param)
{
	IDevice* pCardReader = GwiCardReader::getInstance();
	return pCardReader;
}


