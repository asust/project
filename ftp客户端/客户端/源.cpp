//�ͻ���

#define	_CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>;
#include<vector>
#include<string>
using namespace std;

void MainMenu()
{
	printf("��ѡ�����\n");
	printf("1.��ʾ��ǰ�ļ��е������ļ�\n");
	printf("2.������һ��\n");
	printf("3.ѡ���ļ���\n");
	printf("4.�˳�\n");
	//printf("5.�˳�\n");
}

void Showfilelist(vector<string>& filelist)
{
	for (int i = 0; i < filelist.size(); ++i)
	{
		cout << filelist[i] << endl;
	}

}

string ExecuteCommand(SOCKET &socketClient)
{
	string filename;
	char *command = new char(0);
	while (1)
	{
		system("CLS");
		MainMenu();
		scanf("%d", command);
		getchar();
		*command += '0';
		send(socketClient, command, 1, 0);
		vector<string> filelist;
		//Ȼ��͵ý���
		string Path;
		if (*command == '3')
		{
			cin >> Path;
			send(socketClient, (char *)Path.c_str(), Path.size(), 0);
		}

		if (*command == '4')
		{
			
			cin >> filename;
			send(socketClient, (char *)filename.c_str(), filename.size(), 0);

			break;
		}
		char Buf[260] = { 0 };
		while (1)
		{
			recv(socketClient, Buf, 260, 0);
			filelist.push_back(Buf);
			memset(Buf, 0, 100);
			if (filelist.back() == "transform end")
			{
				filelist.pop_back();
				break;
			}

		}
		Showfilelist(filelist);
		system("pause");
	}
	return filename;
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
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;  //ʹ�õ���TCP/IP 
	addrSrv.sin_port = htons(8001);  //תΪ������  ���ö˿ں�

								   //���ӵ������� ʹ��SOCKET�������ӷ�����,����������ĵ�ַ��Ϣ  ǿת
	if (connect(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) < 0)  //Э�����  �׽��ֲ��� 
	{
		printf("connction faild!");
		closesocket(socketClient);
		return 0;
	}


	//������ӵ���  �Ǿ��ȷ���һ������
	string filename = ExecuteCommand(socketClient);

	//char filename[256] = { 0 };
	char recvBuf[512] = { 0 };

	//int len = recv(socketClient, filename, 256, 0);

	//printf("%d\n", len);


	//printf("%s\n", filename);
	//�����ļ�
	FILE* fp = fopen(filename.c_str(), "wb");
	cout << "�ɹ����ļ�" << filename << endl;
	if (!fp)
		return 0;


	//д���ļ�
	while (1)
	{
		cout << "������" << endl;
		int len = recv(socketClient, recvBuf, 512, 0);
		cout << "����" << endl;
		fwrite(recvBuf, 1, len, fp);
		cout << "��д" << endl;
		memset(recvBuf, 0, 512);
		cout << "����" << endl;
		if (len < 1)
		{
			cout << "�˳�" << endl;
			break;
		}
	}
	printf("�ļ��������\n");




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
