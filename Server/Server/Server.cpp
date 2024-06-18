#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <process.h>

#include <mutex>
#include <thread>

#include <vector>
#include <string>

using namespace std;

#define BUF_SIZE 100
#define MAX_CLNT 256

int clnt_cnt = 0;
SOCKET clntSocks[MAX_CLNT];

// C++11 부터 mutex를 사용할 수 있는 라이브러리가 생겼다.
// C++ 표준에서 얘기하는 std::mutex는 그 녀석(커널모드 동기화)이 아니고 유저모드 동기화 락이다. (이름만 같은것이다. Mutex는 상호배제라는 의미로 다양하게 쓰인다.)
// Windows 환경에서 내부 구현은 역시나 유저모드 락인 CriticalSection이나 SRWLock 등으로 이루어져 있다.
// 즉, C++ 표준 라이브러리 mutex는 "유저모드 동기화"이다.
mutex m;

void SendMsg(char* msg, int len)
{
    lock_guard<mutex> lock(m);

    for (int i = 0; i < clnt_cnt; i++)
    {
        send(clntSocks[i], msg, len, 0);
    }
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
    lock_guard<mutex> lock(m);

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

    closesocket(hClntSock);

    return 0;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSize = 0;

    vector<thread> threads;

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

        lock_guard<mutex> lock(m);
        clntSocks[clnt_cnt++] = hClntSock;

        threads.push_back(thread(HandleClnt, (void*)&hClntSock));

        cout << "Connected Client IP: " << inet_ntoa(clntAddr.sin_addr) << "\n";
    }

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}