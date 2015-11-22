/*
 * deviceClient.h
 *
 *  Created on: 2015-11-2
 *      Author: Administrator
 */

#ifndef DEVICECLIENT_H_
#define DEVICECLIENT_H_

#include "./include/event2/event.h"
#include "./include/event2/bufferevent.h"
#include <event2/thread.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "parseData.h"
#include <list>

class CDeviceClient
{
public:
	static CDeviceClient* getInstance();
	virtual ~CDeviceClient();

	static void static_readcb(struct bufferevent *bev, void *ptr);
	void readcb(struct bufferevent *bev, void *ptr);
	static void static_eventcb(struct bufferevent *bev, short events, void *ptr);
	void eventcb(struct bufferevent *bev, short events, void *ptr);


	static void * static_threadFunForEvent(void *arg);
	void *threadFunForEvent(void *arg);



private:
	static char m_buf[2048];
	static int m_receiveDataPos;
	static int m_bufTotalLen;

public:
	static struct bufferevent *mp_bev;


//	CSendDataProtocal *mp_sendDataProtocal;

	CReceiveDataProtocal *mp_syncReciveData;

//private:
	std::list<CReceiveDataProtocal *> m_receiveDataList;

	bool mb_isConnected;
public:
	pthread_mutex_t m_joblistLock;
	pthread_mutex_t m_syncReciveDataLock;

private:
	CDeviceClient();
	static CDeviceClient *mp_deviceClient;
};


#endif /* DEVICECLIENT_H_ */
