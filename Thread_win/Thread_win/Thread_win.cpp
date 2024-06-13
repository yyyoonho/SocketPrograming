#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <process.h>    // C++ 11 표준 Thread 함수
#include <thread>       // WinAPI Thread 함수

#include <string>
#include <vector>

using namespace std;

unsigned WINAPI ThreadFunc(void* arg)
{
    int cnt = *((int*)arg);

    for (int i = 0; i < cnt; i++)
    {
        Sleep(1000);
        cout << "Running Thread" << endl;
    }

    return 0;
}

int main()
{
    HANDLE hThread;
    unsigned threadID;
    int param = 5;

    hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);

    if (hThread == 0)
    {
        cout << "Error: _beginthreadex()" << endl;
        return 1;
    }

    Sleep(3000);

    return 0;
}

