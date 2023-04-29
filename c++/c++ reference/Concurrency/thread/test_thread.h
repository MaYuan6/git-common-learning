#ifndef __TEST_THREAD_H__
#define __TEST_THREAD_H__


//�̹߳��캯��ʹ��
void test_create_thread();

//��ȡ�߳�id
void test_get_threadId();

//�߳�join
void test_thread_joinable();

//��ȡ�ײ㶨����߳̾��
void test_thread_native_handle();


//windows�»�ȡ���о��
#ifndef __linux__
#include <Windows.h>
int GetProcessThreadList(DWORD th32ProcessID);
#endif


//�����߳�ʵ��֧�ֵĲ����߳�����Ӧ��ֻ�Ѹ�ֵ������ʾ
void test_thread_hardware_concurrency();


#define TEST_MAIN(f)\
int main(int argc,char** argv)\
{\
    f();\
    return 0;\
}\


#endif