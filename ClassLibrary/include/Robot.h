#ifndef __ROBOT__
#define __ROBOT__
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib,"winhttp.lib")
//������ ��
//�ϴ����֣���ȡ����
//����̨����ANSI��
//�ϴ����ı�����UTF-8
class Robot
{
private:
	wchar_t *ANSIToUnicodeResult;
	wchar_t *UTF8ToUnicodeResult;
	char *UnicodeToANSIResult;
	wchar_t apiString[1024];//�ϴ��İ�����Ϣ��api��ַ
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	LPSTR pszBuff;
protected:
	char *UnicodeToANSI(const wchar_t *str);
	wchar_t *UTF8ToUnicode(const char *str);
	wchar_t *ANSIToUnicode(const char* str);
	bool UpLoadInfo();//�ϴ���Ϣ
public:
	Robot() :ANSIToUnicodeResult(NULL), UTF8ToUnicodeResult(NULL), UnicodeToANSIResult(NULL){
		memset(apiString, 0, sizeof(apiString));
		hSession = hConnect = hRequest = NULL;
		LPSTR pszBuff = NULL;
	}//���캯��
	void GetText(char *);//��ȡ����̨�ı�
	char *GetResult();//��ȡ���
	~Robot(){
		if (!ANSIToUnicodeResult)
		{
			delete[]ANSIToUnicodeResult;
		}
		if (!UnicodeToANSIResult)
		{
			delete[]UnicodeToANSIResult;
		}
		if (!UTF8ToUnicodeResult)
		{
			delete[]UTF8ToUnicodeResult;
		}
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
	}
};

char *Robot::UnicodeToANSI(const wchar_t *str){
	int TextLen = 0;
	TextLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char *Result = new char[1024];
	WideCharToMultiByte(CP_ACP, 0, str, -1, Result, TextLen, NULL, NULL);
	Result[TextLen] = '\0';
	return Result;
}
wchar_t *Robot::UTF8ToUnicode(const char *str){
	int TextLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t *Result = new wchar_t[1024];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, Result, TextLen);
	Result[TextLen] = L'\0';
	return Result;
}
wchar_t *Robot::ANSIToUnicode(const char* str){
	int TextLen = 0;
	TextLen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t *Result = new wchar_t[1024];
	memset(Result, 0, sizeof(Result));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)Result, TextLen);
	return Result;
}

void Robot::GetText(char *pStr){
	ANSIToUnicodeResult = ANSIToUnicode(pStr);
	wsprintf(apiString, L"www.tuling123.com/openapi/api?key=1ed1e81e84eb4702b9b4ae38fe59550a&info=%s", ANSIToUnicodeResult);
}
bool Robot::UpLoadInfo(){
	int ret = false;
	hSession = WinHttpOpen(L"A Tuling API Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession)//����һ��http�����ӻỰ���������������У�����������Ҳ������IP��ַ������Ҫhttp;ǰ׺
	{
		hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	}

	if (hConnect)//����http������
		hRequest = WinHttpOpenRequest(hConnect, L"GET", apiString, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);

	if (hRequest)//���� ��������
		ret = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (ret)//��ȡ��Ӧ�����retΪ�棬��ͬ�ⷢ������
	{
		ret = WinHttpReceiveResponse(hRequest, NULL);
	}
	return ret;
}
char *Robot::GetResult(){
	if (!UpLoadInfo())
	{
		//cout << "�ϴ�����" << GetLastError();
		return NULL;
	}
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	int temp;
	int ret = false;
	do
	{
		dwSize = 0;//�ж��Ƿ�������
		if (!(temp = WinHttpQueryDataAvailable(hRequest, &dwSize)))
			break;
		if (!dwSize){
			ret = true;
			break;
		}

		pszBuff = new char[dwSize + 1];
		if (pszBuff == NULL)
		{
			ret = true;
			break;
		}
		ZeroMemory(pszBuff, dwSize + 1);

		if (!(temp = WinHttpReadData(hRequest, (LPVOID)pszBuff, dwSize, &dwDownloaded)))
		{
			//cout << "readdataerror";
			//system("pause");
			ret = true;
			break;
		}
		else
		{
			pszBuff[strlen(pszBuff) - 2] = '\0';
			UTF8ToUnicodeResult = UTF8ToUnicode(pszBuff);
			UnicodeToANSIResult = UnicodeToANSI(UTF8ToUnicodeResult + 23);
		}
		delete[] pszBuff;
		if (!dwDownloaded)
			break;
	} while (dwSize > 0);
	return UnicodeToANSIResult;
}
#endif