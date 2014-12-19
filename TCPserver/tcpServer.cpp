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

void main(void){
	setlocale(LC_ALL, "Russian");

	WSADATA              wsaData;
	SOCKET               ListeningSocket;
	SOCKET               NewConnection;
	struct sockaddr_in   ServerAddr;
	struct sockaddr_in   ClientAddr;
	int                  ClientAddrLen;
	int                  Port = 5150;
	int                  Ret;
	char                 DataBuffer[1024];

	ZeroMemory(&ServerAddr, sizeof(ServerAddr));
	ZeroMemory(&ClientAddr, sizeof(ClientAddr));

	// ������������� Winsock ������ 2.2
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// ��� ��� Winsock �� ���������� �� �� ����� ������������ WSAGetLastError 
		// ��� ����������� ������. ��� ������ ���������� ���� ������� WSAStartup
		printf("WSAStartup ������ %s\n", encodeWSAGetLastError(Ret));
		return;
	}

	// ������ ����� TCP ����� ��� ����� �������� �� ���������� �� ��������.

	if ((ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("������ socket %d\n", encodeWSAGetLastError(WSAGetLastError()));
		WSACleanup();
		return;
	}

	// ���������   struct sockaddr_in ServerAddr, ������� ������ ������� bind, ��� �� �����
	// ������� ���������� �� ���� ����������� (INADDR_ANY), ��������� ���� 5150. 
	// �� ����������� ������� ����  �� ���������� � ������� (htons � htonl)
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// �������  bind ����������� �������� ����������, �����������  � ServerAddr � ������  ListeningSocket
	if (bind(ListeningSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("������ bind %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	// ������ ����� �������� ��� ������������� (�����) �������� �� TCP-���������� �� ��������.
	// ����� ������� ��������  �� ���������� ����� 5
	if (listen(ListeningSocket, 5) == SOCKET_ERROR)
	{
		printf("������ listen %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	printf("������� ���������� �� ����� %d.\n", Port);
	ClientAddrLen = sizeof(ClientAddr);

	// ��������� ����� ����������, ����� ��� ���������
	if ((NewConnection = accept(ListeningSocket, (struct sockaddr *) &ClientAddr, &ClientAddrLen)) == INVALID_SOCKET)
	{
		printf("������ accept  %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	printf("������� ����������� � %s:%d.\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
	// ����� �� ����� ����� ������� �� ������ ListeningSocket ����� ���������� �����, ������� accept
	// �/��� ������ ���������� � ��������� ������ �� ������ NewConnection. 
	// ��� �������� ��������� �������������, �������� ����� ListeningSocket. 
	//����� ��������� ���� � ������� ������ �� ������ NewConnection.

	closesocket(ListeningSocket);
	// ��� �������� ����������� ������ ������
	printf("��� ������ ��� ���������..\n");
	if ((Ret = recv(NewConnection, DataBuffer, sizeof(DataBuffer), 0)) == SOCKET_ERROR)
	{
		printf("������ recv %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(NewConnection);
		WSACleanup();
		return;
	}

	// ������ �� ���������� ������ ������ ����� �
	DataBuffer[Ret] = '\0';
	printf("������� �������� %d ������ � ���������  %s.\n", Ret, DataBuffer);
	printf("��������� ���������� � ��������.\n");
	closesocket(NewConnection);

	// ��������� Winsock
	WSACleanup();
	printf("������� Enter ��� ����������.\n");
	getchar();
}