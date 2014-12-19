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
	case WSAEINTR: return " WSAEINTR: Прерванный вызов функции."; break;
	case WSAEACCES: return " WSAEACCES: Доступ запрещен.";  break;

	default: return "Неизвестная ошибка"; break;
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
	char *msg = "Привет";
	int lmsg = strlen(msg);
	int caSize = sizeof(ca);
	struct sockaddr_in ServerAdd;

	ZeroMemory(&ServerAdd, sizeof(ServerAdd));
	ZeroMemory(&ca, sizeof(caSize));

	if (argc <= 1)
	{
		printf("Использование: tcpclient <Server IP address>.\n");
		getch();
		return;
	}

	// Инициализация Winsock версии 2.2
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// Так как Winsock не загрузился мы не можем использовать WSAGetLastError
		// для определения ошибки. Код ошибки возвращает сама функция WSAStartup
		printf("WSAStartup ошибка %d\n", encodeWSAGetLastError(Ret));
		return;
	}

	// Создадим новый сокет для соединения этого TCP клиента
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("socket ошибка %d\n", encodeWSAGetLastError(WSAGetLastError()));
		WSACleanup();
		return;
	}

	// Заполним в переменной ServerAddr типа struct sockaddr_in адресную информацию о сервере,
	//к которому мы хотим подключится. Сервер слушает порт 5150. IP-адрес сервера задаётся
	//в командной строке
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = inet_addr(argv[1]);
	// Соединим сокет s с сервером

	printf("Попытка соединения к %s:%d...\n",
		inet_ntoa(ServerAddr.sin_addr), htons(ServerAddr.sin_port));
	if (connect(s, (struct sockaddr*) &ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("Ошибка connect %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(s);
		WSACleanup();
		return;
	}

	printf("Наше соединение успешно.\n");

	// Информацию об истинном адресе и порте локального соединённого сокета sc можно получить так
	getsockname(s, (struct sockaddr *)&ca, &caSize);
	printf("Локальный адрес:%s и порт:%d сокета клиента\n", inet_ntoa(ca.sin_addr), ntohs(ca.sin_port));
	// Теперь мы можем обмениваться данными с сервером через сокет s
	// Мы просто передадим серверу сообщение "Привет"
	printf("Попытка передать сообщение Привет.\n");

	if ((Ret = send(s, msg, lmsg, 0)) == SOCKET_ERROR)
	{
		printf("Ошибка send %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(s);
		WSACleanup();
		return;
	}

	printf("Успешно передано %d байтов сообщения %s.\n", Ret, msg);
	printf("Закрываем соединение с сервером.\n");
	closesocket(s);
	// Выгружаем Winsock
	WSACleanup();
	printf("Нажмите Enter для завершения.\n");

	getchar();
}
