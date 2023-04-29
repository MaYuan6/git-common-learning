#include "test_thread.h"
#include <iostream>
#include <thread>
#include <chrono>

/*******************************************************************/

void f1(int n)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "����ִ���߳�1\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
 
void f2(int& n)
{
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "����ִ���߳�2\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
 
class foo
{
public:
    void bar()
    {
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "����ִ���߳�3\n";
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    int n = 0;
};
 
class baz
{
public:
    void operator()()
    {
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "����ִ���߳�4\n";
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    int n = 0;
};
 
void test_create_thread()
{
    int n = 0;
    foo f;
    baz b;
    std::thread t1; // t1 �����߳�
    std::thread t2(f1, n + 1); // ��ֵ����
    std::thread t3(f2, std::ref(n)); // �����ô���
    std::thread t4(std::move(t3)); // t4 �������� f2()��t3 �������߳�
    std::thread t5(&foo::bar, &f); // t5 �ڶ��� f ������ foo::bar()
    std::thread t6(b); // t6 �ڶ��� b �ĸ��������� baz::operator()
    t2.join();
    t4.join();
    t5.join();
    t6.join();
    std::cout << "n ������ֵ�� " << n << '\n';
    std::cout << "f.n (foo::n) ������ֵ�� " << f.n << '\n';
    std::cout << "b.n (baz::n) ������ֵ�� " << b.n << '\n';
}
/*******************************************************************/

void test_id()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

//��ʶ�� *this �������̵߳� std::thread::id ����ֵ�����޹������̣߳��򷵻�Ĭ�Ϲ���� std::thread::id 
void test_get_threadId()
{
    std::thread t1(test_id);
    std::thread::id t1_id = t1.get_id();
 
    std::thread t2(test_id);
    std::thread::id t2_id = t2.get_id();
 
    //std::hex ʹcout ʹ��ʮ���������
    std::cout << "t1's id: " << std::hex << t1_id << '\n';
    std::cout << "t2's id: " << t2_id << '\n';
 
    t1.join();
    t2.join();
}

// TEST_MAIN(test_get_threadId)

/*******************************************************************/

void test_join()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
 
//��� std::thread �����Ƿ��ʶ��Ծ��ִ���̡߳�������ԣ��� get_id() != std::thread::id() �򷵻� true
void test_thread_joinable(){

    std::thread t; //�ⴴ���Ĳ���һ���߳�
    // std::boolalpha ��Ϊ����cout ���� true �� false�����
    std::cout << "before starting, joinable: " << std::boolalpha << t.joinable()
              << '\n';
 
    t = std::thread(test_join); //��������һ���̣߳����Ա�join
    std::cout << "after starting, joinable: " << t.joinable() 
              << '\n';
 
    t.join();
    //�߳��Ѿ�������t������һ����Ծ���߳�
    std::cout << "after joining, joinable: " << t.joinable() 
              << '\n';
}   

// TEST_MAIN(test_thread_join)

/*******************************************************************/


#ifdef __linux__
#include <pthread.h>

std::mutex iomutex;
void test_native_handle(int num)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
 
    sched_param sch;
    int policy; 
    pthread_getschedparam(pthread_self(), &policy, &sch);
    std::lock_guard<std::mutex> lk(iomutex);
    std::cout << "Thread " << num << " is executing at priority "
              << sch.sched_priority << '\n';
}
 
void test_thread_native_handle()
{
    std::thread t1(test_native_handle, 1), t2(test_native_handle, 2);
 
    sched_param sch;
    int policy; 
    //��ȡ��ǰ�̵߳ĵ��Ȳ���
    pthread_getschedparam(t1.native_handle(), &policy, &sch);
    sch.sched_priority = 20;
    //���õ�ǰ�̵߳ĵ��Ȳ���
    if (pthread_setschedparam(t1.native_handle(), SCHED_FIFO, &sch)) {
        std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
    }
 
    t1.join(); 
    t2.join();
}
#else
#include <processthreadsapi.h>
#include <tlhelp32.h>

int GetProcessThreadList(DWORD th32ProcessID) //���̵�ID
{
    HANDLE hThreadSnap;
    THREADENTRY32 th32;
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, th32ProcessID);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
    {
    return 1;
    }
    th32.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hThreadSnap, &th32))
    {
    CloseHandle(hThreadSnap);
    return 1;
    }
    do
    {
    if (th32.th32OwnerProcessID == th32ProcessID)
    {
    printf("ThreadID: %ld\n", th32.th32ThreadID); //��ʾ�ҵ����̵߳�ID
    }
    }while(Thread32Next(hThreadSnap, &th32));
    CloseHandle(hThreadSnap);
    return 0;
}


void test_native_handle(int num)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "num=" << num <<",test_native_handle thread id=" << GetCurrentThreadId() << std::endl;

}
void test_native_handle_2(int num)
{

    while(true){
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "num=" << num <<",test_native_handle thread id=" << GetCurrentThreadId() << std::endl;
    std::cout << "num=" << num <<",test_native_handle thread id=" << GetCurrentThread() << std::endl;
    }
}



void test_thread_native_handle()
{
    std::cout << "test_thread_native_handle" << std::endl;
    std::thread t1(test_native_handle, 1), t2(test_native_handle_2, 2);


    std::cout << "��ǰ���еľ��...."<< std::endl;
    GetProcessThreadList(GetCurrentProcessId());

    t1.join(); 
    //��������̷߳���� TerminateThread ��ʧȥЧ��
    // t2.detach();
    std::cout << "�������ⰴ����ֹ�߳�2:"<< std::endl;

    getchar();
    //ʹ�þ��ǿ����ֹtest_native_handle_2
    std::cout << "������ֹ...."<< (HANDLE)t2.native_handle() << std::endl;
    TerminateThread((HANDLE)t2.native_handle(), (unsigned long)0x00);
    // ExitThread(*(DWORD*)t2.native_handle());
    std::cout << "����...."<< std::endl;
    std::cout << "��ǰ���еľ��...."<< std::endl;
    t2.join();

    //���˳��̣߳����ǲ��������ٵ����
    GetProcessThreadList(GetCurrentProcessId());
    while(true);

    
}
// TEST_MAIN(test_thread_native_handle)


/*******************************************************************/

void test_thread_hardware_concurrency()
{
    uint32_t n = std::thread::hardware_concurrency();

    std::cout << n << "  concurrency threads are supported.\n";
}


TEST_MAIN(test_thread_hardware_concurrency)


/*******************************************************************/
#endif
