/*
 * deviceClient.cpp
 *
 *  Created on: 2015-11-3
 *      Author: Administrator
 */

#include "deviceClient.h"

char CDeviceClient::m_buf[2048] = {0};
int CDeviceClient::m_receiveDataPos = 0;
int CDeviceClient::m_bufTotalLen = 0;
struct bufferevent *CDeviceClient::mp_bev = NULL;
CDeviceClient *CDeviceClient::mp_deviceClient = NULL;

CDeviceClient::CDeviceClient()
{
//	mp_sendDataProtocal = NULL;
	mp_syncReciveData = NULL;

	mb_isConnected = false;
	int ret = pthread_mutex_init(&m_joblistLock, NULL);
	ret = pthread_mutex_init(&m_syncReciveDataLock, NULL);
}


CDeviceClient::~CDeviceClient()
{
	int ret = pthread_mutex_destroy(&m_joblistLock);
	ret = pthread_mutex_destroy(&m_syncReciveDataLock);

	if(m_receiveDataList.size() > 0)
	{
		CReceiveDataProtocal *tmp = NULL;
		for(int i = 0; i < m_receiveDataList.size(); ++i)
		{
			tmp = m_receiveDataList.front();
			m_receiveDataList.pop_front();
			delete tmp;
		}
	}

	if(mp_syncReciveData != NULL)
	{
		delete mp_syncReciveData;
	}

	delete mp_deviceClient;
}


CDeviceClient* CDeviceClient::getInstance()
{
	if(mp_deviceClient == NULL)
	{
		mp_deviceClient = new CDeviceClient;
	}

	return mp_deviceClient;
}

////////////////////////////////////////////////////////////////////
void CDeviceClient::static_readcb(struct bufferevent *bev, void *ptr)
{
	mp_deviceClient->readcb(bev, ptr);
}


void CDeviceClient::readcb(struct bufferevent *bev, void *ptr)
{
    int iLen = 0;

    printf("readcb enter \n");

    while(1)
    {
		iLen = bufferevent_read(bev, m_buf + m_receiveDataPos, sizeof(m_buf) - m_receiveDataPos);

		printf("iLen=<%d> \n", iLen);

		if(!strncmp(m_buf, "StartCommunication", sizeof("StartCommunication")))
		{
			printf("m_buf=<%s> \n", m_buf);
			memset(m_buf, 0x00, sizeof(m_buf));

			mb_isConnected = true;
			break;
		}
		else
		{
			if(iLen < 0)
			{
				break;
			}
			else if(iLen == 0)
			{
				break;
			}
			else
			{
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
						CParseData tmpParseData;
						CReceiveDataProtocal *preceiveDataProtocal = new CReceiveDataProtocal();

						tmpParseData.parseReceiveData((unsigned char *)m_buf + pos, m_bufTotalLen, *preceiveDataProtocal);

						if(!strncmp(preceiveDataProtocal->m_funName, "getDeviceStatus", sizeof("getDeviceStatus") - 1))
						{
							/*
							if(mp_syncReciveData != NULL)
							{
								delete mp_syncReciveData;
								mp_syncReciveData = NULL;
							}
*/
							pthread_mutex_lock(&m_syncReciveDataLock);
							mp_syncReciveData = preceiveDataProtocal;
							pthread_mutex_unlock(&m_syncReciveDataLock);

							//
							printf("m_devID=<%s> \n", preceiveDataProtocal->m_devID);
							printf("m_funName=<%s> \n", preceiveDataProtocal->m_funName);
							printf("m_returnResult=<%ld> \n", preceiveDataProtocal->m_returnResult);
							printf("m_paramNum=<%d> \n", preceiveDataProtocal->m_paramNum);


							for(int i = 0; i < preceiveDataProtocal->m_paramNum; ++i)
							{
								printf("pParam[%d]=<%s> \n", i, preceiveDataProtocal->m_stFunParam[i].pParam);
							}

						}
						else
						{
							//excute
							printf("m_devID=<%s> \n", preceiveDataProtocal->m_devID);
							printf("m_funName=<%s> \n", preceiveDataProtocal->m_funName);
							printf("m_returnResult=<%ld> \n", preceiveDataProtocal->m_returnResult);
							printf("m_paramNum=<%d> \n", preceiveDataProtocal->m_paramNum);


							for(int i = 0; i < preceiveDataProtocal->m_paramNum; ++i)
							{
								printf("pParam[%d]=<%s> \n", i, preceiveDataProtocal->m_stFunParam[i].pParam);
							}

							pthread_mutex_lock(&m_joblistLock);
							m_receiveDataList.push_back(preceiveDataProtocal);
							pthread_mutex_unlock(&m_joblistLock);
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
    }
    printf("readcb end \n");
}


void CDeviceClient::static_eventcb(struct bufferevent *bev, short events, void *ptr)
{

	mp_deviceClient->eventcb(bev, events, ptr);
}


void CDeviceClient::eventcb(struct bufferevent *bev, short events, void *ptr)
{
	if(bev == NULL)
	{
		printf("bev == NULL \n");
	}

    if (events & BEV_EVENT_CONNECTED)
    {
         printf("Connect okay.\n");
    }
    else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF))
    {
    	mb_isConnected = false;

        struct event_base *base = (struct event_base *)ptr;

        printf("Closing\n");
        bufferevent_free(bev);
        event_base_loopexit(base, NULL);
    }
}


void * CDeviceClient::static_threadFunForEvent(void *arg)
{
	mp_deviceClient->threadFunForEvent(arg);
}


void *CDeviceClient::threadFunForEvent(void *arg)
{
    struct event_base *base = NULL;
    //struct bufferevent *bev = NULL;

    evthread_use_pthreads();
    base = event_base_new();

    mp_bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS | BEV_OPT_THREADSAFE);

    bufferevent_setcb(mp_bev, CDeviceClient::static_readcb, NULL, CDeviceClient::static_eventcb, base);
    bufferevent_socket_connect_hostname(mp_bev, NULL, AF_INET, "127.0.0.1", 8888);

    bufferevent_enable(mp_bev, EV_READ|EV_WRITE|EV_PERSIST);

    event_base_dispatch(base);
}


///////////////////////////////////////////////////////////////////
