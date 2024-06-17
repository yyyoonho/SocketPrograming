#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <Windows.h>
#include <process.h>    // C++ 11 표준 Thread 함수
#include <thread>       // WinAPI Thread 함수

#include <mutex>        // C++ 11 mutex 라이브러리 지원

#include <string>
#include <vector>

using namespace std;

#define STR_LEN 100
char str[STR_LEN];

// Event 동기화
// Mutex, Semaphore는 Auto-Reset 모드
// Event는 Auto-Reset/ Manual-Reset 모드 중 하나를 선택할 수 있다.
HANDLE hEvent;

unsigned WINAPI NumberOfA(void* arg)
{
    int cnt = 0;

    WaitForSingleObject(hEvent, INFINITE);
    for (int i = 0; str[i] != 0; i++)
    {
        if (str[i] == 'A')
            cnt++;
    }

    cout << "Num of A: " << cnt << endl;

    return 0;
}

unsigned WINAPI NumberOfOthers(void* arg)
{
    int cnt = 0;

    WaitForSingleObject(hEvent, INFINITE);
    for (int i = 0; str[i] != 0; i++)
    {
        if (str[i] != 'A')
            cnt++;
    }

    cout << "Num of Others: " << cnt << endl;

    return 0;
}

int main()
{
    HANDLE hThread1;
    HANDLE hThread2;

    // 두번째 인자 TRUE -> Manual Mode
    // 세번째 인자 FALSE -> Non-signaled 상태로 초기화
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);
    
    cout << "Input String: ";
    cin >> str;

    SetEvent(hEvent); // NS->S

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    ResetEvent(hEvent); // S->NS;

    CloseHandle(hEvent);

    return 0;
}

