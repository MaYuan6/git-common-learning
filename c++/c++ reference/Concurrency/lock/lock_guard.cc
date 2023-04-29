/*
    �� lock_guard �ǻ������װ����Ϊ����������ڼ�ռ�л����ṩ���� RAII �����ơ�

    ���� lock_guard ����ʱ������ͼ���ո������������Ȩ�������뿪���� lock_guard �����������ʱ������ lock_guard ���ͷŻ��⡣

    lock_guard �಻�ɸ���
*/

#include <thread>
#include <mutex>
#include <iostream>
 
int g_i = 0;
std::mutex g_i_mutex;  // ���� g_i
 
void safe_increment()
{
    std::lock_guard<std::mutex> lock(g_i_mutex);
    ++g_i;
 
    std::cout << std::this_thread::get_id() << ": " << g_i << '\n';
 
    // g_i_mutex �����뿪������ʱ�Զ��ͷ�
}
 
int main()
{
    std::cout << "main: " << g_i << '\n';
 
    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
 
    t1.join();
    t2.join();
 
    std::cout << "main: " << g_i << '\n';
}