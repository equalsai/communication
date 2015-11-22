/*
 * GwiCRMidInterface.cpp
 *
 *  Created on: 2015-10-15
 *      Author: Administrator
 */
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "GwiCRMidInterface.h"
#include "include/GWI_CardReader_Driver.h"
#include "ErrorCodeParse.h"

#define TimeOut 15000


GwiCRMidInterface::GwiCRMidInterface()
{
	strcpy(errCode, "0000000000");
	memset(Track1,0x00,sizeof(Track1));
	memset(Track2,0x00,sizeof(Track2));
	memset(Track3,0x00,sizeof(Track3));
	memset(AtrStr,0x00,sizeof(AtrStr));
	CardType = -1;
}

GwiCRMidInterface::~GwiCRMidInterface()
{
	closeDevice();
}


int GwiCRMidInterface::openDevice()
{
	int ret = 0;
	char devtype[10];
	strcpy(devtype,"CRT_288K");

	ret = Card_SetDeviceParam(devtype, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_SetDeviceParam--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}

	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}

	ret = Card_Reset(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_Reset--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	closeDevice();
	return ret;
}

int GwiCRMidInterface::closeDevice()
{
	int ret = 0;
	ret = Card_CloseDevice(errCode);

	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_CloseDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}

	return ret;
}

int GwiCRMidInterface::getDeviveStatus()
{
	int ret = 0;
	int status = -1;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}
	ret = Card_GetStatus(TimeOut, status, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_GetStatus--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	//0:正常    1:错误
	closeDevice();
	return status;
}

const char* GwiCRMidInterface::getDeviceId()
{
	int ret = 0;

/*	char devid[10];
	ret = Card_GetDevid(TimeOut, devid, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_GetDevid--[%s]. \n",buffer);
		return NULL;
	}*/
	return "CRT_288K";
}

int GwiCRMidInterface::readCard(int timeout, int* reqID)
{
	int ret = 0;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}
	int status = -1;
	int statusEX = -1;
	unsigned long time1 = GetTickCount();
	//Card_Enable:检测是否有卡，有则返回错误，无卡则设置进卡方式
	ret = Card_Enable(TimeOut,errCode);
	if(ret !=0 )
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_Enable--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	//等待卡片进入
	while(1)
	{
		usleep(50000);
		unsigned long useTime = GetTickCount() - time1;
		if(useTime>=timeout*1000 && timeout>0)//时间到了,判断是否有卡，无卡则返回超时
		{
			ret = Card_GetMediaStatus(TimeOut, status,statusEX, errCode);
			if(ret !=0 )
			{
				char buffer[100];
				memset(buffer,0x00,sizeof(buffer));
				getErrorDescription(errCode,buffer);
				printf("Card_GetMediaStatus--[%s]. \n",buffer);
				closeDevice();
				return CR_OPERA_FAILED;
			}
			if(status == 3)
			{
				char buffer[100];
				memset(buffer,0x00,sizeof(buffer));
				strcpy(errCode,"0000000014");
				getErrorDescription(errCode,buffer);
				printf("readCard:[%s]. \n",buffer);
				Card_Disable(TimeOut,errCode);
				closeDevice();
				return CR_ERR_TIMEOUT;
			}
		}
		else
		{
			ret = Card_GetMediaStatus(TimeOut, status,statusEX, errCode);
			if(ret !=0 )
			{
				char buffer[100];
				memset(buffer,0x00,sizeof(buffer));
				getErrorDescription(errCode,buffer);
				printf("Card_GetMediaStatus--[%s]. \n",buffer);
				closeDevice();
				return CR_OPERA_FAILED;
			}
			if(status == 0)
			{
				break;
			}
		}


	}
	//Card_GetCardType:判断卡类型：磁条卡、IC卡
	int type = 0;
	ret = Card_GetCardType(type,errCode);
	CardType = type;
	if(ret !=0 )
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_GetCardType--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	//磁条卡：设置读卡方式（读3个磁道）
	if(type == 0)//磁条卡
	{
		ret = Card_SetReadMode(7,errCode);
		if(ret !=0 )
		{
			char buffer[100];
			memset(buffer,0x00,sizeof(buffer));
			getErrorDescription(errCode,buffer);
			printf("Card_SetReadMode--[%s]. \n",buffer);
			closeDevice();
			return CR_OPERA_FAILED;
		}
	}
	else if(type == 1 || type == 2) //IC卡：读ATR数据
	{
		int len = 0;
		int type = 0;
		ret = Card_IcPowerOn(TimeOut, AtrStr,len, type, errCode);
		if(ret !=0 )
		{
			char buffer[100];
			memset(buffer,0x00,sizeof(buffer));
			getErrorDescription(errCode,buffer);
			printf("Card_SetReadMode--[%s]. \n",buffer);
			closeDevice();
			return CR_OPERA_FAILED;
		}
	}
	else//未实现
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		strcpy(errCode,"1100000066");
		getErrorDescription(errCode,buffer);
		printf("Card_GetCardType--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	closeDevice();
	return 0;
}

