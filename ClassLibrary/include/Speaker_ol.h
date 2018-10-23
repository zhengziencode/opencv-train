#ifndef __SPEAKEROL__
#define __SPEAKEROL__
#include <windows.h>
#include <msp_cmn.h>
#include <qtts.h>
#include <msp_errors.h>
#include <mmsystem.h>
#ifdef _WIN32
#pragma comment(lib,"msc.lib")
#else
#pragma comment(lib,"msc_x64.lib")
#endif 
#pragma comment(lib,"winMM.lib")
//wav音频头部格式
typedef struct _wave_pcm_hdr
{
	char riff[4];
	int size_8;
	char wave[4];
	char fmt[4];
	int fmt_size;

	short int format_tag;
	short int channels;
	int samples_per_sec;
	int avg_bytes_per_sec;
	short int block_align;
	short int bits_per_sample;

	char data[4];
	int data_sizes;
}wave_pcm_hdr;

//默认wav头部数据
wave_pcm_hdr default_wav_hdr = {
	{ 'R', 'I', 'F', 'F' },
	0,
	{ 'W', 'A', 'V', 'E' },
	{ 'f', 'm', 't', ' ' },
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{ 'd', 'a', 't', 'a' },
	0
};

class Recognizer
{
private:
protected:
public:
};
class Speaker
{
private:
	const char * sessionID;
	int count;
	char FileName[100];
public:
	Speaker(){
		memset(FileName, 0, 100);
		sessionID = NULL;
		const char* usr = NULL;
		const char* pwd = NULL;
		const char* lgi_param = "appid = 5932465d";
		int ret = MSPLogin(usr, pwd, lgi_param);
		if (MSP_SUCCESS != ret)
		{
			printf("MSPLogin failed, error code is: %d", ret);
		}
	}
	void Speaking(char *text,bool language){
		int ret = -1;
		const char * ssb_param;
		if (language==true)
		{
			ssb_param = "voice_name = xiaoyan, aue = speex-wb;7, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
		}
		else
		{
			ssb_param = "voice_name = catherine , aue = speex-wb;7, sample_rate = 16000, speed = 20, volume = 50, pitch = 40, rdn = 2";
		}
		sessionID = QTTSSessionBegin(ssb_param, &ret);
		if (MSP_SUCCESS != ret)
		{
			printf("QTTSSessionBegin failed, error code is : %d", ret);
		}
		unsigned int text_len = strlen(text); //textLen参数为合成文本所占字节数
		ret = QTTSTextPut(sessionID, text, text_len, NULL);
		if (MSP_SUCCESS != ret)
		{
			printf("QTTSTextPut failed, error code is : %d", ret);
		}

		sprintf_s(FileName, 100, "MyVoice.wav");
		//获取音频
		FILE* fp;
		fopen_s(&fp,FileName, "wb");
		if (fp==NULL)
		{
			return;
		}
		fwrite(&default_wav_hdr, sizeof(default_wav_hdr), 1, fp);
		unsigned int audio_len = 0;
		int synth_status = 0;
		while (1)
		{
			const void * data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
			if (NULL != data)
			{
				fwrite(data, audio_len, 1, fp);
				default_wav_hdr.data_sizes += audio_len;
			}
			if (MSP_TTS_FLAG_DATA_END == synth_status || MSP_SUCCESS != ret)
			{
				break;
			}
		}
		default_wav_hdr.size_8 += default_wav_hdr.data_sizes + (sizeof(default_wav_hdr)-8);
		fseek(fp, 4, 0);
		fwrite(&default_wav_hdr.size_8, sizeof(default_wav_hdr.size_8), 1, fp);
		fseek(fp, 40, 0);
		fwrite(&default_wav_hdr.data_sizes, sizeof(default_wav_hdr.data_sizes), 1, fp);
		fclose(fp);

		ret = QTTSSessionEnd(sessionID, "normal end");
		if (MSP_SUCCESS != ret)
		{
			printf("QTTSSessionEnd failed, error code is : %d", ret);
		}
		wchar_t wFileName[200];
		MultiByteToWideChar(CP_ACP, 0, FileName, -1, wFileName, 100);
		//PlaySound(wFileName, NULL, SND_FILENAME|SND_SYNC);
		//播放音频
		mciSendString(L"open MyVoice.wav alias aa", NULL, 0, NULL);
		mciSendString(L"play aa wait", NULL, 0, NULL);
		mciSendString(L"close aa", NULL, 0, NULL);
		//DeleteFile(L"MyVoice.wav");
	}
	~Speaker(){
		int ret = MSPLogout();
		if (MSP_SUCCESS != ret)
		{
			printf("MSPLogout failed, error code is: %d", ret);
		}
	}
};
#endif