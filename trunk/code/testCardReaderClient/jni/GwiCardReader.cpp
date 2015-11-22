/*
 * GwiCardReader.cpp
 *
 *  Created on: 2015-10-15
 *      Author: Administrator
 */
#include "GwiCardReader.h"


GwiCardReader* GwiCardReader::m_pCardReader = NULL;
CDeviceClient *GwiCardReader::mp_deviceClientInstance = NULL;

const int CR_BASE = 200;
const int CR_OPENDEVICE_FAILED = (CR_BASE + 1);
const int CR_OPERA_FAILED = (CR_BASE + 2);
const int CR_ERR_INVALID_DATA	= (CR_BASE + 3);
const int CR_ERR_NOMEDIAPRESENT = (CR_BASE + 4);
const int CR_ERR_DEVERROR = (CR_BASE + 5);
const int CR_ERR_TIMEOUT = (CR_BASE + 6);

GwiCardReader::GwiCardReader()
{

}

GwiCardReader::~GwiCardReader()
{
	delete mp_deviceClientInstance;
	delete m_pCardReader;
}

GwiCardReader* GwiCardReader::getInstance()
{
	if(m_pCardReader == NULL)
	{
		m_pCardReader = new GwiCardReader;
	}

	return m_pCardReader;
}




void GwiCardReader::initialize(XmlParser* pConfig)
{
	mp_deviceClientInstance = CDeviceClient::getInstance();

    //�����̣߳�Ȼ���¼��ַ��ŵ����߳���(ymj)
    pthread_t ntid;

    int err = pthread_create(&ntid, NULL, CDeviceClient::static_threadFunForEvent, NULL);
    if(err != 0)
    {
        printf("create thread failed %s \n", strerror(err));
        m_pEventHandler->initializeCompleted(DEVICE_ERROR_INTERNAL_ERROR);
        return;
    }

    pthread_t ntid2;
    int err2 = pthread_create(&ntid2, NULL, GwiCardReader::static_threadFunForExcute, NULL);
    if(err != 0)
    {
    	printf("create thread failed %s \n", strerror(err));
    	m_pEventHandler->initializeCompleted(DEVICE_ERROR_INTERNAL_ERROR);
    	return;
    }

    //��֤�ͷ����������������
    while(1)
    {
    	if(mp_deviceClientInstance->mb_isConnected)
    	{
    		break;
    	}
    	else
    	{
    		usleep(500000);
    	}
    }

    //���豸
	//1. ��ɷ������ݵĽṹ��
	CSendDataProtocal tmpSendDataProtocal;

	memcpy(tmpSendDataProtocal.m_devID, "CR", 2);
    memcpy(tmpSendDataProtocal.m_funName, "initialize", sizeof("initialize") - 1);
    tmpSendDataProtocal.m_funLen = sizeof("initialize") - 1;

    tmpSendDataProtocal.m_timeout = 0;
    tmpSendDataProtocal.m_paramNum = 0;

    //2. ���ṹ�����Ϊ�ɷ��͵����ݴ�
    CParseData tmpParseData;
    char sendData[128] = {0};
    int len = 0;
    tmpParseData.composeSendData(tmpSendDataProtocal, sendData, len);

    //3. ��������
    bufferevent_write(mp_deviceClientInstance->mp_bev, sendData, len);

    printf("initialize send data end \n");
}

void * GwiCardReader::static_threadFunForExcute(void *arg)
{
	m_pCardReader->threadFunForExcute(arg);
}