int GwiCRMidInterface::ejectCard(int* reqID)
{
	int ret = 0;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}

	memset(Track1,0x00,sizeof(Track1));
	memset(Track2,0x00,sizeof(Track2));
	memset(Track3,0x00,sizeof(Track3));
	memset(AtrStr,0x00,sizeof(AtrStr));
	ret = Card_ClearBufferData(TimeOut,errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_ClearBufferData--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	ret = Card_Eject(TimeOut, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_Eject--[%s]. \n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	closeDevice();
	return ret;
}

int GwiCRMidInterface::retainCard(int* reqID)
{
	int ret = 0;
	ret = Card_Capture(TimeOut, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_Capture--[%s]. \n",buffer);
		return CR_OPERA_FAILED;
	}
	return ret;
}

int GwiCRMidInterface::resetRetainCount(int* reqID)
{
	int ret = 0;
	ret = Card_ResetCaptureNum(TimeOut, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("resetRetainCount--[%s] .\n",buffer);
		return CR_OPERA_FAILED;
	}
	return ret;
}

int GwiCRMidInterface::getRetainCount()
{
	int ret = 0;
	int num = 0;
	ret = Card_GetCaptureNum(TimeOut, num, errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("getRetainCount--[%s] .\n",buffer);
		return CR_OPERA_FAILED;
	}
	return num;
}

int GwiCRMidInterface::getMediaStatus()
{
	int ret = 0;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}
	int status = -1;
	int statusEx = -1;

	ret = Card_GetMediaStatus(TimeOut, status, statusEx, errCode);
	printf("media status=[%d] .\n", status);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_GetMediaStatus--[%s] .\n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	//0:有卡   1:已上电  2:在门口 3:无卡4:不支持
	closeDevice();
	return status;
}

int GwiCRMidInterface::getCardType()
{
	int ret = 0;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}
	int type = 0;
	ret = Card_GetCardType(type,errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_GetCardType--[%s] .\n",buffer);
		closeDevice();
		return CR_OPERA_FAILED;
	}
	closeDevice();
	return type;
}

bool GwiCRMidInterface::getData(const std::string& key, std::string& value)
{
	int ret = 0;
	ret = Card_OpenDevice(errCode);
	if(ret != 0)
	{
		char buffer[100];
		memset(buffer,0x00,sizeof(buffer));
		getErrorDescription(errCode,buffer);
		printf("Card_OpenDevice--[%s]. \n",buffer);
		return CR_OPENDEVICE_FAILED;
	}
	if(CardType == 0)
	{
		ret = Card_ReadTrackAll(TimeOut,Track1,Track2,Track3,errCode);
		if(ret != 0)
		{
			char buffer[100];
			memset(buffer,0x00,sizeof(buffer));
			getErrorDescription(errCode,buffer);
			printf("Card_ReadTrackAll--[%s] .\n",buffer);
			closeDevice();
			return false;
		}
	}
	else if(CardType == 1 || CardType == 2)
	{
		if(NULL == AtrStr)
		{
			int len = 0;int type = 0;
			ret = Card_IcWarnReset(TimeOut,AtrStr,len,type,errCode);
			if(ret != 0)
			{
				char buffer[100];
				memset(buffer,0x00,sizeof(buffer));
				getErrorDescription(errCode,buffer);
				printf("Card_IcWarnReset--[%s] .\n",buffer);
				closeDevice();
				return false;
			}
		}
	}
	closeDevice();
	if(key.compare("Track1") == 0)
	{
		if(NULL != Track1)
		{
			value = Track1;
			return true;
		}
	}
	else if(key.compare("Track2") == 0)
	{
		if(NULL != Track2)
		{
			value = Track2;
			return true;
		}
	}
	else if(key.compare("Track3") == 0)
	{
		if(NULL != Track3)
		{
			value = Track3;
			return true;
		}
	}
	else if(key.compare("ATR") == 0)
	{
		if(NULL != AtrStr)
		{
			value = AtrStr;
			return true;
		}
	}
	return false;
}

bool GwiCRMidInterface::setData(const std::string& key, const std::string& value)
{
	return false;
}

DeviceErrorCode GwiCRMidInterface::getErrorCode(char *errCode)
{
	int i = 0;
	while(strcmp(m_ErrorCodeParse[i].errorcode,""))
	{
		if(!strcmp(m_ErrorCodeParse[i].errorcode,errCode))
			break;
	 	i++;
 	}
	if(!strcmp(m_ErrorCodeParse[i].errorcode,""))
		return DEVICE_ERROR_INTERNAL_ERROR;
	else
		return m_ErrorCodeParse[i].IDCResult;
}

//记录日志使用
int GwiCRMidInterface::getErrorDescription(char *errCode,char *errDescrible)
{
	int i = 0;
	memset(errDescrible,0x00,sizeof(errDescrible));
	while(strcmp(m_ErrorCodeParse[i].errorcode,""))
	{
		if(!strcmp(m_ErrorCodeParse[i].errorcode,errCode))
			break;
	 	i++;
 	}
	if(!strcmp(m_ErrorCodeParse[i].errorcode,""))
	{
		strcpy(errDescrible,"Undefined Error");
		return 1;
	}
	else
	{
		strcpy(errDescrible,m_ErrorCodeParse[i].errordescrible);
	}
	return 0;
}

//记录系统开机时的毫秒数
unsigned long GwiCRMidInterface::GetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	return (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));
}





