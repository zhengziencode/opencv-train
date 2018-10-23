#ifndef _UDPZZE_
#define _UDPZZE_

#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib")
#define BUFFSIZE 456
enum Options
{
	SEND=1,
	RECIEVE
};
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define START "start64444"
#define CONTINUE "continue64"
#define END "end6444444"
#define STARTANDEND "startanded"
#define ENDWITHNOLEFT "endnoleft"
#define PASSWORD "mojoLuginslandzze"
#define PORT 9537
class udpFileTranform{
private:
	int flag;//flag=1 send files as client do not request ip address
			 //flag=2 recieve files as server request ip address
			 //original value is -1
	int fileLen;
	bool initalized;
	bool addressInitalized;
	std::string fileName;
	std::string ipAddr;
	std::string localIpaddr;
	SOCKET Sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
protected:
	bool ReadFilesLength();
	bool sendFile();
	bool recieveFile();
	bool getlocalhostip();
public:
	udpFileTranform() :flag(-1), fileLen(0),initalized(false),addressInitalized(false){}
	bool initAll(int nOptions = -1, char *ipAddr="127.0.0.1");
	bool isInitalized(){ return initalized; }
	bool operate(char *fileName = NULL);
	std::string showIpaddress(){
		getlocalhostip();
		return localIpaddr;
	}
	~udpFileTranform(){
		closesocket(Sock);
		WSACleanup();
	}
};
bool udpFileTranform::getlocalhostip(){
	bool returnValue = false;
	do 
	{
		if (isInitalized())
		{
			char host[256] = { 0 };
			if (gethostname(host, sizeof(host)) == SOCKET_ERROR)
			{
				printf("Get host name failed!error code %d\n", GetLastError());
				break;
			}
			hostent *hostMsg = gethostbyname(host);
			if (hostMsg == NULL)
			{
				printf("Get ip address failed\n");
				break;
			}
			char tempIp[20] = { 0 };
			sprintf_s(tempIp, "%d.%d.%d.%d",
				hostMsg->h_addr_list[0][0] & 0x0ff,
				hostMsg->h_addr_list[0][1] & 0x0ff,
				hostMsg->h_addr_list[0][2] & 0x0ff,
				hostMsg->h_addr_list[0][3] & 0x0ff);
			localIpaddr = tempIp;
		}
		returnValue = true;
	} while (false);
	return returnValue;
}
bool udpFileTranform::initAll(int nOptions,char *ipAddr){
	if (nOptions==-1)
	{
		printf("Error!Please choose ur option first!\n");
		return false;
	}

	this->ipAddr = ipAddr;
	flag = nOptions;
	int ret = -1;
	bool bflag = false;
	do 
	{
		WSADATA vVersion;
		ret = WSAStartup(MAKEWORD(2, 2), &vVersion);
		if (ret)
		{
			break;
		}
		Sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (Sock==INVALID_SOCKET)
		{
			break;
		}
		bflag = true;
	} while (false);
	initalized = true;
	return bflag;
}
bool udpFileTranform::operate(char *fileName){
	this->fileName = fileName;
	int ret = 0;
	bool fflag = false;
	do
	{
		if (flag == SEND)
		{
			if (!addressInitalized)
			{
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(PORT);
				//serverAddr.sin_addr.S_un.S_addr = inet_addr(ipAddr.c_str());
				serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
				int ret = bind(Sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
				if (ret == SOCKET_ERROR)
				{
					break;
				}
				do
				{
					char recvBuf[BUFFSIZE] = { 0 };
					int AddrLen = sizeof(clientAddr);
					recvfrom(Sock, recvBuf, BUFFSIZE, 0, (sockaddr*)&clientAddr, &AddrLen);
					if (strcmp(recvBuf, PASSWORD) == 0)
					{
						break;
					}
				} while (true);
				addressInitalized = true;
			}
			
			/*
			1.Get file length
			2.Set transform symbol
			3.send file
			*/

			//get file length     ----------fileLen
			if (!ReadFilesLength())
			{
				break;
			}
			
			
			//send file
			if (!sendFile())
			{
				break;
			}
			//printf("The file have been sent!\n");
		}
		else if (flag == RECIEVE)
		{
			if (!addressInitalized)
			{
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(PORT);
				serverAddr.sin_addr.S_un.S_addr = inet_addr(ipAddr.c_str());
				sendto(Sock, PASSWORD, sizeof(PASSWORD), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
				addressInitalized = true;
			}
			
// 			int ret = bind(Sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
// 			if (ret == SOCKET_ERROR)
// 			{
// 				break;
// 			}
			if (!recieveFile())
			{
				break;
			}
			else
			{
				printf("File have been recieved!\n");
			}
		}
		fflag = true;
	} while (false);

	return fflag;
}
bool udpFileTranform::ReadFilesLength(){
	FILE *fp = NULL;
	fopen_s(&fp, fileName.c_str(), "a+b");
	if (!fp)
	{
		return false;
	}
	fseek(fp, 0L, SEEK_END);
	fileLen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (!fileLen)
	{
		printf("error file length is 0\n");
	}
	else
	{
		//std::cout << "File length = " << fileLen << std::endl;
	}
	fclose(fp);
	return true;
}
bool udpFileTranform::sendFile(){
	if (!fileLen)
	{
		return false;
	}
	FILE *fp;
	fopen_s(&fp, fileName.c_str(), "a+b");
	if (fp==NULL)
	{
		return false;
	}
	int count = (int)fileLen / BUFFSIZE;//sending loops
	int lastBytes = fileLen % BUFFSIZE;//the bytes of final package.

	std::string status = START;
	if (count==0 && lastBytes!=0)
	{ 
		status = STARTANDEND;
	}
	int ret = -1;
	//Sleep(20);
	while (count!=0 && count--)
	{
		//Sleep(1);
		char buf[BUFFSIZE+10] = { 0 };
// 		sprintf_s(buf, "%d", fileLen);
// 		sendto(Sock, buf, 466, 0, (sockaddr*)&serverAddr, sizeof(sockaddr_in));

		memcpy(buf, status.c_str(), 10);	
		int readLen = fread(buf+10, 1 ,BUFFSIZE, fp);
		sendto(Sock, buf, BUFFSIZE+10, 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
		if (status==START)
		{
			status = CONTINUE;
		}
		else if (status==STARTANDEND)
		{
			break;
		}
	}
	if (lastBytes==0)
	{
		sendto(Sock, ENDWITHNOLEFT, 10, 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
	}
	else if (lastBytes!=0 && status!=STARTANDEND)
	{
		char buf[BUFFSIZE+10] = { 0 };
		memcpy(buf, END, 10);
		int readLen = fread(buf+10, 1 ,lastBytes, fp);
		sendto(Sock, buf, lastBytes+10, 0, (sockaddr*)&clientAddr, sizeof(sockaddr_in));
	}
	fclose(fp);
	printf("sent\n");
	return true;
}

bool udpFileTranform::recieveFile(){
	FILE *fp = NULL;
	fopen_s(&fp, fileName.c_str(), "w");
	fclose(fp);
	fp = NULL;
	fopen_s(&fp, fileName.c_str(), "a+b");
	if (fp == NULL)
	{
		return false;
	}
	char buf[BUFFSIZE+10] = { 0 };
	std::string status = START;
	int len = sizeof(clientAddr);
	int readLen = 0;

// 	/*recvfrom(Sock, buf, 466, 0, (sockaddr*)&clientAddr, &len);*/
// 	/*int fileLLen = atoi(buf);*/
// 	printf("fileLLen==%d\n", fileLLen);
	while (status!=END)
	{
		memset(buf, 0, BUFFSIZE+10);
		recvfrom(Sock, buf,BUFFSIZE+10, 0, (sockaddr*)&clientAddr, &len);
		status = std::string(buf, 10);
		readLen+=fwrite(buf+10, 1, BUFFSIZE, fp);
		//printf("done,%d bytes been read\n", readLen);
		if (status==ENDWITHNOLEFT)
		{
			break;
		}
		if (status==STARTANDEND)
		{
			break;
		}
	}
	//printf("done,%d bytes been read\n",readLen);
	fclose(fp);
	return true;
}
#endif