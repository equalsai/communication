#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
//#include "testDeviceClient.h"
#include "GwiCardReader.h"

extern "C" IDevice* createDevice(const char* id, void* param);

int main()
{

	GwiCardReader *pcardReader = (GwiCardReader *)createDevice(NULL, NULL);
	if(pcardReader == NULL)
	{
		printf("pcardReader == NULL \n");
		return 1;
	}

	printf("initialize start \n");
	pcardReader->initialize(NULL);
	printf("initialize end \n");

	sleep(10);

	printf("getDeviceStatus start \n");
	pcardReader->getDeviceStatus();
	printf("getDeviceStatus end \n");

	printf("getMediaStatus start \n");
	pcardReader->getMediaStatus();
	printf("getMediaStatus end \n");

	sleep(10);

	printf("readCard start \n");
	int req = 2;
	pcardReader->readCard(5000, &req);
	printf("readCard end \n");

	/*
	printf("readCard start \n");
	req = 3;
	pcardReader->readCard(5000, &req);
	printf("readCard end \n");
*/

	while(1)
	{
		sleep(2);
	}

    return 0;
}
