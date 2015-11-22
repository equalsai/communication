/*
 * GwiCRMidInterface.h
 *
 *  Created on: 2015-10-15
 *      Author: Administrator
 */

#ifndef GWICRMIDINTERFACE_H_
#define GWICRMIDINTERFACE_H_

#include <string>
#include "Interface/IDevice.h"

const int CR_BASE = 200;
const int CR_OPENDEVICE_FAILED = (CR_BASE + 1);
const int CR_OPERA_FAILED = (CR_BASE + 2);
const int CR_ERR_INVALID_DATA	= (CR_BASE + 3);
const int CR_ERR_NOMEDIAPRESENT = (CR_BASE + 4);
const int CR_ERR_DEVERROR = (CR_BASE + 5);
const int CR_ERR_TIMEOUT = (CR_BASE + 6);

class GwiCRMidInterface
{
public:
	GwiCRMidInterface();
	virtual ~GwiCRMidInterface();

public:
	int openDevice();
	int closeDevice();
	int getDeviveStatus();
	const char* getDeviceId();
	int readCard(int timeout = 0, int* reqID = NULL);
	int ejectCard(int* reqID = NULL);
	int retainCard(int* reqID = NULL);
    int resetRetainCount(int* reqID = NULL);
    int getRetainCount();
    int getMediaStatus();
    int getCardType();
    bool getData(const std::string& key, std::string& value);
    bool setData(const std::string& key, const std::string& value);
    DeviceErrorCode getErrorCode(char *errCode);
    int getErrorDescription(char *errCode,char *errDescrible);

public:
    char errCode[11];
    char Track1[80];
    char Track2[50];
    char Track3[110];
    char AtrStr[20];
    int CardType;

private:
    unsigned long GetTickCount();

};

#endif /* GWICRMIDINTERFACE_H_ */
