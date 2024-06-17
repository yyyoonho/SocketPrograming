#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#define BUF_SIZE 100
#define NAME_SIZE 20

char name[NAME_SIZE];
char msg[BUF_SIZE];

unsigned WINAPI SendMsg(void* arg)
{
    SOCKET hSocket = *((SOCKET*)arg);

    char nameMsg[NAME_SIZE + BUF_SIZE];
    string inputString;

    while (true)
    {
        getline(cin, inputString);

        if (inputString == "Q" || inputString == "q")
        {
            closesocket(hSocket);
            exit(0);
        }

        sprintf_s(nameMsg, "%s %s", name, inputString.c_str());

        send(hSocket, nameMsg, strlen(nameMsg), 0);
    }

    return 0;
}

unsigned WINAPI RecvMsg(void* arg)
{
    SOCKET hSocket = *((SOCKET*)arg);

    char nameMsg[NAME_SIZE + BUF_SIZE];

    while (true)
    {
        int strLen = recv(hSocket, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
        if (strLen == -1)
        {
            return -1;
        }

        nameMsg[strLen] = 0;
        cout << nameMsg << endl;
    }

    return 0;
}

int main()
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;
    HANDLE hThread1, hThread2;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    string inputName;
    cout << "Name? ";
    cin >> inputName;
    sprintf_s(name, "[%s]", inputName.c_str());

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
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

    hThread1 = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSocket, 0, NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSocket, 0, NULL);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    closesocket(hSocket);
    WSACleanup();

    return 0;

}