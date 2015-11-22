/*
 * parseData.cpp
 *
 *  Created on: 2015-10-23
 *      Author: Administrator
 */

#include "parseData.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

////////////////class CSendDataProtocal////////////////////////
CSendDataProtocal::CSendDataProtocal()
{
	printf("CSendDataProtocal enter \n");
	m_totalLen = 0;
	memset(m_devID, 0x00, sizeof(m_devID));
	m_funLen = 0;
	memset(m_funName, 0x00, sizeof(m_funName));
	m_timeout = 0;
	m_paramNum = 0;

	memset(m_stFunParam, 0x00, 10 * sizeof(STParam));

	printf("CSendDataProtocal end \n");
}


CSendDataProtocal::CSendDataProtocal(const CSendDataProtocal& rhs)
{
	m_totalLen = rhs.m_totalLen;
	memcpy(m_devID, rhs.m_devID, 4);
	m_funLen = rhs.m_funLen;
	memcpy(m_funName, rhs.m_funName, 100);
	m_timeout = rhs.m_timeout;

	m_paramNum = rhs.m_paramNum;

	//参数不要超过10个
	memcpy(m_stFunParam, rhs.m_stFunParam, 10 * sizeof(STParam));

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(rhs.m_stFunParam[i].paramLen > 0)
		{
			this->m_stFunParam[i].pParam = new char[rhs.m_stFunParam[i].paramLen];
			if(this->m_stFunParam[i].pParam != NULL)
			{
				memset(this->m_stFunParam[i].pParam, 0x00, sizeof(rhs.m_stFunParam[i].paramLen));
				memcpy(this->m_stFunParam[i].pParam, rhs.m_stFunParam[i].pParam, rhs.m_stFunParam[i].paramLen);
			}
			else
			{
				//输出错误Log
				printf("this->m_stFunParam[%d].pParam == NULL \n", i);
			}
		}
	}
}


CSendDataProtocal::~CSendDataProtocal()
{
	printf("~CSendDataProtocal enter \n");

	for(int i = 0; i < m_paramNum; ++i)
	{
		printf("m_paramNum=<%d> \n", m_paramNum);
		delete []m_stFunParam[i].pParam;
	}

	printf("~CSendDataProtocal end \n");
}


CSendDataProtocal& CSendDataProtocal::operator= (const CSendDataProtocal& rhs)
{
	if(this == &rhs)
	{
		return *this;
	}


	m_totalLen = rhs.m_totalLen;
	memcpy(m_devID, rhs.m_devID, 4);
	m_funLen = rhs.m_funLen;
	memcpy(m_funName, rhs.m_funName, 100);
	m_timeout = rhs.m_timeout;

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(this->m_stFunParam[i].pParam != NULL)
		{
			delete this->m_stFunParam[i].pParam;
			this->m_stFunParam[i].pParam = NULL;
		}
	}

	m_paramNum = rhs.m_paramNum;

	//参数不要超过10个
	memcpy(m_stFunParam, rhs.m_stFunParam, 10 * sizeof(STParam));

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(rhs.m_stFunParam[i].paramLen > 0)
		{
			this->m_stFunParam[i].pParam = new char[rhs.m_stFunParam[i].paramLen];
			if(this->m_stFunParam[i].pParam != NULL)
			{
				memset(this->m_stFunParam[i].pParam, 0x00, sizeof(rhs.m_stFunParam[i].paramLen));
				memcpy(this->m_stFunParam[i].pParam, rhs.m_stFunParam[i].pParam, rhs.m_stFunParam[i].paramLen);
			}
			else
			{
				//输出错误Log
				printf("this->m_stFunParam[%d].pParam == NULL \n", i);
			}
		}
	}

	return *this;
}
//////////////////////////////////////////////////////////////


////////////////class CReceiveDataProtocal////////////////////
CReceiveDataProtocal::CReceiveDataProtocal()
{
	m_totalLen = 0;
	memset(m_devID, 0x00, sizeof(m_devID));
	m_funLen = 0;
	memset(m_funName, 0x00, sizeof(m_funName));
	m_returnResult = 0;

	m_paramNum = 0;

	memset(m_stFunParam, 0x00, 10 * sizeof(STParam));

}


