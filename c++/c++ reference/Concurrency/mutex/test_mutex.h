#ifndef __TEST_MUTEX_H__
#define __TEST_MUTEX_H__

/*
    mutex ���������ڱ��������������ܴӶ���߳�ͬʱ���ʵ�ͬ��ԭ�
*/

void test_mutex();



#define TEST_MAIN(f)\
int main(int argc,char** argv)\
{\
    f();\
    return 0;\
}\

#endif