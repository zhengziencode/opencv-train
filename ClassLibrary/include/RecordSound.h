#ifndef __RECORDSOUND__
#define __RECORDSOUND__
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class SoundRecorder{
private:
	FILE *fp;
	WAVEFORMATEX waveFormat;
	HWAVEIN phwi;
	bool saveToFile;
	bool isRecording;
	bool stopRecord;
	WAVEINCAPS waveInCaps;
	char *szFileName;
	WAVEHDR pwh1, pwh2;
	char szBuf1[10240], szBuf2[10240];
	static DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
protected:
	void initFormat();
public:
	SoundRecorder();
	SoundRecorder(char *szFileName);
	bool StartRecord();
	bool StopRecord();
};

void SoundRecorder::initFormat(){
	waveFormat.cbSize = 0;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = 16000;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = 2;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec*(16>>3);// (waveFormat.nBlockAlign*waveFormat.nSamplesPerSec)/2;
}
SoundRecorder::SoundRecorder(){
	saveToFile = false;
	this->initFormat();
}
SoundRecorder::SoundRecorder(char *szFileName){
	FILE *fp;
	fopen_s(&fp, szFileName, "w");
	fclose(fp);
	stopRecord = false;
	saveToFile = true;
	this->szFileName = szFileName;
	this->initFormat();
}
bool SoundRecorder::StartRecord(){
	if (isRecording)
	{
		return false;
	}
	DWORD dwDevCount = waveInGetNumDevs();

	MMRESULT mmRlt = waveInGetDevCaps(0, &waveInCaps, sizeof(waveInCaps));
	if (mmRlt!=MMSYSERR_NOERROR)
	{
		return false;
	}
	mmRlt = waveInOpen(&phwi, WAVE_MAPPER, &waveFormat, (DWORD)MicCallback, (DWORD)this, CALLBACK_FUNCTION);
	if (mmRlt!=MMSYSERR_NOERROR)
	{
		return false;
	}
	pwh1.lpData = szBuf1;
	pwh1.dwBufferLength = 10240;
	pwh1.dwUser = 1;
	pwh1.dwFlags = 0;
	mmRlt = waveInPrepareHeader(phwi, &pwh1, sizeof(WAVEHDR));

	pwh2.lpData = szBuf2;
	pwh2.dwBufferLength = 10240;
	pwh2.dwUser = 1;
	pwh2.dwFlags = 0;
	mmRlt = waveInPrepareHeader(phwi, &pwh2, sizeof(WAVEHDR));

	if (mmRlt != MMSYSERR_NOERROR)
	{
		return false;
	}
	mmRlt = waveInAddBuffer(phwi, &pwh1, sizeof(WAVEHDR));
	mmRlt = waveInAddBuffer(phwi, &pwh2, sizeof(WAVEHDR));
	
	if (mmRlt != MMSYSERR_NOERROR)
	{
		return false;
	}
	mmRlt = waveInStart(phwi);
	isRecording = true;
	return true;
}
bool SoundRecorder::StopRecord(){
	if (!isRecording)
	{
		return false;
	}
	stopRecord = true;
	MMRESULT rlt = waveInStop(phwi);
	rlt = waveInReset(phwi);
	rlt = waveInClose(phwi);
	if (rlt!=MMSYSERR_NOERROR)
	{
		return false;
	}
	isRecording = false;
	return true;
}
DWORD CALLBACK SoundRecorder::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2){
	SoundRecorder *phwnd = (SoundRecorder *)dwInstance;
	PWAVEHDR whd = (PWAVEHDR)dwParam1;
	MMRESULT rlt;
	if (phwnd->stopRecord)
	{
		uMsg = WM_CLOSE;
	}
	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WIM_DATA:
		if (phwnd->saveToFile)
		{
			fopen_s(&phwnd->fp, phwnd->szFileName, "ab+");
			fwrite(whd->lpData, 1, whd->dwBufferLength, phwnd->fp);

		}
		waveInAddBuffer(hwavein, whd, sizeof(WAVEHDR));
		fclose(phwnd->fp);
		break;
	case WIM_CLOSE:
		rlt = waveInStop(phwnd->phwi);
		rlt = waveInReset(phwnd->phwi);
		rlt = waveInClose(phwnd->phwi);
		break;
	default:
		break;
	}

	return 0;
}
#endif