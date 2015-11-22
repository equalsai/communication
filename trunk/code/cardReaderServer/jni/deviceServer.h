/*
 * deviceServer.h
 *
 *  Created on: 2015-10-23
 *      Author: Administrator
 */

#ifndef DEVICESERVER_H_
#define DEVICESERVER_H_

#include <stdio.h>
#include "./include/event2/event.h"
#include "./include/event2/bufferevent.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <event2/thread.h>
#include "parseData.h"
#include <list>
#include <vector>
#include "GwiCRMidInterface.h"

typedef struct tagSTJobParam
{
	struct bufferevent *bev;
	bool isCancel;
	CSendDataProtocal *mp_sendDataProtocal;

}STJobParam, *PSTJobParam;

class CDeviceServer
{
public:
	CDeviceServer();
	~CDeviceServer();

	static void static_read_cb(struct bufferevent *bev, void *arg);
	void read_cb(struct bufferevent *bev, void *arg);
	static void static_write_cb(struct bufferevent *bev, void *arg);
	void write_cb(struct bufferevent *bev, void *arg);
	static void static_error_cb(struct bufferevent *bev, short event, void *arg);
	void error_cb(struct bufferevent *bev, short event, void *arg);
	static void static_do_accept(evutil_socket_t listener, short event, void *arg);
	void do_accept(evutil_socket_t listener, short event, void *arg);

	static void * static_threadFunForEvent(void *arg);
	void *threadFunForEvent(void *arg);
	static void * static_threadFunForExcute(void *arg);
	void *threadFunForExcute(void *arg);
	static void * static_threadFunStatus(void *arg);
	void *threadFunStatus(void *arg);

private:
	unsigned char m_buf[2048];
	int m_receiveDataPos;
	int m_bufTotalLen;
	struct bufferevent *g_bev;

private:
	pthread_mutex_t m_joblistLock;
	pthread_mutex_t m_deviceSDKLock;
	pthread_mutex_t m_deviceStatusLock;

private:
	CParseData *mp_ParseData;
	CReceiveDataProtocal *mp_receiveDataProtocal;
	//该参数涉及到两个线程需要读写，临界资源，需要使用锁
	//CReceiveDataProtocal *mp_receiveDataProtocal;
	std::list<PSTJobParam> m_joblist;

	bool m_isGetStatus;

	GwiCRMidInterface m_CRMidInterface;
	bool mb_isDeviceOpened;

	std::vector<struct bufferevent *> m_deviceStatusDev;
};


#endif /* DEVICESERVER_H_ */
