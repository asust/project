//�ͻ���

#define	_CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<windows.h>
#include<locale.h>
using namespace std;

bool Judge(char *buf, size_t size)
{
	for (int i = 0; i < size; ++i)
	{
		if (buf[i] != 0)
		{
			return false;
		}
	}
	return true;
}

#pragma comment(lib, "ws2_32.lib")
int main()
{
	WORD wVersionRequested;  //typedef unsigned short WORD
	WSADATA wsaData;   //�ð���洢ϵͳ���صĹ���WinSocket������
	int err;  //�����ж����������Ƿ�����

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return -1;
	}

	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);   //AF_INET tcpip��Э��
															 //��ʼ������
	SOCKADDR_IN addrSrv;  //�������ĵ�ַ
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.74.129");
	addrSrv.sin_family = AF_INET;  //ʹ�õ���TCP/IP 
	addrSrv.sin_port = htons(8001);  //תΪ������  ���ö˿ں�

								   //���ӵ������� ʹ��SOCKET�������ӷ�����,����������ĵ�ַ��Ϣ  ǿת
	if (connect(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) < 0)  //Э�����  �׽��ֲ��� 
	{
		printf("connction faild!");
		closesocket(socketClient);
		return 0;
	}

	//������ӵ���  �Ǿ��ȴ��������ַ�����ȥ��������ִ�н����
	char recvBuf[512] = { 0 };
	char sendBuf[512] = { 0 };
	
//	FILE* fp = fopen("resultfile.txt", "w");//����б�Ҫ��д��һ���ļ�����ô��
//	if (!fp)
//		return 0;


	//д���ļ�
	while (1)
	{
		//���벢��������
		cin >> sendBuf;
		send(socketClient, sendBuf, 512, 0);
		//cout << "�Ѿ�����������ȴ����ܽ����" << endl;
		memset(sendBuf, 0, 512);

		while (1)
		{
			//���ս����
			int len = recv(socketClient, recvBuf, 512, 0);
		//	cout << "���ܽ����" << endl;
			//fwrite(recvBuf, 1, len, fp);
			cout << recvBuf << endl;
		//	cout << "len:" << len << endl;
		//	cout << "��ӡ�����" << endl;
			//printf("%s\n", recvBuf);
			
			if (Judge(recvBuf,512))
			{
		//		cout << "��Ҫ�˳����ܽ������ѭ����" << endl;
				break;
			}
			memset(recvBuf, 0, 512);
	//		cout << "��ջ�����" << endl;
		}

	}






	//char recvBuf[50] = {0};

	//int size = 10;
	//int i = 0;
	//while (i<100)
	//{
	//	//recv(socketClient, recvBuf, 50, 0);  //socket�����Ѿ����յ����ݣ����ڿ�ʼ��on����������ȡ����
	//    //printf("%s\n", recvBuf);
	//	send(socketClient, "123456789" , 50, 0);
	//	//Sleep(100);
	//}

	closesocket(socketClient);  //�ر�socket����

	WSACleanup();

	printf("Client exit!");
	system("pause");
	return 0;

}