/*
 * deviceServer.cpp
 *
 *  Created on: 2015-10-23
 *      Author: Administrator
 */

#include "deviceServer.h"
#include <stdlib.h>

static CDeviceServer g_deviceServer;

CDeviceServer::CDeviceServer()
{
	memset(m_buf, 0x00, sizeof(m_buf));
	m_receiveDataPos = 0;
	m_bufTotalLen = 0;
	g_bev = NULL;

	int ret = pthread_mutex_init(&m_joblistLock, NULL);
	ret = pthread_mutex_init(&m_deviceSDKLock, NULL);
	ret = pthread_mutex_init(&m_deviceStatusLock, NULL);

	mp_ParseData = new CParseData();
	mp_receiveDataProtocal = NULL;

	m_isGetStatus = false;
	mb_isDeviceOpened = false;
}


CDeviceServer::~CDeviceServer()
{
	int ret = pthread_mutex_destroy(&m_joblistLock);
	ret = pthread_mutex_destroy(&m_deviceSDKLock);
	ret = pthread_mutex_destroy(&m_deviceStatusLock);

	if(mp_ParseData != NULL)
	{
		delete mp_ParseData;
		mp_ParseData = NULL;
	}

	if(mp_receiveDataProtocal != NULL)
	{
		delete mp_receiveDataProtocal;
		mp_receiveDataProtocal = NULL;
	}

	if(m_joblist.size() > 0)
	{
		PSTJobParam jobparam;
		for(int i = 0; i < m_joblist.size(); ++i)
		{
			jobparam = m_joblist.front();
			m_joblist.pop_front();
			delete jobparam->mp_sendDataProtocal;
			delete jobparam;
		}
	}
}

void CDeviceServer::static_read_cb(struct bufferevent *bev, void *arg)
{
	g_deviceServer.read_cb(bev, arg);
}


void CDeviceServer::read_cb(struct bufferevent *bev, void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);

    int iLen = 0;

    while(1)
    {
		iLen = bufferevent_read(bev, m_buf + m_receiveDataPos, sizeof(m_buf) - m_receiveDataPos);

		printf("CDeviceServer::read_cb ilen=<%d> \n", iLen);

		if(iLen < 0)
		{
			//输出错误Log
			break;
		}
		else if(iLen == 0)
		{
			break;
		}
		else
		{
			//m_buf[iLen] = 0;
			printf("Client Info: m_buf=<%s> iLen=<%d> \n", m_buf, iLen);

			m_receiveDataPos += iLen;


			int pos = 0;
			while(m_receiveDataPos > pos)
			{
				if(m_bufTotalLen == 0 && m_receiveDataPos - pos >= 4)
				{
					char strTotalLen[8] = {0};
					memcpy(strTotalLen, m_buf, 4);

					m_bufTotalLen = atoi(strTotalLen);
					printf("m_bufTotalLen=<%d> \n", m_bufTotalLen);
				}
				else
				{
					break;
				}

				if(m_receiveDataPos - pos >= m_bufTotalLen)
				{
					PSTJobParam jobParam = new STJobParam;
					jobParam->isCancel = false;
					jobParam->bev = bev;
					jobParam->mp_sendDataProtocal = new CSendDataProtocal();

					mp_ParseData->parseSendData(m_buf + pos, m_bufTotalLen, *jobParam->mp_sendDataProtocal);

					//判断是获取(设备、介质)状态还是其他
					if(!strncmp(jobParam->mp_sendDataProtocal->m_funName, "getDeviceStatus", sizeof("getDeviceStatus")))
					{
						printf("getDeviceStatus \n");
						pthread_mutex_lock(&m_deviceStatusLock);
						//获取状态的标识
						m_isGetStatus = true;
						m_deviceStatusDev.push_back(bev);
						pthread_mutex_unlock(&m_deviceStatusLock);
					}
					else
					{
						printf("excute \n");
						//加锁
						pthread_mutex_lock(&m_joblistLock);
						m_joblist.push_back(jobParam);
						pthread_mutex_unlock(&m_joblistLock);
						//解锁
					}

					pos += m_bufTotalLen;
					m_bufTotalLen = 0;
				}
				else
				{

					break;
				}
				usleep(500000);
			}

			if(m_receiveDataPos > pos)
			{
				char *pbuf = new char[m_receiveDataPos - pos + 1];

				memset(pbuf, 0x00, m_receiveDataPos - pos + 1);
				memcpy(pbuf, m_buf + pos, m_receiveDataPos - pos);
				memset(m_buf, 0x00, sizeof(m_buf));
				memcpy(m_buf, pbuf, m_receiveDataPos - pos);
				delete []pbuf;

				m_receiveDataPos -= pos;
				m_bufTotalLen = 0;
			}
			else
			{
				m_receiveDataPos = 0;
				memset(m_buf, 0x00, sizeof(m_buf));
			}

		}
    }

