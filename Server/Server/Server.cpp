#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

using namespace std;

#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET sendSock;
    SOCKADDR_IN broad_addr;

    // 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    // 소켓 생성
    sendSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP소켓
    if (sendSock == INVALID_SOCKET)
    {
        cout << "Error: socket()";
        return 1;
    }

    broad_addr = {};
    broad_addr.sin_family = AF_INET;
    broad_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Local 브로드캐스트
    broad_addr.sin_port = htons(8888);

    int so_brd = 1;
    setsockopt(sendSock, SOL_SOCKET, SO_BROADCAST, (char*)&so_brd, sizeof(so_brd));

    while (true)
    {
        char buf[BUF_SIZE] = {};
        string inputMSG;
        int msgSize = 0;
        getline(cin, inputMSG);

        if (inputMSG == "Q" || inputMSG == "q")
        {
            break;
        }

        msgSize = inputMSG.size();
        strcpy_s(buf, BUF_SIZE, inputMSG.c_str());

        sendto(sendSock, buf, msgSize, 0, (SOCKADDR*)&broad_addr, sizeof(broad_addr));
    }


    closesocket(sendSock);
    WSACleanup();

    return 0;
}