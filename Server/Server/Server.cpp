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


    // IO멀티플렉싱 + MSG_OOB
    fd_set reads, cpyReads;
    fd_set excepts, cpyExcepts;
    TIMEVAL timeout;

    FD_ZERO(&reads);
    FD_SET(hServSock, &reads);

    FD_ZERO(&excepts);


    while (true)
    {
        cpyReads = reads;
        cpyExcepts = excepts;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        int result = select(0, &cpyReads, 0, &cpyExcepts, &timeout);

        if (result == SOCKET_ERROR)
        {
            cout << "SOCKET ERROR!" << endl;
            break;
        }
        if (result == 0)
        {
            cout << "Time out!" << endl;
        }

        if (result > 0)
        {
            // 예외(MSG_OOB)
            
            for (int i = 0; i < excepts.fd_count; i++)
            {
                if (FD_ISSET(excepts.fd_array[i], &cpyExcepts))
                {
                    // 메시지 읽기
                    char buf[30] = {};
                    int strLen = 0;
                    strLen = recv(hClntSock, buf, sizeof(buf) - 1, MSG_OOB);

                    buf[strLen] = 0;
                    cout << "(MSG_OOB) " << buf << endl;
                }
            }
            

            // 일반
            for (int i = 0; i < reads.fd_count; i++)
            {
                if (FD_ISSET(reads.fd_array[i], &cpyReads))
                {
                    // 소켓이 서버소켓인 경우(=connect요청)
                    if (reads.fd_array[i] == hServSock)
                    {
                        int addrSize = sizeof(clntAddr);
                        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &addrSize); // (다중접속을 할 경우 클라이언트소켓을 vector로 관리해보자)
                                                                                        // (테스트 편의를 위해 하나의 클라이언트만 접속하도록 하였음)
                        FD_SET(hClntSock, &reads);
                        FD_SET(hClntSock, &excepts);

                        cout << "Connected Client: " << hClntSock << endl;
                    }
                    // 메시지 읽기
                    else
                    {
                        char buf[30] = {};
                        int strLen = 0;
                        strLen = recv(reads.fd_array[i], buf, sizeof(buf) - 1, 0);

                        // 종료요청
                        if (strLen == 0)
                        {
                            FD_CLR(reads.fd_array[i], &reads);
                            FD_CLR(reads.fd_array[i], &excepts);
                            closesocket(reads.fd_array[i]);

                            cout << "Close Client: " << reads.fd_array[i] << endl;

                            break;
                        }
                        else
                        {
                            buf[strLen] = 0;
                            cout << "(NO Option)" << buf << endl;
                        }
                    }
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}