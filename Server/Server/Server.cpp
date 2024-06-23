#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <process.h>

#include <mutex>
#include <thread>

#include <vector>
#include <string>
#include "Server.h"

using namespace std;

#define BUF_SIZE 100


int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAddr, recvAddr;

    int recvAddrSize;
    
    WSABUF dataBuf;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

    char buf[BUF_SIZE];
    unsigned long recvBytes = 0;
    unsigned long flags = 0;

    // 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    // 소켓 생성
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hLisnSock == INVALID_SOCKET)
    {
        cout << "Error: socket()";
        return 1;
    }

    lisnAddr = {};
    lisnAddr.sin_family = AF_INET;
    lisnAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAddr.sin_port = htons(8888);

    if ((bind(hLisnSock, (SOCKADDR*)&lisnAddr, sizeof(lisnAddr))) == SOCKET_ERROR)
    {
        cout << "Error: bind()";
        return 1;
    }
    
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
    {
        cout << "Error: listen()";
        return 1;
    }

    recvAddrSize = sizeof(recvAddr);
    hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAddr, &recvAddrSize);

    evObj = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;
    dataBuf.buf = buf;
    dataBuf.len = BUF_SIZE;

    if (WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            cout << "Background data receive..." << endl;

            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);

            WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);
        }
        else
        {
            cout << "ERROR: WSARecv()" << endl;
            return 1;
        }
    }

    cout << "Received message: " << buf;

    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();

    return 0;
}