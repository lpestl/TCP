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

	// Инициализация Winsock версии 2.2
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// Так как Winsock не загрузился мы не можем использовать WSAGetLastError 
		// для определения ошибки. Код ошибки возвращает сама функция WSAStartup
		printf("WSAStartup ошибка %s\n", encodeWSAGetLastError(Ret));
		return;
	}

	// Создаём новый TCP сокет для приёма запросов на соединение от клиентов.

	if ((ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("ошибка socket %d\n", encodeWSAGetLastError(WSAGetLastError()));
		WSACleanup();
		return;
	}

	// Заполняем   struct sockaddr_in ServerAddr, которая скажет функции bind, что мы хотим
	// слушать соединения на всех интерфейсах (INADDR_ANY), используя порт 5150. 
	// Мы преобразуем порядок байт  из системного в сетевой (htons и htonl)
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Функция  bind привязывает адресную информацию, определённую  в ServerAddr к сокету  ListeningSocket
	if (bind(ListeningSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("Ошибка bind %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	// Делаем сокет пасивным для прослушивания (приёма) запросов на TCP-соединение от клиентов.
	// Длина очереди запросов  на соединение равна 5
	if (listen(ListeningSocket, 5) == SOCKET_ERROR)
	{
		printf("Ошибка listen %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	printf("Ожидаем соединение на порту %d.\n", Port);
	ClientAddrLen = sizeof(ClientAddr);

	// Принимаеи новое соединение, когда оно возникнет
	if ((NewConnection = accept(ListeningSocket, (struct sockaddr *) &ClientAddr, &ClientAddrLen)) == INVALID_SOCKET)
	{
		printf("Ошибка accept  %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(ListeningSocket);
		WSACleanup();
		return;
	}

	printf("Успешно соединились с %s:%d.\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
	// Далее мы можем снова ожидать на сокете ListeningSocket новые соединения снова, вызывая accept
	// и/или начать передавать и принимать данные на сокете NewConnection. 
	// ДЛя простоты остановим прослушивание, закрывая сокет ListeningSocket. 
	//Можно начнинать приём и предачу данных на сокете NewConnection.

	closesocket(ListeningSocket);
	// Для простоты ограничимся только приёмом
	printf("Ждём данные для получения..\n");
	if ((Ret = recv(NewConnection, DataBuffer, sizeof(DataBuffer), 0)) == SOCKET_ERROR)
	{
		printf("Ошибка recv %s\n", encodeWSAGetLastError(WSAGetLastError()));
		closesocket(NewConnection);
		WSACleanup();
		return;
	}

	// Делаем из полученных данных строку языка С
	DataBuffer[Ret] = '\0';
	printf("Успешно получено %d байтов в сообщении  %s.\n", Ret, DataBuffer);
	printf("Закрываем соединение с клиентом.\n");
	closesocket(NewConnection);

	// Выгружаем Winsock
	WSACleanup();
	printf("Нажмите Enter для завершения.\n");
	getchar();
}