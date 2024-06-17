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
long long sum = 0;

HANDLE sem1;
HANDLE sem2;

unsigned WINAPI Read(void* arg)
{
    for (int i = 0; i < 5; i++)
    {
        WaitForSingleObject(sem2, INFINITE);
        cout << "Input num: ";
        cin >> num;
        ReleaseSemaphore(sem1, 1, NULL);
    }

    return 0;
}

unsigned WINAPI Accu(void* arg)
{
    for (int i = 0; i < 5; i++)
    {
        WaitForSingleObject(sem1, INFINITE);
        sum += num;
        ReleaseSemaphore(sem2, 1, NULL);
    }

    cout << "Result: " << sum << endl;

    return 0;
}

int main()
{
    HANDLE hThread1;
    HANDLE hThread2;

    sem1 = CreateSemaphore(NULL, 0, 1, NULL);
    sem2 = CreateSemaphore(NULL, 1, 1, NULL);

    hThread1 = (HANDLE)_beginthreadex(NULL, 0, Read, NULL, 0, NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, Accu, NULL, 0, NULL);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(sem1);
    CloseHandle(sem2);

    return 0;
}