CReceiveDataProtocal::CReceiveDataProtocal(const CReceiveDataProtocal& rhs)
{
	m_totalLen = rhs.m_totalLen;
	memcpy(m_devID, rhs.m_devID, 4);
	m_funLen = rhs.m_funLen;
	memcpy(m_funName, rhs.m_funName, 100);

	m_returnResult = rhs.m_returnResult;

	m_paramNum = rhs.m_paramNum;

	memcpy(m_stFunParam, rhs.m_stFunParam, 10 * sizeof(STParam));

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(rhs.m_stFunParam[i].paramLen > 0)
		{
			this->m_stFunParam[i].pParam = new char[rhs.m_stFunParam[i].paramLen];
			if(this->m_stFunParam[i].pParam)
			{
				memset(this->m_stFunParam[i].pParam, 0x00, sizeof(rhs.m_stFunParam[i].paramLen));
				memcpy(this->m_stFunParam[i].pParam, rhs.m_stFunParam[i].pParam, rhs.m_stFunParam[i].paramLen);
			}
			else
			{
				//输出错误Log
				printf("this->m_stFunParam[%d].pParam == NULL \n", i);
			}
		}
	}
}

CReceiveDataProtocal::~CReceiveDataProtocal()
{
	for(int i = 0; i < m_paramNum; ++i)
	{
		delete []m_stFunParam[i].pParam;
	}
}

CReceiveDataProtocal& CReceiveDataProtocal::operator= (const CReceiveDataProtocal& rhs)
{
	if(this == &rhs)
	{
		return *this;
	}

	printf("CReceiveDataProtocal::operator= Enter \n");
	m_totalLen = rhs.m_totalLen;
	printf("m_totalLen=<%d> \n", m_totalLen);
	memcpy(m_devID, rhs.m_devID, 4);
	printf("m_devID=<%s> \n", m_devID);
	m_funLen = rhs.m_funLen;
	printf("m_funLen=<%d> \n", m_funLen);
	memcpy(m_funName, rhs.m_funName, 100);
	printf("m_funName=<%s> \n", m_funName);

	m_returnResult = rhs.m_returnResult;
	printf("m_returnResult=<%ld> \n", m_returnResult);

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(this->m_stFunParam[i].pParam != NULL)
		{
			delete this->m_stFunParam[i].pParam;
			this->m_stFunParam[i].pParam = NULL;
		}
	}

	printf("delete this param m_paramNum=<%d> \n", m_paramNum);
	m_paramNum = rhs.m_paramNum;
	printf("m_paramNum=<%d> \n", m_paramNum);

	memcpy(m_stFunParam, rhs.m_stFunParam, 10 * sizeof(STParam));

	for(int i = 0; i < m_paramNum; ++i)
	{
		if(rhs.m_stFunParam[i].paramLen > 0)
		{
			this->m_stFunParam[i].pParam = new char[rhs.m_stFunParam[i].paramLen];
			if(this->m_stFunParam[i].pParam)
			{
				memset(this->m_stFunParam[i].pParam, 0x00, sizeof(rhs.m_stFunParam[i].paramLen));
				memcpy(this->m_stFunParam[i].pParam, rhs.m_stFunParam[i].pParam, rhs.m_stFunParam[i].paramLen);
			}
			else
			{
				//输出错误Log
				printf("this->m_stFunParam[%d].pParam == NULL \n", i);
			}
		}
	}

	return *this;
}
//////////////////////////////////////////////////////////////

CParseData::CParseData()
{

}

CParseData::~CParseData()
{

}

