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
    DWORD wr;
    unsigned threadID;
    int param = 5;

    hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);
    if (hThread == 0)
    {
        cout << "Error: _beginthreadex()" << endl;
        return 1;
    }

    // WaitForSingleObject
    // 하나의 커널 오브젝트에 대해서 signaled 상태인지를 확인하기 위해서 호출하는 함수
    // OS: "프로세스나 쓰레드가 종료되면 해당 커널 오브젝트를 signaled 상태로 변경해 놓겠다!"
    if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED)
    {
        cout << "Error: WaitForSingleObject()" << endl;
        return 1;
    }

    string tmp = (wr == WAIT_OBJECT_0) ? "Signaled" : "Time-out";
    cout << "Aait result: " << tmp << endl;

    return 0;
}

