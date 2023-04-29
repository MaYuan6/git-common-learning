#include <thread>
#include <vector>
#include <iostream>
#include <atomic>

//��ʼ��ԭ����
std::atomic_flag lock= ATOMIC_FLAG_INIT;

int b =0;

void f(int n)
{
    for(int i  = 0; i < 50; ++i){
        //���������������Ի����,��ѡ����
        while(lock.test_and_set(std::memory_order_acquire));
        b += n;
        std::cout << "thread id" << std::this_thread::get_id() 
        <<" n=" << b << std::endl;
        lock.clear(std::memory_order_release);
    }
}

int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(f, 1);
    }
    for (auto& t : v) {
        t.join();
    }
}