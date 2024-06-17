#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <process.h>    // C++ 11 표준 Thread 함수
#include <thread>       // WinAPI Thread 함수

#include <mutex>        // C++ 11 mutex 라이브러리 지원

#include <string>
#include <vector>

using namespace std;

#define NUM_THREAD 50
long long num = 0;

mutex m1;

unsigned WINAPI ThreadFuncInc(void* arg)
{
    lock_guard<mutex> lock(m1);
    for (int i = 0; i < 50000000; i++)
    {
        num -= 1;
    }

    return 0;
}

unsigned WINAPI ThreadFuncDes(void* arg)
{
    lock_guard<mutex> lock(m1);
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


    WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);

    cout << "Result: " << num << endl;

    return 0;
}