void *GwiCardReader::threadFunForExcute(void *arg)
{
	CReceiveDataProtocal* currentJob;
	bool hasJob = false;


	while(1)
	{
    	pthread_mutex_lock(&(mp_deviceClientInstance->m_joblistLock));
    	hasJob = (mp_deviceClientInstance->m_receiveDataList).size() > 0 ? true : false;
    	pthread_mutex_unlock(&(mp_deviceClientInstance->m_joblistLock));

    	if(hasJob)
    	{
    		printf("hasJob 11 \n");
    		pthread_mutex_lock(&(mp_deviceClientInstance->m_joblistLock));
    		currentJob = mp_deviceClientInstance->m_receiveDataList.front();
    		mp_deviceClientInstance->m_receiveDataList.pop_front();
    		pthread_mutex_unlock(&(mp_deviceClientInstance->m_joblistLock));

    		printf("hasJob 22 \n");

    		if(!strncmp(currentJob->m_funName, "initialize", currentJob->m_funLen))
    		{
    			if(currentJob->m_returnResult == 0)
    			{
    				printf("send initializeComplete event returnResult=<%ld> \n", currentJob->m_returnResult);
    			}
    			else
    			{
    				printf("send initializeComplete event returnResult=<%ld> \n", currentJob->m_returnResult);
    			}
    		}
    		else if(!strncmp(currentJob->m_funName, "readCard", currentJob->m_funLen))
    		{
    			//����readCard��ɵ��¼�
    			printf("send readCardComplete event returnResult=<%ld> \n", currentJob->m_returnResult);
    			printf("11111111111 \n");

    			if(currentJob->m_paramNum > 0)
    			{
        			int reqid = atoi(currentJob->m_stFunParam[0].pParam);
        			printf("param readCardComplete event reqid=<%d> \n", reqid);
    			}

    			delete currentJob;
    		}
    		else if(!strncmp(currentJob->m_funName, "getMediaStatus", currentJob->m_funLen))
    		{
    			pthread_mutex_lock(&(mp_deviceClientInstance->m_syncReciveDataLock));
    			if(mp_deviceClientInstance->mp_syncReciveData == NULL)
    			{
    				mp_deviceClientInstance->mp_syncReciveData = currentJob;
    			}
    			else
    			{
    				printf("threadFunForExcute mp_deviceClientInstance->mp_syncReciveData != NULL \n");
    			}
    			pthread_mutex_unlock(&(mp_deviceClientInstance->m_syncReciveDataLock));
    		}



    	}
    	else
    	{
    		sleep(1);
    	}

		sleep(1);
	}
}


const char* GwiCardReader::getDeviceId()
{
	return NULL;
	//return m_CRMidInterface.getDeviceId();
}


bool GwiCardReader::isBusy()
{
	return false;
}


DeviceStatus GwiCardReader::getDeviceStatus()
{
	printf("getDeviceStatus enter \n");

	//1. ��ɷ������ݵĽṹ��
	CSendDataProtocal tmpSendDataProtocal;

	memcpy(tmpSendDataProtocal.m_devID, "CR", 2);
    memcpy(tmpSendDataProtocal.m_funName, "getDeviceStatus", sizeof("getDeviceStatus") - 1);
    tmpSendDataProtocal.m_funLen = sizeof("getDeviceStatus") - 1;

    tmpSendDataProtocal.m_timeout = 0;
    tmpSendDataProtocal.m_paramNum = 0;

    //2. ���ṹ�����Ϊ�ɷ��͵����ݴ�
    CParseData tmpParseData;
    char sendData[128] = {0};
    int len = 0;
    tmpParseData.composeSendData(tmpSendDataProtocal, sendData, len);

    //3. ��������
    bufferevent_write(mp_deviceClientInstance->mp_bev, sendData, len);

    printf("send data end \n");
    //��Ҫ�ȴ����ؽ������Ϊ����һ��ͬ���ӿ�
    CReceiveDataProtocal *tmpData = NULL;
    bool hasJob = false;
    bool hasReturn = false;

    //4. ͬ���ӿڣ��ȴ���������
    while(1)
    {
		pthread_mutex_lock(&(mp_deviceClientInstance->m_syncReciveDataLock));
		tmpData = mp_deviceClientInstance->mp_syncReciveData;
		pthread_mutex_unlock(&(mp_deviceClientInstance->m_syncReciveDataLock));
    	if(tmpData == NULL)
    	{
    		printf("mp_syncReciveData == NULL \n");
    		sleep(1);
    	}
    	else
    	{
			if(!strncmp(tmpData->m_funName, "getDeviceStatus", sizeof("getDeviceStatus") - 1))
			{

				printf("get the return data \n");
				break;
			}
			else
			{
				printf("m_funName=<%s> \n", tmpData->m_funName);
				sleep(1);
			}
    	}
    }

    //5. ����������
	int ret = (int)tmpData->m_returnResult;
	printf("getDeviceStatus ret=<%d> \n", ret);

	pthread_mutex_lock(&(mp_deviceClientInstance->m_syncReciveDataLock));
	delete mp_deviceClientInstance->mp_syncReciveData;
	mp_deviceClientInstance->mp_syncReciveData = NULL;
	pthread_mutex_unlock(&(mp_deviceClientInstance->m_syncReciveDataLock));

	if(ret == 0)
	{
		return DEVICE_STATUS_ONLINE;
	}
	else if(ret == 1)
	{
		getLastError();
		printf("getDeviceStatus--[%s]. \n",getLastErrorDescription());
		return DEVICE_STATUS_HWERROR;
	}
	else
	{
		getLastError();
		printf("getDeviceStatus--[%s]. \n",getLastErrorDescription());
		return DEVICE_STATUS_END;
	}
}


