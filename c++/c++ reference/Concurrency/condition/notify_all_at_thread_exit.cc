/*
    notify_all_at_thread_exit �ṩ���ƣ�֪ͨ�����̸߳������߳�����ȫ��ɣ������������� thread_local �������������£�

    ����ǰ��õ��� lk ������Ȩת�Ƶ��ڲ��洢��
    �޸�ִ�л��������ǰ�߳��˳�ʱ����ͬ�����з�ʽ֪ͨ condition_variable cond:
        lk.unlock();
        cond.notify_all();
*/

#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
 
std::mutex m;
std::condition_variable cv;
 
bool ready = false;
int result;  

thread_local int g_thread_data =0 ;

int function_that_uses_thread_locals()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    g_thread_data =1000;
    return 1;
}
 
void thread_func()
{
    std::unique_lock<std::mutex> lk(m);
    // �� thread_local ���ݸ�ֵ�� result
    result = function_that_uses_thread_locals();
    ready = true;
    std::notify_all_at_thread_exit(cv, std::move(lk));
} // 1. ���� thread_local ���� 2. �������⣬ 3. ֪ͨ cv
 
int main()
{
    std::thread t(thread_func);
    t.detach();
 
    // ����������
    // .��
 
    // �ȴ��Ѹ����߳�
    std::unique_lock<std::mutex> lk(m);
    while(!ready) {
        cv.wait(lk);
    }
    return 0;
}