#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>

#include <vector>

using namespace std;

int CalFunc(int cnt, int* num, char op)
{
    int firstNum = num[0];

    cout << "firstNum: " << num[0] << endl;

    if (op == '+')
    {
        for (int i = 1; i < cnt; i++)
        {
            cout << "num " << i << ": " << num[i] << endl;

            firstNum += num[i];
        }
    }
    else if (op == '-')
    {
        for (int i = 1; i < cnt; i++)
        {
            firstNum -= num[i];
        }
    }
    else if (op == '*')
    {
        for (int i = 1; i < cnt; i++)
        {
            firstNum *= num[i];
        }
    }

    return firstNum;
}

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

    // 클라이언트에서 온 메시지 처리
    {
        char recvMsg[30] = {};
        int recvLen = 0;
        int result = 0;
        int operandCnt = 0;

        recv(hClntSock, recvMsg, 1, 0);
        operandCnt = recvMsg[0];

        while (recvLen < operandCnt * sizeof(int) + 1)
        {
            int tmpRecvLen = recv(hClntSock, &recvMsg[recvLen], sizeof(recvMsg), 0);
            recvLen += tmpRecvLen;
        }

        result = CalFunc(operandCnt, (int*)recvMsg, recvMsg[recvLen - 1]);
        
        cout << "result: " << result;

        send(hClntSock, (char*)&result, sizeof(result), 0);
    }

    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();

    return 0;
}