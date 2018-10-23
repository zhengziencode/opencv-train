#ifndef __RECOGNIZSOUND__
#define __RECOGNIZSOUND__
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <qtts.h>


#ifdef _WIN64
#pragma comment(lib,"msc_x64.lib")
#else
#pragma comment(lib,"msc.lib")
#endif
class sttRecognizer{
private:
	int VoiceChoice;//1Ϊ����   2ΪӢ��
	char *szFileName;
protected:
	int ReadWAVFile(char *FileName, char **buf);//��ȡWAV�ļ���������,�����ļ�����
	char *RunAndGetResult(char *WAVfilebuf, int, char *session_begin_params);//�ϴ���Ƶ�ļ�����ȡʶ����
public:
	sttRecognizer();
	bool initConfig(char *szFileName, int language);
	char* GetText();
};
sttRecognizer::sttRecognizer(){
	VoiceChoice = 1;
	szFileName = NULL;
}
bool sttRecognizer::initConfig(char  *szFileName, int language){
	VoiceChoice = language;
	this->szFileName = szFileName;
	int error = MSPLogin(NULL, NULL, "appid = 5932465d, work_dir = .");
	if (error != MSP_SUCCESS)
	{
		printf("Failed to login,check out your network first!\n");
		return false;
	}
	return true;
}
int sttRecognizer::ReadWAVFile(char *FileName, char **buf){
	FILE *pFile = NULL;
	int FileLen = 0;
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
	{
		return 0;
	}
	fseek(pFile, 0, SEEK_END);
	FileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	*buf = new char[FileLen];
	int ReadLen = fread((void *)(*buf), 1, FileLen, pFile);
	if (ReadLen != FileLen)
	{
		return 0;
	}
	fclose(pFile);
	return FileLen;
}
char *sttRecognizer::RunAndGetResult(char *WAVfilebuf, int WAVfileLen, char *session_begin_params){
	if (WAVfilebuf == NULL)
	{
		printf("WAVfilebuf��������\n");
		return NULL;
	}
	char *result = NULL;
	const int RESULTBUFF = 4068;
	result = new char[RESULTBUFF];
	memset(result, 0, RESULTBUFF);
	int error = -1;
	//�������óɹ������ַ�����ʽ��sessionID��ʧ�ܷ���NULL��sessionID�Ǳ���ʶ��ľ��
	const char *sessionID = QISRSessionBegin(NULL, session_begin_params, &error);
	if (error != MSP_SUCCESS)
	{
		printf("QISRSessionID()�������󣡴������:%d\n", error);
		return NULL;
	}
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//��Ƶ״̬
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//�˵���
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//ʶ��״̬
	int             Count = 0;
	unsigned int    TotalLen = 0;

	const int FRAME_LEN = 640;
	while (true)
	{
		unsigned int len = 10 * FRAME_LEN;
		int ret = 0;

		if (WAVfileLen < 2 * len)
			len = WAVfileLen;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == Count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		//printf("��");
		ret = QISRAudioWrite(sessionID, (const void*)&WAVfilebuf[Count], len, aud_stat, &ep_stat, &rec_stat);
		if (ret != MSP_SUCCESS)
		{
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			return NULL;
		}
		Count += (long)len;
		WAVfileLen -= (long)len;

		if (MSP_REC_STATUS_SUCCESS == rec_stat)
		{
			const char *TempResult = QISRGetResult(sessionID, &rec_stat, 0, &error);
			if (MSP_SUCCESS != error)
			{
				printf("\nQISRGetResult failed! error code: %d\n", error);
				return NULL;
			}
			if (NULL != TempResult)
			{
				unsigned int ResultLen = strlen(TempResult);
				TotalLen += ResultLen;
				if (TotalLen >= RESULTBUFF)
				{
					printf("�������ڴ治��!\n");
					return NULL;
				}
				strncat(result, TempResult, ResultLen);
			}
		}
		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		Sleep(200);
	}
	error = QISRAudioWrite(sessionID, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != error)
	{
		printf("\nQISRAudioWrite failed! error code:%d \n", error);
		return NULL;
	}
	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *TempResult = QISRGetResult(sessionID, &rec_stat, 0, &error);
		if (MSP_SUCCESS != error)
		{
			printf("\nQISRGetResult failed, error code: %d\n", error);
			return NULL;
		}
		if (NULL != TempResult)
		{
			unsigned int ResultLen = strlen(TempResult);
			TotalLen += ResultLen;
			if (TotalLen >= RESULTBUFF)
			{
				printf("\nno enough buffer for rec_result !\n");
			}
			strncat(result, TempResult, ResultLen);
		}
		Sleep(150);
	}

	error = QISRSessionEnd(sessionID, NULL);//���ӿں�QISRSessionBegin��Ӧ,���ô˽ӿں󣬸þ����Ӧ�������Դ���������﷨����Ƶ��ʵ���ȣ����ᱻ�ͷţ��û���Ӧ��ʹ�øþ����
	if (error != MSP_SUCCESS)
	{
		return NULL;
	}
	return result;
}
char *sttRecognizer::GetText(){
	char *WAVfilebuf = NULL;
	int WAVEFileLen = ReadWAVFile(szFileName, &WAVfilebuf);
	if (!WAVEFileLen)
	{
		return NULL;
	}
	char* Chinese_session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
	char* English_session_begin_params = "sub = iat, domain = iat, language = en_us, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
	if (VoiceChoice==1)
	{
		return RunAndGetResult(WAVfilebuf, WAVEFileLen, Chinese_session_begin_params);
	}
	if (VoiceChoice==2)
	{
		return RunAndGetResult(WAVfilebuf, WAVEFileLen, English_session_begin_params);
	}
}
#endif