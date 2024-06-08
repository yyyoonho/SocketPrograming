#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>

using namespace std;

int main()
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    int szClntAddr;

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

    // 주소 할당
    servAddr = {};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(8888);

    if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        cout << "Error: bind()";
        return 1;
    }

    // 연결될 준비
    if (listen(hServSock, 5) == SOCKET_ERROR)
    {
        cout << "Error: listen()";
        return 1;
    }

    // 연결
    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
    if (hClntSock == INVALID_SOCKET)
    {
        cout << "Error: accept()";
        return 1;
    }

    // SO_REUSEADDR -> Time-wait 수정
    int option = 0;
    option = TRUE;
    int state = setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));

    if (state == SOCKET_ERROR)
    {
        cout << "Error: setsockopt()" << endl;
        return 1;
    }
    

    // 클라이언트에서 온 메시지 처리
    {
        char recvMsg[30] = {};
        int strLen = 0;

        while ((strLen = recv(hClntSock, recvMsg, sizeof(int), 0)) != 0)
        {
            int recvLen = 0;
            int msgSize = 0;

            msgSize = (int)recvMsg[0];

            while (recvLen < msgSize * sizeof(char))
            {
                int tmpRecvLen = recv(hClntSock, &recvMsg[recvLen], sizeof(recvMsg), 0);
                recvLen += tmpRecvLen;
            }

            string newString(recvMsg);
            cout << newString << "\n";
        }

        cout << strLen << endl;
    }

    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();

    return 0;
}