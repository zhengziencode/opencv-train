#ifndef __URLDECODEENCODE__
#define __URLDECODEENCODE__
#include <stdio.h>
#include <string.h>
#include <string>
#include <windows.h>
#define NON_NUM '0'  
class urlDecodeEncode{
private:
	std::string m_decode;//����
	std::string m_encode;//����
	wchar_t *ANSIToUnicodeResult;
	wchar_t *UTF8ToUnicodeResult;
	char *UnicodeToANSIResult;
protected:
	int hex2num(char c);
public:
	urlDecodeEncode();
	char *UnicodeToANSI(const wchar_t *str);
	wchar_t *UTF8ToUnicode(const char *str);
	wchar_t *ANSIToUnicode(const char* str);
	int URLEncode(const char* str);//����
	int URLDecode(const char* str);//����
	std::string GetDecodedText(){
		if (m_decode != "")
		{
			return m_decode;
		}
		else
			return NULL;
	}
	std::string GetEncodedText(){
		if (m_encode != "")
		{
			return m_encode;
		}
		else
			return NULL;
	}
	~urlDecodeEncode(){
		if (ANSIToUnicodeResult)
		{
			delete[]ANSIToUnicodeResult;
		}
		else if (ANSIToUnicodeResult)
		{
			delete[]ANSIToUnicodeResult;
		}
		else if (UnicodeToANSIResult)
		{
			delete[]UnicodeToANSIResult;
		}
	}
};
urlDecodeEncode::urlDecodeEncode() :ANSIToUnicodeResult(NULL), UTF8ToUnicodeResult(NULL), UnicodeToANSIResult(NULL){}
int urlDecodeEncode::URLEncode(const char* str)
{
	const int resultSize = 1024;
	char result[resultSize] = { 0 };
	int strSize = strlen(str);
	int i;
	int j = 0;//for result index  
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) {
		return 0;
	}

	for (i = 0; (i < strSize) && (j < resultSize); ++i) {
		ch = str[i];
		if (((ch >= 'A') && (ch < 'Z')) ||
			((ch >= 'a') && (ch < 'z')) ||
			((ch >= '0') && (ch < '9'))) {
			result[j++] = ch;
		}
		else if (ch == ' ') {
			result[j++] = '+';
		}
		else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
			result[j++] = ch;
		}
		else {
			if (j + 3 < resultSize) {
				sprintf(result + j, "%%%02X", (unsigned char)ch);
				j += 3;
			}
			else {
				return 0;
			}
		}
	}

	result[j] = '\0';
	m_encode = result;
	return j;
}


int urlDecodeEncode::hex2num(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'z') return c - 'a' + 10;//����+10��ԭ����:����16���Ƶ�aֵΪ10
	if (c >= 'A' && c <= 'Z') return c - 'A' + 10;

	printf("unexpected char: %c", c);
	return NON_NUM;
}

/**
* @brief URLDecode ���ַ���URL����,����������
*
* @param str ԭ�ַ���
* @param strSize ԭ�ַ�����С������������\0��
* @param result ����ַ���������
* @param resultSize �����ַ�Ļ�������С(��������\0)
*
* @return: >0 result ��ʵ����Ч���ַ�������
*            0 ����ʧ��
*/

int urlDecodeEncode::URLDecode(const char* str)
{
	const int resultSize = 1024;
	char result[resultSize] = { 0 };
	int strSize = strlen(str);
	char ch, ch1, ch2;
	int i;
	int j = 0;//record result index

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) {
		return 0;
	}

	for (i = 0; (i<strSize) && (j<resultSize); ++i) {
	ch = str[i];
	switch (ch) {
	case '+':
	result[j++] = ' ';
		break;
	case '%':
	if (i + 2<strSize) {
	ch1 = hex2num(str[i + 1]);//��4λ
	ch2 = hex2num(str[i + 2]);//��4λ
	if ((ch1 != NON_NUM) && (ch2 != NON_NUM))
		result[j++] = (char)((ch1 << 4) | ch2);
		i += 2;
		break;
	}
	else {
		break;
	}
	default:
		result[j++] = ch;
		break;
	}
		}

	result[j] = 0;
	m_decode = result;
	return j;
}

char *urlDecodeEncode::UnicodeToANSI(const wchar_t *str){
	if (UnicodeToANSIResult!=NULL)
	{
		delete[]UnicodeToANSIResult;
	}
	int TextLen = 0;
	TextLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	UnicodeToANSIResult = new char[1024];
	WideCharToMultiByte(CP_ACP, 0, str, -1, UnicodeToANSIResult, TextLen, NULL, NULL);
	UnicodeToANSIResult[TextLen] = '\0';
	return UnicodeToANSIResult;
}
wchar_t *urlDecodeEncode::UTF8ToUnicode(const char *str){
	if (UTF8ToUnicodeResult!=NULL)
	{
		delete[]UTF8ToUnicodeResult;
	}
	int TextLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	UTF8ToUnicodeResult = new wchar_t[1024];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, UTF8ToUnicodeResult, TextLen);
	UTF8ToUnicodeResult[TextLen] = L'\0';
	return UTF8ToUnicodeResult;
}
wchar_t *urlDecodeEncode::ANSIToUnicode(const char* str){
	if (ANSIToUnicodeResult!=NULL)
	{
		delete[]ANSIToUnicodeResult;
	}
	int TextLen = 0;
	TextLen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	ANSIToUnicodeResult = new wchar_t[1024];
	memset(ANSIToUnicodeResult, 0, sizeof(ANSIToUnicodeResult));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)ANSIToUnicodeResult, TextLen);
	return ANSIToUnicodeResult;
}
#endif