#include <winsock2.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <wtypes.h>
#include <locale>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//int myprintf(const char *lpFormat, ...);
char* encodeWSAGetLastError(int n){
	switch (n){
	case WSAEINTR: return " WSAEINTR: ���������� ����� �������."; break;
	case WSAEACCES: return " WSAEACCES: ������ ��������.";  break;

	default: return "����������� ������"; break;
	}
}

void main(int argc, char **argv)
{
	setlocale(LC_ALL, "Russian");

	WSADATA wsaData;
	SOCKET s;

	struct sockaddr_in ServerAddr, ca;
	int Port = 5150;
	int Ret;
	char *msg = "������";
	int lmsg = strlen(msg);
	int caSize = sizeof(ca);
	struct sockaddr_in ServerAdd;

	ZeroMemory(&ServerAdd, sizeof(ServerAdd));
	ZeroMemory(&ca, sizeof(caSize));

	if (argc <= 1)
	{
		printf("�������������: tcpclient <Server IP address>.\n");
		getch();
		return;
	}

	// ������������� Winsock ������ 2.2
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// ��� ��� Winsock �� ���������� �� �� ����� ������������ WSAGetLastError
		// ��� ����������� ������. ��� ������ ���������� ���� ������� WSAStartup
		printf("WSAStartup ������ %d\n", encodeWSAGetLastError(Ret));
		return;
	}

	// �������� ����� ����� ��� ���������� ����� TCP �������
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("socket ������ %d\n", encodeWSAGetLastError(WSAGetLastError()));
		WSACleanup();
		return;
	}

	// �������� � ���������� ServerAddr ���� struct sockaddr_in �������� ���������� � �������,
	//� �������� �� ����� �����������. ������ ������� ���� 5150. IP-����� ������� �������
	//� ��������� ������
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = inet_addr(argv[1]);
	// �������� ����� s � ��������

	printf("������� ���������� � %s:%d...\n",
		inet_ntoa(ServerAddr.sin_addr), htons(ServerAddr.sin_port));
	if (connect(s, (struct sockaddr*) &ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("������ connect %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(s);
		WSACleanup();
		return;
	}

	printf("���� ���������� �������.\n");

	// ���������� �� �������� ������ � ����� ���������� ����������� ������ sc ����� �������� ���
	getsockname(s, (struct sockaddr *)&ca, &caSize);
	printf("��������� �����:%s � ����:%d ������ �������\n", inet_ntoa(ca.sin_addr), ntohs(ca.sin_port));
	// ������ �� ����� ������������ ������� � �������� ����� ����� s
	// �� ������ ��������� ������� ��������� "������"
	printf("������� �������� ��������� ������.\n");

	if ((Ret = send(s, msg, lmsg, 0)) == SOCKET_ERROR)
	{
		printf("������ send %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(s);
		WSACleanup();
		return;
	}

	printf("������� �������� %d ������ ��������� %s.\n", Ret, msg);
	printf("��������� ���������� � ��������.\n");
	closesocket(s);
	// ��������� Winsock
	WSACleanup();
	printf("������� Enter ��� ����������.\n");

	getchar();
}
