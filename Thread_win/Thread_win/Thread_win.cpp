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

HANDLE hMutex; // Mutex 동기화 (=커널모드 동기화)

unsigned WINAPI ThreadFuncInc(void* arg)
{
    // Mutex는 WaitForSingleObject 함수가 반환될 때, 자동으로 N-S 상태가 되는 "auto-reset 모드' 커널 오브젝트이다.
    // 따라서 WaitForSingleObject 함수가 결과적으로 Mutex를 소유할 때 호출하는 함수가 된다.

    // 호출 후 블로킹 상태 -> Mutex 오브젝트가 다른 쓰레드에게 소유되어서 현재 N-S 상태에 놓여있는 상황
    // 호출 후 반환된 상태 -> Mutex 오브젝트의 소유가 해제되었거나 소유되지 않아서 Signaled 상태에 놓여있는 상황

    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < 50000000; i++)
    {
        num -= 1;
    }
    ReleaseMutex(hMutex);

    return 0;
}

unsigned WINAPI ThreadFuncDes(void* arg)
{
    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < 50000000; i++)
    {
        num += 1;
    }
    ReleaseMutex(hMutex);

    return 0;
}

int main()
{
    HANDLE tHandles[NUM_THREAD];

    // CreateMutex()의 두번째 인자
    // TRUE 전달 시, 생성되는 Mutex 오브젝트는 이 함수를 호출한 쓰레드의 소유가 되면서 Non-signaled 상태가 된다.
    // FALSE 전달 시, 생성되는 Mutex 오브젝트는 소유자가 존재하지 않으며, Signaled 상태가 된다. (= 누군가 소유할 수 있는 상태)
    hMutex = CreateMutex(NULL, FALSE, NULL);

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

