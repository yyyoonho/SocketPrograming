#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <process.h>    // C++ 11 표준 Thread 함수
#include <thread>       // WinAPI Thread 함수

#include <string>
#include <vector>

using namespace std;

#define NUM_THREAD 50
long long num = 0;

CRITICAL_SECTION cs; // CRITICAL SECTION 기반의 동기화 (=유저모드 동기화)

unsigned WINAPI ThreadFuncInc(void* arg)
{
    EnterCriticalSection(&cs); // Key 획득
    for (int i = 0; i < 50000000; i++)
    {
        num -= 1;
    }
    LeaveCriticalSection(&cs); // Key 반납

    return 0;
}

unsigned WINAPI ThreadFuncDes(void* arg)
{
    EnterCriticalSection(&cs); // Key 획득
    for (int i = 0; i < 50000000; i++)
    {
        num += 1;
    }
    LeaveCriticalSection(&cs); // Key 반납

    return 0;
}

int main()
{
    HANDLE tHandles[NUM_THREAD];

    InitializeCriticalSection(&cs); // Critical Section 초기화

    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (i % 2)
        {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncInc, NULL, 0, NULL);
        }
        else
        {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncDes, NULL, 0, NULL);
        }
    }


    WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);

    DeleteCriticalSection(&cs);

    cout << "Result: " << num << endl;

    return 0;
}

