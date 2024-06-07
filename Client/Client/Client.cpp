#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

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

    // 서버와 메시지 주고받기 (3회)
    {
        while (true)
        {
            // 메시지 발신
            char pktMsg[100] = {};

            string inputMessage;
            int inputMessageSize = 0;

            cout << "메시지를 입력하세요: ";
            getline(cin, inputMessage);

            inputMessageSize = inputMessage.size();

            pktMsg[0] = (char)inputMessageSize;

            strcpy_s(&pktMsg[sizeof(int)], sizeof(char) * 100 - sizeof(int), inputMessage.c_str());

            send(hSocket, pktMsg, inputMessageSize + sizeof(int), 0);


            // 메시지 수신
            char recvMsg[30] = {};
            int recvLen = 0;
            int result = 0;
            int msgSize = 0;

            recv(hSocket, recvMsg, sizeof(int), 0);
            msgSize = (int)recvMsg[0];

            while (recvLen < msgSize * sizeof(char))
            {
                int tmpRecvLen = recv(hSocket, &recvMsg[recvLen], sizeof(recvMsg), 0);
                recvLen += tmpRecvLen;
            }

            string newString(recvMsg);
            cout << newString << "\n";
        }
    }
    
    closesocket(hSocket);
    WSACleanup();

    return 0;
    
}