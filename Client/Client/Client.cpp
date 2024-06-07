#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

int main()
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    int strLen=0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
    {
        cout << "Error: socket()";
        return 1;
    }

    servAddr = {};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(8888);

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        cout << "Error: connect()";
        return 1;
    }

    vector<char> inputMessage;
    inputMessage.resize(30);

    // 서버에 메시지 보내기
    {
        char opMsg[30] = {};
        int count = 0;
        int num = 0;

        cout << "Operand count:";
        cin >> count;
        opMsg[0] = (char)count;

        for (int i = 0; i < count; i++)
        {

            cout << "Operand " << i << ": ";
            cin >> num;
            opMsg[sizeof(int) * i + 1] = num;
        }

        char op;
        cout << "Operator: ";
        cin >> op;
        opMsg[sizeof(int) * count + 1] = op;

        send(hSocket, opMsg, sizeof(int) * count + 2, 0);
    }

    // 서버에서 메시지 받기
    {
        int result;
        recv(hSocket, (char*) & result, sizeof(int), 0);

        cout << result;
    }
    
    closesocket(hSocket);
    WSACleanup();

    return 0;
}