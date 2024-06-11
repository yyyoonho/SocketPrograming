#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
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

    if (bind(hSocket, (sockaddr*)&clntAddr, sizeof(clntAddr))==SOCKET_ERROR)
    {
        cout << "Error: Bind()" << endl;
        return 1;
    }

    // 멀티캐스트 IP가입 절차
    struct ip_mreq joinAddr;
    joinAddr.imr_multiaddr.s_addr = inet_addr("239.255.0.1");   // 멀티캐스트 그룹의 주소정보
    joinAddr.imr_interface.s_addr = htonl(INADDR_ANY);          // 그룹에 가입할 호스트의 주소정보(나)
    if (setsockopt(hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&joinAddr, sizeof(joinAddr)) == SOCKET_ERROR)
    {
        cout<<"Error: setsockopt()"<<endl;
        return 1;
    }

    while (true)
    {
        char buf[30] = {};
        int strLen = recvfrom(hSocket, buf, sizeof(buf) - 1, 0, NULL, 0);
        if (strLen < 0)
            break;

        buf[strLen] = 0;
        cout << buf << endl;
    }

    closesocket(hSocket);
    WSACleanup();

    return 0;
    
}