//
bool CParseData::parseSendData(unsigned char *data, int len, CSendDataProtocal &sendData)
{
	unsigned char *pPos = NULL;
	pPos = data;

	//总长度
	char strTotolLen[8] = {0};
	memcpy(strTotolLen, pPos, 4);
	sendData.m_totalLen = atoi(strTotolLen);

//	printf("strTotolLen=<%s> \n", strTotolLen);

	//DevID
	pPos += 4;
	memcpy(sendData.m_devID, pPos, 2);

	//函数长度
	pPos += 2;
	char strFunLen[4] = {0};
	memcpy(strFunLen, pPos, 2);
	sendData.m_funLen = atoi(strFunLen);

//	printf("strFunLen=<%s> \n", strFunLen);

	//函数名
	if(sendData.m_funLen > 0)
	{
		pPos += 2;
		memcpy(sendData.m_funName, pPos, sendData.m_funLen);

//		printf("m_funName=<%s> \n", sendData.m_funName);

		pPos += sendData.m_funLen;
	}

	//超时时间
	char strTimeout[8] = {0};
	memcpy(strTimeout, pPos, 5);
	sendData.m_timeout = atoi(strTimeout);

//	printf("strTimeout=<%s> \n", strTimeout);

	//第一个参数的长度
	pPos += 5;

	if((pPos - data) == len)
	{
		sendData.m_paramNum = 0;
		return true;
	}

	char strParamLen[8] = {0};
	int tmpParamLen = 0;

	memcpy(strParamLen, pPos, 4);
	tmpParamLen = atoi(strParamLen);

//	printf("strParamLen=<%s> \n", strParamLen);

	int i = 0;
	while(tmpParamLen > 0)
	{
		if(i >= 10)
		{
			//报错
			break;
		}

		sendData.m_stFunParam[i].paramLen = tmpParamLen;

		pPos += 4;
		sendData.m_stFunParam[i].pParam = new char[tmpParamLen + 1];
		memset(sendData.m_stFunParam[i].pParam, 0x00, tmpParamLen + 1);
		memcpy(sendData.m_stFunParam[i].pParam, pPos, tmpParamLen);

		pPos += tmpParamLen;

		if((pPos - data) == len)
		{
			break;
		}

		memset(strParamLen, 0x00, sizeof(strParamLen));
		memcpy(strParamLen, pPos, 4);
		tmpParamLen = atoi(strParamLen);

		i++;
	}

	//参数个数
	sendData.m_paramNum = i + 1;

	return true;
}


bool CParseData::parseReceiveData(unsigned char *data, int len, CReceiveDataProtocal &receiveData)
{
	unsigned char *pPos = NULL;
	pPos = data;

	//总长度
	char strTotalLen[8] = {0};
	memcpy(strTotalLen, pPos, 4);
	receiveData.m_totalLen = atoi(strTotalLen);

//	printf("strTotalLen=<%s> \n", strTotalLen);

	//DevID
	pPos += 4;
	memcpy(receiveData.m_devID, pPos, 2);

	//函数长度
	pPos += 2;
	char strFunLen[4] = {0};
	memcpy(strFunLen, pPos, 2);
	receiveData.m_funLen = atoi(strFunLen);

//	printf("strFunLen=<%s> \n", strFunLen);

	//函数名
	if(receiveData.m_funLen > 0)
	{
		pPos += 2;
		memcpy(receiveData.m_funName, pPos, receiveData.m_funLen);

		pPos += receiveData.m_funLen;
	}

	//返回值
	char strReturnResult[8] = {0};
	memcpy(strReturnResult, pPos, 4);
	receiveData.m_returnResult = atoi(strReturnResult);

	//第一个参数的长度
	pPos += 4;

	if((pPos - data) == len)
	{
		receiveData.m_paramNum = 0;
		return true;
	}

	char strParamLen[8] = {0};
	int tmpParamLen = 0;

	memcpy(strParamLen, pPos, 4);
	tmpParamLen = atoi(strParamLen);

	int i = 0;
	while(tmpParamLen > 0)
	{
		if(i >= 10)
		{
			//报错
			break;
		}

		receiveData.m_stFunParam[i].paramLen = tmpParamLen;

		pPos += 4;
		receiveData.m_stFunParam[i].pParam = new char[tmpParamLen + 1];
		memset(receiveData.m_stFunParam[i].pParam, 0x00, tmpParamLen + 1);
		memcpy(receiveData.m_stFunParam[i].pParam, pPos, tmpParamLen);
//		printf("%d %s", i, receiveData.m_stFunParam[i].pParam);

		pPos += tmpParamLen;

		if((pPos - data) == len)
		{
			break;
		}

		memcpy(strParamLen, pPos, 4);
		tmpParamLen = atoi(strParamLen);

		i++;
	}

	receiveData.m_paramNum = i + 1;

	return true;
}