void GwiCardReader::cancel(int nReqID)
{

}


int GwiCardReader::transaction(const char* tranID, const void* parameter, int* pReqID)
{
	return 0;
}

// ��ȡ���һ���������
int GwiCardReader::getLastError()
{
	char errStr[100];
	memset(errStr,0x00,sizeof(errStr));
	//LastErrorCode = m_CRMidInterface.getErrorCode(m_CRMidInterface.errCode);
	return 0;
}

// ��ȡ���һ�����������
// ��ֹ����
const char* GwiCardReader::getLastErrorDescription()
{
	return NULL;
	//return ErrorCode2String(LastErrorCode);
}

//��ȡ������Ϣ
bool GwiCardReader::getVendorInfo(std::string& key, std::string& val)
{
	if(key.compare("Name") == 0)
	{
		val = "GWI";
		return true;
	}
	return false;
}

//����
int GwiCardReader::readCard(int timeout, int* reqID)
{
	printf("readCard enter \n");

	CSendDataProtocal tmpSendDataProtocal;

    memcpy(tmpSendDataProtocal.m_devID, "CR", 2);
    memcpy(tmpSendDataProtocal.m_funName, "readCard", sizeof("readCard") - 1);
    tmpSendDataProtocal.m_funLen = sizeof("readCard") - 1;

    tmpSendDataProtocal.m_timeout = timeout;

    if(reqID != NULL)
    {
        char tmpreqID[8] = {0};
        sprintf(tmpreqID, "%d", *reqID);
        tmpSendDataProtocal.m_stFunParam[0].paramLen = sizeof(int);
        tmpSendDataProtocal.m_stFunParam[0].pParam = new char[5];
        memset(tmpSendDataProtocal.m_stFunParam[0].pParam, 0x00, sizeof(tmpSendDataProtocal.m_stFunParam[0].pParam));
        memcpy(tmpSendDataProtocal.m_stFunParam[0].pParam, tmpreqID, sizeof(int));

        tmpSendDataProtocal.m_paramNum = 1;
    }
    else
    {
    	tmpSendDataProtocal.m_paramNum = 0;
    }


    CParseData tmpParseData;
    char sendData[128] = {0};
    int len = 0;
    tmpParseData.composeSendData(tmpSendDataProtocal, sendData, len);

    bufferevent_write(mp_deviceClientInstance->mp_bev, sendData, len);

    printf("readCard send data end \n");

    return 0;
}


// ������Ƭ���˶�������Ԥ��ʱ��������δ�ɹ����򷵻ز�����ʱ
int GwiCardReader::ejectCard(int* reqID)
{
	//int ret = m_CRMidInterface.ejectCard(reqID);
	int ret = CR_OPERA_FAILED;
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("ejectCard--[%s]. \n",getLastErrorDescription());
	}
	return ret;
}

// �̿�
int GwiCardReader::retainCard(int* reqID)
{
	int ret = CR_OPERA_FAILED;//m_CRMidInterface.retainCard(reqID);
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("retainCard--[%s]. \n",getLastErrorDescription());
	}
	return ret;
}

//�����̿�����
int GwiCardReader::resetRetainCount(int* reqID)
{
	int ret = CR_OPERA_FAILED;//m_CRMidInterface.resetRetainCount(reqID);
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("resetRetainCount--[%s]. \n",getLastErrorDescription());
	}
	return ret;
}