//    char MESSAGE[]="welcome to server 2 ..";
//    bufferevent_write(bev, MESSAGE, strlen(MESSAGE));

}

void CDeviceServer::static_write_cb(struct bufferevent *bev, void *arg)
{
	g_deviceServer.write_cb(bev, arg);
}


void CDeviceServer::write_cb(struct bufferevent *bev, void *arg)
{

}

void CDeviceServer::static_error_cb(struct bufferevent *bev, short event, void *arg)
{
	g_deviceServer.error_cb(bev, event, arg);
}

void CDeviceServer::error_cb(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, bev=<%u> \n", fd, bev);

    //struct event_base *base = (struct event_base *)ptr;
    if (event & BEV_EVENT_TIMEOUT)
    {
        printf("Timed out\n"); //if bufferevent_set_timeouts() called
    }
    else if (event & BEV_EVENT_EOF)
    {
        printf("connection closed\n");
    }
    else if (event & BEV_EVENT_ERROR)
    {
        printf("some other error\n");
    }
    bufferevent_free(bev);
    //如果发现客户端退出连接，是否需要退出消息循环(ymj)
    //event_base_loopexit(base, NULL);
}

void CDeviceServer::static_do_accept(evutil_socket_t listener, short event, void *arg)
{
	g_deviceServer.do_accept(listener, event, arg);
}

void CDeviceServer::do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    evutil_socket_t fd;
    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    fd = accept(listener, (struct sockaddr *)&sin, &slen);
    if (fd < 0)
    {
        perror("accept");
        return;
    }

    if (fd > FD_SETSIZE)
    {
        perror("fd > FD_SETSIZE\n");
        return;
    }

    printf("ACCEPT: fd = %u\n", fd);

    //关于bufferevent_socket_new函数，应该还需要添加线程安全
    g_bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS | BEV_OPT_THREADSAFE);
    bufferevent_setwatermark(g_bev, EV_READ, 1, 0);
    bufferevent_setcb(g_bev, CDeviceServer::static_read_cb, NULL, CDeviceServer::static_error_cb, arg);
    bufferevent_enable(g_bev, EV_READ|EV_WRITE|EV_PERSIST);

    //需要触发一下，要不然客户端和服务端不会收发数据(ymj)
    char MESSAGE[]="StartCommunication";
    bufferevent_write(g_bev, MESSAGE, strlen(MESSAGE));
}

void * CDeviceServer::static_threadFunForEvent(void *arg)
{
	g_deviceServer.threadFunForEvent(arg);
}

void *CDeviceServer::threadFunForEvent(void *arg)
{
    printf("threadFunForEvent enter \n");

    evthread_use_pthreads();
    evutil_socket_t *listener = (evutil_socket_t *)arg;
    struct event_base *base = event_base_new();
    assert(base != NULL);
    struct event *listen_event;
    listen_event = event_new(base, *listener, EV_READ|EV_PERSIST, CDeviceServer::static_do_accept, (void*)base);
    event_add(listen_event, NULL);

    event_base_dispatch(base);
}

void * CDeviceServer::static_threadFunForExcute(void *arg)
{
	g_deviceServer.threadFunForExcute(arg);
}

