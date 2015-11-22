/*
 * GWI_CardReader_Driver.h
 *
 *  Created on: 2015-9-29
 *      Author: Administrator
 */
#ifndef GWI_CARDREADER_DRIVER_H_
#define GWI_CARDREADER_DRIVER_H_

#define UnknowICCard 0
#define CPU_T0 1
#define CPU_T1 2
#define SLE4442 3
#define SLE4428 4
#define AT24   5
#ifdef __cplusplus
extern "C"
{
#endif
int  Card_SetDeviceParam( char* devtype, char* pszRcCode, int serialtype=0, char*devport=NULL, char *devportparam=NULL);// 设置参数
int  Card_OpenDevice(char *pszRcCode);					// 打开设备
int  Card_CloseDevice(char *pszRcCode);					// 关闭设备
int  Card_GetErrorMsg(char *errcode, char *errmsg, char *pszRcCode);
int  Card_GetVersionInfo(char *versionInfo, char *pszRcCode);

int  Card_Reset(char *pszRcCode, int mode=0);
int  Card_LockCard(char *pszRcCode);
int  Card_UnlockCard(char *pszRcCode);
int  Card_SetMode(char *pszRcCode, int mode=0);
int  Card_GetStatus(long dwTimeOut, int &status,char *pszRcCode);
int  Card_GetDevid(long dwTimeOut, char *devid, char *pszRcCode);
int  Card_GetCardType(int &type, char *pszRcCode, int mode=0);
int  Card_Capture(long dwTimeOut, char *pszRcCode);
int  Card_Eject(long dwTimeOut, char *pszRcCode);

int  Card_GetMediaStatus(long dwTimeOut, int &status,int &statusEX, char *pszRcCode);
int  Card_Enable(long dwTimeOut, char *pszRcCode, int mode=0);
int  Card_Disable(long dwTimeOut, char *pszRcCode);
int  Card_SetReadMode( int trackid, char *pszRcCode, int validmode=0, int atuotransfer=0);
int  Card_ReadTrack(int trackid, long dwTimeOut, char *trackBuf, char *pszRcCode);
int  Card_WriteTrack(int trackid, char *trackBuf, long dwTimeOut, char *pszRcCode);
int  Card_GetCaptureNum(long dwTimeOut, int &num, char *pszRcCode);
int  Card_ResetCaptureNum(long dwTimeOut, char *pszRcCode);
int  Card_IcPowerOn(long dwTimeOut, char *outAtrBuf,int &outdatalen, int &type, char *pszRcCode,int powermode=0);
int  Card_IcPowerOff(long dwTimeOut, char *pszRcCode);
int  Card_IcExchange(char *indataBuf, int indatalen, long dwTimeOut, char *outdataBuf, int &outdatalen,char *pszRcCode,int protocol=2);
int  Card_IcWarnReset(long dwTimeOut, char *outAtrBuf,int &outdatalen,int &type,char *pszRcCode);

int  Card_ReadTrackAll(long dwTimeOut, char *track1Buf, char *track2Buf, char *track3Buf, char *pszRcCode);
int  Card_ClearBufferData(long dwTimeOut,char *pszRcCode);


#ifdef __cplusplus
}
#endif

#endif /* GWI_CARDREADER_DRIVER_H_ */