// ��ȡ�̿�����
int GwiCardReader::getRetainCount()
{
	int ret = CR_OPERA_FAILED;//m_CRMidInterface.getRetainCount();
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("getRetainCount--[%s]. \n",getLastErrorDescription());
	}
	return ret;
}

// ��ȡ��״̬
ICardReader::CardReaderMediaStatus GwiCardReader::getMediaStatus()
{
	printf("getDeviceStatus enter \n");

	CSendDataProtocal tmpSendDataProtocal;

	memcpy(tmpSendDataProtocal.m_devID, "CR", 2);
    memcpy(tmpSendDataProtocal.m_funName, "getMediaStatus", sizeof("getMediaStatus") - 1);
    tmpSendDataProtocal.m_funLen = sizeof("getMediaStatus") - 1;

    tmpSendDataProtocal.m_timeout = 0;
    tmpSendDataProtocal.m_paramNum = 0;

    CParseData tmpParseData;
    char sendData[128] = {0};
    int len = 0;
    tmpParseData.composeSendData(tmpSendDataProtocal, sendData, len);

    bufferevent_write(mp_deviceClientInstance->mp_bev, sendData, len);

    printf("send data end \n");

    //��Ҫ�ȴ����ؽ������Ϊ����һ��ͬ���ӿ�
    CReceiveDataProtocal *tmpData = NULL;
    bool hasJob = false;
    bool hasReturn = false;

    while(1)
    {
		pthread_mutex_lock(&(mp_deviceClientInstance->m_syncReciveDataLock));
		tmpData = mp_deviceClientInstance->mp_syncReciveData;
		pthread_mutex_unlock(&(mp_deviceClientInstance->m_syncReciveDataLock));
    	if(tmpData == NULL)
    	{
    		printf("mp_syncReciveData == NULL \n");
    		sleep(1);
    	}
    	else
    	{
			if(!strncmp(tmpData->m_funName, "getMediaStatus", sizeof("getMediaStatus") - 1))
			{

				printf("get the return data \n");
				break;
			}
			else
			{
				printf("m_funName=<%s> \n", tmpData->m_funName);
				sleep(1);
			}
    	}
    }

	int ret = (int)tmpData->m_returnResult;
	printf("getMediaStatus ret=<%d> \n", ret);

	pthread_mutex_lock(&(mp_deviceClientInstance->m_syncReciveDataLock));
	delete mp_deviceClientInstance->mp_syncReciveData;
	mp_deviceClientInstance->mp_syncReciveData = NULL;
	pthread_mutex_unlock(&(mp_deviceClientInstance->m_syncReciveDataLock));


	CardReaderMediaStatus status = NOTEXIST;
//	int ret = CR_OPERA_FAILED;//m_CRMidInterface.getMediaStatus();
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("getMediaStatus--[%s]. \n",getLastErrorDescription());
	}
	switch(ret)
	{
	case 0:
		status = PRESENT;
		break;
	case 1:
		status = LATCHED;
		break;
	case 2:
		status = ENTERING;
		break;
	case 3:
		status = NOTEXIST;
		break;
	case 4:
		status = NOTSUPPORTED;
		break;
	default:
		status = NOTSUPPORTED;
		break;
	}
	return status;
}

// ��ȡλ�ڿ����еĿ������ͣ�����������޿����򷵻� Unknown
ICardReader::CardReader_CardType GwiCardReader::getCardType()
{
	CardReader_CardType type = CRCT_Unknown;
	int ret = CR_OPERA_FAILED;//m_CRMidInterface.getCardType();
	if(ret == CR_OPERA_FAILED)
	{
		getLastError();
		printf("getCardType--[%s]. \n",getLastErrorDescription());
	}
	switch(ret)
	{
	case 0:
		type = CRCT_MegCard;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		type = CRCT_ICCard;
		break;
	default:
		type = CRCT_Unknown;
		break;
	}
	return type;
}

// ��ȡ����
bool GwiCardReader::getData(const std::string& key, std::string& value)
{
	return  false;//m_CRMidInterface.getData(key,value);
}

// ��������
bool GwiCardReader::setData(const std::string& key, const std::string& value)
{
	return true;
}



