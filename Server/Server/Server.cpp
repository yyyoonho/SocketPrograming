#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

using namespace std;

#define TTL 64
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock;
    SOCKADDR_IN mulAddr;

    // 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Error: WSAStartup()";
        return 1;
    }

    // 소켓 생성
    hServSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP소켓
    if (hServSock == INVALID_SOCKET)
    {
        cout << "Error: socket()";
        return 1;
    }

    // 주소 할당 (멀티캐스트용)
    mulAddr = {};
    mulAddr.sin_family = AF_INET;
    mulAddr.sin_addr.s_addr = inet_addr("239.255.0.1");
    mulAddr.sin_port = htons(8888);
    
    int timeLive = TTL; 
    setsockopt(hServSock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&timeLive, sizeof(timeLive)); // TTL 설정
    
    while (true)
    {
        char buf[BUF_SIZE] = {};
        int inpuMsgSize = 0;

        string inputMSG;
        getline(cin, inputMSG);

        if (inputMSG == "q" || inputMSG == "Q")
        {
            break;
        }

        inpuMsgSize = inputMSG.size();
        strcpy_s(buf, BUF_SIZE, inputMSG.c_str());

        sendto(hServSock, buf, inpuMsgSize, 0, (sockaddr*)&mulAddr, sizeof(mulAddr));

        cout << inpuMsgSize << endl;

        cout << "sendTo 완료" << endl;
    }

    closesocket(hServSock);
    WSACleanup();

    return 0;
}