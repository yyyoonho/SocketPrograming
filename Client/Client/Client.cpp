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

    // GetHostByName
    struct hostent* host;
    host = gethostbyname("www.google.co.kr");

    if (!host)
    {
        cout << "Error: gethostbyname()";
        return 1;
    }

    cout << "Official name: " << host->h_name << endl;

    for (int i = 0; host->h_aliases[i]; i++)
    {
        cout << "Aliases #" << i + 1 << " " << host->h_aliases[i] << endl;
    }

    string addrType = (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6";
    cout << addrType << endl;

    for (int i = 0; host->h_addr_list[i]; i++)
    {
        cout << "IP #" << i + 1 << " " << inet_ntoa(*(struct in_addr*)host->h_addr_list[i]) << endl;
    }


    WSACleanup();

    return 0;
    
}