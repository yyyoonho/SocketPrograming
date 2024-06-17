#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <process.h>

#include <vector>
#include <string>

using namespace std;

#define BUF_SIZE 100
#define MAX_CLNT 256

int clnt_cnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

void SendMsg(char* msg, int len)
{
    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < clnt_cnt; i++)
    {
        send(clntSocks[i], msg, len, 0);
    }
    ReleaseMutex(hMutex);
}

unsigned WINAPI HandleClnt(void* arg)
{
    SOCKET hClntSock = *((SOCKET*)arg);
    int strLen = 0;
    char msg[BUF_SIZE];

    while ((strLen = recv(hClntSock, msg, BUF_SIZE - 1, 0)) != 0)
    {
        SendMsg(msg, strLen);
    }

    // Remove disconnted client
    WaitForSingleObject(hMutex, INFINITE); // Lock
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (hClntSock == clntSocks[i])
        {
            while (i++ < clnt_cnt - 1)
            {
                clntSocks[i] = clntSocks[i + 1];
            }
            break;
        }
    }

    clnt_cnt--;
    ReleaseMutex(hMutex);                  // Unlock
    closesocket(hClntSock);

    return 0;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSize = 0;
    HANDLE hThread;

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

    while (true)
    {
        clntAddrSize = sizeof(clntAddr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrSize);

        WaitForSingleObject(hMutex, INFINITE);  // Lock
        clntSocks[clnt_cnt++] = hClntSock;
        ReleaseMutex(hMutex);                   // UnLock

        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);

        cout << "Connected Client IP: " << inet_ntoa(clntAddr.sin_addr) << "\n";
    }


    closesocket(hServSock);
    WSACleanup();

    return 0;
}