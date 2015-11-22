/*
 * GwiCardReader.h
 *
 *  Created on: 2015-10-15
 *      Author: Administrator
 */

#ifndef GWICARDREADER_H_
#define GWICARDREADER_H_

//#include "GwiCRMidInterface.h"

#include "Interface/CardReader/ICardReader.h"
#include "deviceClient.h"
/*
#include "./include/event2/event.h"
#include "./include/event2/bufferevent.h"
#include <event2/thread.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "parseData.h"
#include <list>
*/

class GwiCardReader : public ICardReader
{
public:
	static GwiCardReader* getInstance();
	virtual ~GwiCardReader();

	static void * static_threadFunForExcute(void *arg);
	void *threadFunForExcute(void *arg);

	/*
	static void static_readcb(struct bufferevent *bev, void *ptr);
	void readcb(struct bufferevent *bev, void *ptr);
	static void static_eventcb(struct bufferevent *bev, short events, void *ptr);
	void eventcb(struct bufferevent *bev, short events, void *ptr);


	static void * static_threadFunForEvent(void *arg);
	void *threadFunForEvent(void *arg);
	static void * static_threadFunForExcute(void *arg);
	void *threadFunForExcute(void *arg);


private:
	static char m_buf[1500];
	static int m_receiveDataPos;
	static int m_bufTotalLen;
	static struct bufferevent *mp_bev;
*/
public:
	virtual void initialize(XmlParser* pConfig);
	virtual const char* getDeviceId();
	virtual bool isBusy();
	virtual DeviceStatus getDeviceStatus();
	virtual void cancel(int nReqID);
	virtual int transaction(const char* tranID, const void* parameter, int* pReqID = NULL);

	// ��ȡ���һ���������
	virtual int getLastError();

	// ��ȡ���һ�����������
	// ��ֹ����
	virtual const char* getLastErrorDescription();

	virtual bool getVendorInfo(std::string& key, std::string& val);
	//
	// ������ timeout �Ժ���Ϊ��λ���������Ϊ0�����ʾ���޵ȴ�
	//
	virtual int readCard(int timeout = 0, int* reqID = NULL);

	//
	// ������Ƭ���˶�������Ԥ��ʱ��������δ�ɹ����򷵻ز�����ʱ
	//
	virtual int ejectCard(int* reqID = NULL);

	//
	// �̿�
	//
	virtual int retainCard(int* reqID = NULL);

	//
	// �����̿�����
	//
	virtual int resetRetainCount(int* reqID = NULL);

	// ��ȡ�̿�����
	virtual int getRetainCount();

	//
	// ��ȡ��״̬
	//
	virtual CardReaderMediaStatus getMediaStatus();

	// ��ȡλ�ڿ����еĿ������ͣ�����������޿����򷵻� Unknown
	virtual CardReader_CardType getCardType();

	//
	// ��ȡ����
	//
	virtual bool getData(const std::string& key, std::string& value);

	//
	// ��������
	//
	virtual bool setData(const std::string& key, const std::string& value);

private:
	GwiCardReader();

	static GwiCardReader *m_pCardReader;
	static CDeviceClient *mp_deviceClientInstance;

	DeviceErrorCode  LastErrorCode;

	//GwiCRMidInterface m_CRMidInterface;
/*
private:
	CSendDataProtocal *mp_sendDataProtocal;
	std::list<CReceiveDataProtocal *> m_receiveDataList;
	CReceiveDataProtocal *mp_syncReciveData;

private:
	pthread_mutex_t m_joblistLock;
	*/
};

#endif /* GWICARDREADER_H_ */
