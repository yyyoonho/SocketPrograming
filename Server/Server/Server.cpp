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
    int ClntAddrSize = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &ClntAddrSize);

    TIMEVAL timeout;
    fd_set reads, cpyReads;
    fd_set excepts, cpyExcepts;
    int fdNum = 0;

    FD_ZERO(&reads);
    FD_SET(hClntSock, &reads);  // Urgent Pointer의 앞 부분에 위치한 1바이틑 제외한 나머지는 일반 적인 입력함수의 호출을 통해서 읽힌다.
                                // 때문에, 일반 수신 fd_set에도 넣어주어야 한다.
    FD_ZERO(&excepts);
    FD_SET(hClntSock, &excepts);

    while (true)
    {
        cpyReads = reads;
        cpyExcepts = excepts;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        int result = select(0, &cpyReads, 0, &cpyExcepts, &timeout);
        
        if (result > 0)
        {
            char buf[30] = {};
            int strLen = 0;

            if (FD_ISSET(hClntSock, &cpyExcepts))
            {
                strLen = recv(hClntSock, buf, sizeof(buf) - 1, MSG_OOB);
                buf[strLen] = 0;
                cout << "(MSG_OOB)" << buf << endl;
            }

            
            if (FD_ISSET(hClntSock, &cpyReads))
            {
                strLen = recv(hClntSock, buf, sizeof(buf) - 1, 0);
                if (strLen == 0)
                {
                    closesocket(hClntSock);
                    break;
                }
                else
                {
                    buf[strLen] = 0;
                    cout << "(NoOption)" << buf << endl;
                }
            }
            
        }
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}