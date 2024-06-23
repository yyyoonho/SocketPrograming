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

WSABUF dataBuf;
char buf[BUF_SIZE];
unsigned long recvBytes = 0;

void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{

}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAddr, recvAddr;

    int recvAddrSize;
    
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

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

    evObj = WSACreateEvent(); // Dummy event object (오로지 함수 호출을 위해 존재)
    memset(&overlapped, 0, sizeof(overlapped)); 
    overlapped.hEvent = evObj;
    dataBuf.buf = buf;
    dataBuf.len = BUF_SIZE;

    // Dummy event object 여기에 활용!
    if (WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, CompRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSA_IO_PENDING)
        {
            cout << "Background data receive..." << endl;
        }
    }

    // 정말 Alertable Wait 상태 진입만을 위한 목적으로 함수호출
    // Dummy event object 여기에 활용!
    // Completion Routine으로 등록된 함수가 끝나면 리턴된다.
    int idx = WSAWaitForMultipleEvents(1, &evObj, FALSE, WSA_INFINITE, TRUE);
    if (idx == WAIT_IO_COMPLETION)
    {
        cout << "Overlapped IO Completed!" << endl;
    }
    else
    {
        cout << "ERROR: WSARecv()" << endl;
    }

    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();

    return 0;
}