void *CDeviceServer::threadFunForExcute(void *arg)
{
	PSTJobParam currentJob = NULL;
	bool hasJob = false;

    while(1)
    {
    	pthread_mutex_lock(&m_joblistLock);
    	hasJob = m_joblist.size() > 0 ? true : false;
    	pthread_mutex_unlock(&m_joblistLock);

    	if(hasJob)
    	{
    		printf("get a job start \n");
    		//加锁
    		pthread_mutex_lock(&m_joblistLock);
    		currentJob = m_joblist.front();
    		m_joblist.pop_front();
    		pthread_mutex_unlock(&m_joblistLock);
    		//解锁
    		printf("get a job end \n");
/*
    		if(!mb_isOpened)
    		{
    			int ret = m_CRMidInterface.openDevice();
    			printf("threadFunForExcute openDevice ret=<%d> \n", ret);


    			mb_isOpened = true;
    		}
*/
    		if(currentJob->isCancel)
    		{

    		}
    		else
    		{
    			//excute
    			printf("m_devID=<%s> \n", currentJob->mp_sendDataProtocal->m_devID);
    			printf("m_funName=<%s> \n", currentJob->mp_sendDataProtocal->m_funName);
    			printf("m_paramNum=<%d> \n", currentJob->mp_sendDataProtocal->m_paramNum);

    			for(int i = 0; i < currentJob->mp_sendDataProtocal->m_paramNum; ++i)
    			{
    				printf("pParam[%d]=<%s> \n", i, currentJob->mp_sendDataProtocal->m_stFunParam[i].pParam);
    			}

    			//执行完毕后，将执行结果返回客户端

    			mp_receiveDataProtocal = new CReceiveDataProtocal();

    			if(!strncmp(currentJob->mp_sendDataProtocal->m_funName, "initialize", sizeof("initialize")))
    			{
    				memcpy(mp_receiveDataProtocal->m_devID, "CR", 2);
    				memcpy(mp_receiveDataProtocal->m_funName, "initialize", sizeof("initialize") - 1);
    				mp_receiveDataProtocal->m_funLen = sizeof("initialize") - 1;

    				if(!mb_isDeviceOpened)
    				{
    					mp_receiveDataProtocal->m_returnResult = m_CRMidInterface.openDevice();
    					if(mp_receiveDataProtocal->m_returnResult == 0)
    					{
    						mb_isDeviceOpened = true;
    					}
    				}
    				else
    				{
    					mp_receiveDataProtocal->m_returnResult = 0;
    				}

    				mp_receiveDataProtocal->m_paramNum = 0;
    			}
    			else if(!strncmp(currentJob->mp_sendDataProtocal->m_funName, "readCard", sizeof("readCard")))
    			{
    				memcpy(mp_receiveDataProtocal->m_devID, "CR", 2);
    				memcpy(mp_receiveDataProtocal->m_funName, "readCard", sizeof("readCard") - 1);
    				mp_receiveDataProtocal->m_funLen = sizeof("readCard") - 1;

    				int timeout = currentJob->mp_sendDataProtocal->m_timeout;

    				if(currentJob->mp_sendDataProtocal->m_paramNum == 0)
    				{
    					mp_receiveDataProtocal->m_returnResult = m_CRMidInterface.readCard(timeout);
    					mp_receiveDataProtocal->m_paramNum = 0;
    				}
    				else
    				{
    					int reqid = atoi(currentJob->mp_sendDataProtocal->m_stFunParam[0].pParam);
    					mp_receiveDataProtocal->m_returnResult = m_CRMidInterface.readCard(timeout, &reqid);

    					char tmpreqid[16] = {0};
    					sprintf(tmpreqid, "%d", reqid);

    					printf("reqid=<%d> tmpreqid=<%s> \n", reqid, tmpreqid);

    	    	        mp_receiveDataProtocal->m_stFunParam[0].paramLen = strlen(tmpreqid);
    	    	        mp_receiveDataProtocal->m_stFunParam[0].pParam = new char[mp_receiveDataProtocal->m_stFunParam[0].paramLen + 1];
    	    	        memset(mp_receiveDataProtocal->m_stFunParam[0].pParam, 0x00, sizeof(mp_receiveDataProtocal->m_stFunParam[0].pParam));
    	    	        memcpy(mp_receiveDataProtocal->m_stFunParam[0].pParam, tmpreqid, strlen(tmpreqid));

    	    	        printf("param[0]=<%s> \n", mp_receiveDataProtocal->m_stFunParam[0].pParam);
    	    	        mp_receiveDataProtocal->m_paramNum = 1;
    				}
    			}
    			else if(!strncmp(currentJob->mp_sendDataProtocal->m_funName, "getMediaStatus", sizeof("getMediaStatus")))
    			{
    				memcpy(mp_receiveDataProtocal->m_devID, "CR", 2);
    				memcpy(mp_receiveDataProtocal->m_funName, "getMediaStatus", sizeof("getMediaStatus") - 1);
    				mp_receiveDataProtocal->m_funLen = sizeof("getMediaStatus") - 1;

    				mp_receiveDataProtocal->m_returnResult = m_CRMidInterface.getMediaStatus();
    				mp_receiveDataProtocal->m_paramNum = 0;
    			}



    			//excute
    			printf("m_devID=<%s> \n", mp_receiveDataProtocal->m_devID);
    			printf("m_funName=<%s> \n", mp_receiveDataProtocal->m_funName);
    			printf("m_paramNum=<%d> \n", mp_receiveDataProtocal->m_paramNum);
    			printf("m_returnResult=<%ld> \n", mp_receiveDataProtocal->m_returnResult);

    			for(int i = 0; i < mp_receiveDataProtocal->m_paramNum; ++i)
    			{
    				printf("pParam[%d]=<%s> \n", i, mp_receiveDataProtocal->m_stFunParam[i].pParam);
    			}

    	        CParseData tmpParseData;
    	        char receiveData[1280] = {0};
    	        int len = 0;
    	        tmpParseData.composeReceiveData(*mp_receiveDataProtocal, receiveData, len);

    	        delete mp_receiveDataProtocal;
    	        mp_receiveDataProtocal = NULL;

    	        printf("len=<%d> \n", len);
    	        for(int i = 0; i < len; ++i)
    	        {
    	        	printf("%c", receiveData[i]);
    	        }
    	        printf("\n");

    	        bufferevent_write(currentJob->bev, receiveData, len);

    			delete currentJob->mp_sendDataProtocal;
    			delete currentJob;

//    			char MESSAGE[]="welcome to server 2 ..";
//    	        bufferevent_write(g_bev, MESSAGE, strlen(MESSAGE));
    		}
    	}
    	else
    	{

    	}

        sleep(1);
    }
}

