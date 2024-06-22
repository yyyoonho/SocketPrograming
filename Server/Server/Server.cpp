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

void CompressSockets(SOCKET hSockArr[], int idx, int total)
{
    for (int i = idx; i < total; i++)
    {
        hSockArr[i] = hSockArr[i + 1];
    }
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
    for (int i = idx; i < total; i++)
    {
        hEventArr[i] = hEventArr[i + 1];
    }
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS]; // 64
    WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT newEvent;
    WSANETWORKEVENTS netEvents;

    int numOfClntSock = 0;
    int strLen;
    int posInfo;
    int startIdx;
    int clntAdrLen;
    char msg[BUF_SIZE];

    // 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    // 소켓 생성
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
    {
        cout << "Error: socket()";
        return 1;
    }

    servAddr = {};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(8888);

    if ((bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) == SOCKET_ERROR)
    {
        cout << "Error: bind()";
        return 1;
    }
    
    if (listen(hServSock, 5) == SOCKET_ERROR)
    {
        cout << "Error: listen()";
        return 1;
    }

    newEvent = WSACreateEvent();
    if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
    {
        cout << "Error: ServSock: WSAEventSelect()";
        return 1;
    }

    hSockArr[numOfClntSock] = hServSock;
    hEventArr[numOfClntSock] = newEvent;
    numOfClntSock++;

    while (true)
    {
        posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
        startIdx = posInfo - WSA_WAIT_EVENT_0;

        for (int i = startIdx; i < numOfClntSock; i++)
        {
            int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);

            if ((sigEventIdx == WSA_WAIT_FAILED) || (sigEventIdx == WSA_WAIT_TIMEOUT))
            {
                continue;
            }
            else
            {
                sigEventIdx = i;
                WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

                if (netEvents.lNetworkEvents & FD_ACCEPT) // 연결 요청 시
                {
                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                    {
                        cout << "Eccept Error";
                        break;
                    }

                    clntAdrLen = sizeof(clntAddr);
                    hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAddr, &clntAdrLen);
                    newEvent = WSACreateEvent();

                    WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

                    hEventArr[numOfClntSock] = newEvent;
                    hSockArr[numOfClntSock] = hClntSock;
                    numOfClntSock++;

                    cout << "Connected New Client..." << endl;
                }

                if (netEvents.lNetworkEvents & FD_READ) // 데이터 수신 시
                {
                    if (netEvents.iErrorCode[FD_READ_BIT] != 0)
                    {
                        cout << "Read Error";
                        break;
                    }

                    strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
                    send(hSockArr[sigEventIdx], msg, strLen, 0);
                }

                if (netEvents.lNetworkEvents & FD_CLOSE)
                {
                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
                    {
                        cout << "Close Error";
                        break;
                    }

                    WSACloseEvent(hEventArr[sigEventIdx]);
                    closesocket(hSockArr[sigEventIdx]);

                    numOfClntSock--;

                    CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
                    CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}