bool CParseData::composeSendData(CSendDataProtocal data, char *sendData, int &len)
{
	char *pPos = sendData;
	len = 0;

	//移动到总长度的下一个数据节
	len += 4;
	pPos += 4;

	//type
	memcpy(pPos, data.m_devID, 2);
	len += 2;
	pPos += 2;

	//函数长度
	char strFunLen[4];
	if(data.m_funLen > 0 && data.m_funLen <= 99)
	{
		sprintf(strFunLen, "%d", data.m_funLen);
		memcpy(pPos, strFunLen, 2);
		len += 2;
		pPos += 2;
	}
	else
	{
		//输出错误Log
		return false;
	}

	//函数名
	memcpy(pPos, data.m_funName, data.m_funLen);
	len += data.m_funLen;
	pPos += data.m_funLen;

	//超时时间
	char strTimeout[8] = {0};
	if(data.m_timeout >= 0 && data.m_timeout <= 99999)
	{
		sprintf(strTimeout, "%ld", data.m_timeout);
		memcpy(pPos, strTimeout, 5);
		len += 5;
		pPos += 5;
	}
	else
	{
		//输出错误Log
		return false;
	}

	//参数
	if(data.m_paramNum > 10)
	{
		//输出错误Log
		return false;
	}

	char strParamLen[8] = {0};
	for(int i = 0; i < data.m_paramNum; ++i)
	{
		if(data.m_stFunParam[i].paramLen > 0 && data.m_stFunParam[i].paramLen <= 9999)
		{
			memset(strParamLen, 0x00, sizeof(strParamLen));
			sprintf(strParamLen, "%d", data.m_stFunParam[i].paramLen);
			memcpy(pPos, strParamLen, 4);
			len += 4;
			pPos += 4;

//			printf("strParamLen=<%s> \n", strParamLen);

			memcpy(pPos, data.m_stFunParam[i].pParam, data.m_stFunParam[i].paramLen);
			len += data.m_stFunParam[i].paramLen;
			pPos += data.m_stFunParam[i].paramLen;

//			printf("pParam=<%s> \n", data.m_stFunParam[i].pParam);
		}
		else
		{
			//输出错误Log
			return false;
		}
	}

	//总长度
	char strTotalLen[8] = {0};
	if(len > 0 && len <= 9999)
	{
		sprintf(strTotalLen, "%d", len);
		memcpy(sendData, strTotalLen, 4);
	}
	else
	{
		//输出错误Log
		return false;
	}
}


bool CParseData::composeReceiveData(CReceiveDataProtocal data, char *receiveData, int &len)
{
	char *pPos = receiveData;
	len = 0;

	//移动到总长度的下一个数据节
	len += 4;
	pPos += 4;

	//type
	memcpy(pPos, data.m_devID, 2);
	len += 2;
	pPos += 2;

	//函数长度
	char strFunLen[4];
	if(data.m_funLen > 0 && data.m_funLen <= 99)
	{
		sprintf(strFunLen, "%d", data.m_funLen);
		memcpy(pPos, strFunLen, 2);
		len += 2;
		pPos += 2;
	}
	else
	{
		//输出错误Log
		return false;
	}

	//函数名
	memcpy(pPos, data.m_funName, data.m_funLen);
	len += data.m_funLen;
	pPos += data.m_funLen;

	//返回值
	char strReturnResult[8] = {0};
	if(data.m_returnResult >= 0 && data.m_returnResult <= 9999)
	{
		sprintf(strReturnResult, "%ld", data.m_returnResult);
		memcpy(pPos, strReturnResult, 4);
		len += 4;
		pPos += 4;
	}
	else
	{
		//输出错误Log
		return false;
	}

	//参数
	if(data.m_paramNum > 10)
	{
		//输出错误Log
		return false;
	}

	char strParamLen[8] = {0};
	for(int i = 0; i < data.m_paramNum; ++i)
	{
		if(data.m_stFunParam[i].paramLen > 0 && data.m_stFunParam[i].paramLen <= 9999)
		{
			memset(strParamLen, 0x00, sizeof(strParamLen));
			sprintf(strParamLen, "%d", data.m_stFunParam[i].paramLen);
			memcpy(pPos, strParamLen, 4);

//			printf("strParamLen=<%s> \n", strParamLen);

			len += 4;
			pPos += 4;

			memcpy(pPos, data.m_stFunParam[i].pParam, data.m_stFunParam[i].paramLen);
			len += data.m_stFunParam[i].paramLen;
			pPos += data.m_stFunParam[i].paramLen;

//			printf("pParam=<%s> \n", data.m_stFunParam[i].pParam);
		}
		else
		{
			//输出错误Log
			return false;
		}
	}

	//总长度
	char strTotalLen[8] = {0};
	if(len > 0 && len <= 9999)
	{
		sprintf(strTotalLen, "%d", len);
		memcpy(receiveData, strTotalLen, 4);

	}
	else
	{
		//输出错误Log
		return false;
	}
}