void * CDeviceServer::static_threadFunStatus(void *arg)
{
	g_deviceServer.threadFunStatus(arg);
}

void *CDeviceServer::threadFunStatus(void *arg)
{
	bool needGetStatus = false;

	while(1)
	{
		pthread_mutex_lock(&m_deviceStatusLock);
		needGetStatus = m_isGetStatus ? true : false;
		pthread_mutex_unlock(&m_deviceStatusLock);
		if(needGetStatus)
		{

/*			if(!mb_isOpened)
			{
				int ret = m_CRMidInterface.openDevice();
				printf("threadFunStatus openDevice ret=<%d> \n", ret);


				mb_isOpened = true;
			}
*/
			printf("send result \n");
			//直接调获取状态的驱动接口
			//执行完成后，返回执行结果，发送数据给client
			mp_receiveDataProtocal = new CReceiveDataProtocal();

	        memcpy(mp_receiveDataProtocal->m_devID, "CR", 2);
	        memcpy(mp_receiveDataProtocal->m_funName, "getDeviceStatus", sizeof("getDeviceStatus") - 1);
	        mp_receiveDataProtocal->m_funLen = sizeof("getDeviceStatus") - 1;


	        mp_receiveDataProtocal->m_returnResult = m_CRMidInterface.getDeviveStatus();
	        printf("mp_receiveDataProtocal->m_returnResult=<%ld> \n", mp_receiveDataProtocal->m_returnResult);

	        mp_receiveDataProtocal->m_paramNum = 0;

	        CParseData tmpParseData;
	        char receiveData[1280] = {0};
	        int len = 0;
	        tmpParseData.composeReceiveData(*mp_receiveDataProtocal, receiveData, len);

	        delete mp_receiveDataProtocal;
	        mp_receiveDataProtocal = NULL;

	        //bufferevent_write(g_bev, receiveData, len);

			pthread_mutex_lock(&m_deviceStatusLock);
			m_isGetStatus = false;
			for(int i = 0; i < m_deviceStatusDev.size(); ++i)
			{
				bufferevent_write(m_deviceStatusDev[i], receiveData, len);
			}
			m_deviceStatusDev.erase(m_deviceStatusDev.begin(), m_deviceStatusDev.end());
			pthread_mutex_unlock(&m_deviceStatusLock);
		}
		else
		{
			sleep(1);
		}
	}
}


int main()
{
    int ret;
    evutil_socket_t listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener > 0);
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");;
    sin.sin_port = htons(8888);

    if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(listener, 32) < 0) {
        perror("listen");
        return 1;
    }

    printf ("Listening...\n");

    evutil_make_socket_nonblocking(listener);

//    g_pdeviceServer = new CDeviceServer();
//    if(g_pdeviceServer == NULL)
//    {
//    	return 1;
//    }
    //创建线程，然后将事件分发放到该线程中(ymj)
    pthread_t ntid;

    int err = pthread_create(&ntid, NULL, CDeviceServer::static_threadFunForEvent, &listener);
    if(err != 0)
    {
        printf("create thread failed %s \n", strerror(err));
    }

    pthread_t ntid2;
    int err2 = pthread_create(&ntid2, NULL, CDeviceServer::static_threadFunForExcute, NULL);
    if(err2 != 0)
    {
        printf("create thread faild 2 %s \n", strerror(err));
    }

    pthread_t ntid3;
    int err3 = pthread_create(&ntid3, NULL, CDeviceServer::static_threadFunStatus, NULL);
    if(err3 != 0)
    {
        printf("create thread faild 3 %s \n", strerror(err));
    }

    printf("The End. \n");

    //不能让主线程结束了。
    while(1)
    {
        sleep(1);
    }
    return 0;
}



