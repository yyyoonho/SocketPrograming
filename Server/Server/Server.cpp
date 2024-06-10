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

    // IO 멀티 플렉싱
    //(멀티플렉싱 구조 서버: 다수의 클라이언트의 입출력 정보를 한꺼번에 묶어서 검사하고 이에 따른 처리를 진행하는 모델)
    TIMEVAL timeout;
    fd_set reads, cpyReads;
    int fdNum = 0;

    FD_ZERO(&reads);
    FD_SET(hServSock, &reads);

    while (true)
    {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
        {
            break;
        }

        if (fdNum == 0)
        {
            cout << "Timeout!" << endl;
        }

        for (int i = 0; i < reads.fd_count; i++)
        {
            if (FD_ISSET(reads.fd_array[i], &cpyReads))
            {
                if (reads.fd_array[i] == hServSock) // connection request!
                {
                    // 연결
                    szClntAddr = sizeof(clntAddr);
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
                    if (hClntSock == INVALID_SOCKET)
                    {
                        cout << "Error: accept()";
                        return 1;
                    }

                    FD_SET(hClntSock, &reads);

                    cout << "Connected client: " << hClntSock << endl;
                }

                else // read message
                {
                    char recvMsg[30] = {};
                    int strLen = 0;

                    strLen = recv(reads.fd_array[i], recvMsg, sizeof(int), 0);

                    if (strLen == 0) // close request
                    {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(reads.fd_array[i]);
                        cout << "Closed Client: " << reads.fd_array[i] << endl;
                    }
                    else // echo!
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

                        send(reads.fd_array[i], recvMsg, msgSize, 0);
                    }
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}