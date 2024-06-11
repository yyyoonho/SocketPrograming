#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#define BUF_SIZE 30

void Test()
{
    string inputMessage;
    int inputMessageSize = 0;

    cout << "메시지를 입력하세요: ";
    cin >> inputMessage;

    inputMessageSize = inputMessage.size();

    cout << inputMessageSize;
}

int main()
{
    //Test();

    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN clntAddr;

    int strLen=0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    hSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSocket == -1)
    {
        cout << "Error: socket()";
    }

    memset(&clntAddr, 0, sizeof(clntAddr));
    clntAddr.sin_family = AF_INET;
    clntAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clntAddr.sin_port = htons(8888);

    if ((bind(hSocket, (SOCKADDR*)&clntAddr, sizeof(clntAddr)) == SOCKET_ERROR))
    {
        cout<<"Error: bind()"<<endl;
        return 1;
    }

    while (true)
    {
        char buf[BUF_SIZE] = {};

        int strLen = recvfrom(hSocket, buf, BUF_SIZE - 1, 0, NULL, 0);
        buf[strLen] = 0;

        cout << buf << endl;
    }

    closesocket(hSocket);
    WSACleanup();

    return 0;
    
}