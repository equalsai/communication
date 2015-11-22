/*
 * parseData.h
 *
 *  Created on: 2015-10-23
 *      Author: Administrator
 */

#ifndef PARSEDATA_H_
#define PARSEDATA_H_

typedef struct tagSTParam
{
	int paramLen;
	char *pParam;
}STParam;


class CSendDataProtocal
{
public:
	CSendDataProtocal();
	CSendDataProtocal(const CSendDataProtocal& rhs);
	~CSendDataProtocal();
	CSendDataProtocal& operator= (const CSendDataProtocal& rhs);

public:

	int m_totalLen;
	char m_devID[4];
	short m_funLen;
	char m_funName[100];
	long m_timeout;

	int m_paramNum;

	//参数不要超过10个
	STParam m_stFunParam[10];

};


class CReceiveDataProtocal
{
public:
	CReceiveDataProtocal();
	CReceiveDataProtocal(const CReceiveDataProtocal& rhs);
	~CReceiveDataProtocal();
	CReceiveDataProtocal& operator= (const CReceiveDataProtocal& rhs);

public:
	int m_totalLen;
	char m_devID[4];
	short m_funLen;
	char m_funName[100];

	long m_returnResult;

	int m_paramNum;

	STParam m_stFunParam[10];
};


class CParseData
{
public:
	CParseData();
	~CParseData();

	//senddata：应用层->设备
	bool parseSendData(unsigned char *data, int len, CSendDataProtocal &sendData);
	//receivedata：设备->应用层
	bool parseReceiveData(unsigned char *data, int len, CReceiveDataProtocal &receiveData);

	bool composeSendData(CSendDataProtocal data, char *sendData, int &len);
	bool composeReceiveData(CReceiveDataProtocal data, char *receiveData, int &len);
};


#endif /* PARSEDATA_H_ */
