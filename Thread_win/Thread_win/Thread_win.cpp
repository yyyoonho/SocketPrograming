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

unsigned WINAPI ThreadFuncInc(void* arg)
{
    for (int i = 0; i < 50000000; i++)
    {
        num -= 1;
    }

    return 0;
}

unsigned WINAPI ThreadFuncDes(void* arg)
{
    for (int i = 0; i < 50000000; i++)
    {
        num += 1;
    }

    return 0;
}

int main()
{
    HANDLE tHandles[NUM_THREAD];

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

    // 세번째 인자 TRUE = 모든 검사 대상이 다 Signaled상태가 되어야 리턴한다.
    // Signaled = 커널 오브젝트가 종료되는 경우.
    WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);

    cout << "Result: " << num << endl;

    return 0;
}

