#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Windows.h>

#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#define BUF_SIZE 100

char msg[]="Hello. Overlapped IO (WSASend).";

int main()
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;
    WSABUF dataBuf;
    unsigned long sendBytes = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hSocket == -1)
    {
        cout << "Error: socket()";
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(8888);

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        cout << "Error: connect()";
        return 1;
    }

    evObj = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;
    dataBuf.buf = msg;
    dataBuf.len = strlen(msg) + 1;

    if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            cout << "Background data send" << endl;

            // WSAWaitForMultipleEvents() 를 통해서 데이터 전송의 완료를 기다리지 않아도
            // WSAGetOverlappedResult()의 FALSE를 TRUE로 만들어서 전송의 완료를 기다리고, 확인할 수 도 있다.
            // 하지만, Event 오브젝트가 Manuel-Reset모드이고, 전송의 완료를 기다리고 결과를 확인하는 절차를 분리해서도 구현이 가능하다는 것을 알기위해
            // 분리해서 구현했다.

            // WSAWaitForMultipleEvents() 를 통해서 데이터 전송의 완료를 기다린다.
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);

            // 데이터 전송완료가 확인되면 WSAGetOverlappedResult() 를 통해서 전송결과를 확인할 수 있다.
            WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
        }
        else
        {
            cout << "Error: WSASend()" << endl;
            return 1;
        }
    }

    cout << "Send Data Size: " << sendBytes << endl;

    WSACloseEvent(evObj);
    closesocket(hSocket);
    WSACleanup();

    return 0;

}