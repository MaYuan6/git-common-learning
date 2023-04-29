/*
    ��ģ�� std::promise �ṩ�洢ֵ���쳣����ʩ��֮��ͨ�� std::promise ������������ std::future �����첽��ý����
    ע�� std::promise ֻӦ��ʹ��һ�Ρ�
*/

#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

template<class itr>
void accumulate(itr first,itr last,std::promise<int> accumulae_promise)
{
    int sum = std::accumulate(first,last,0);
    accumulae_promise.set_value(sum);
}

void do_work(std::promise<void(*)(void)> func)
{   

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << std::this_thread::get_id() << ":do_work" << std::endl;
    func.set_value([](){
        std::cout << std::this_thread::get_id() << ":new function" << std::endl;
    });
}


int main()
{
       // ��ʾ�� promise<int> ���̼߳䴫�ݽ����
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::promise<int> accumulate_promise;
    std::future<int> accumulate_future = accumulate_promise.get_future();
    std::thread work_thread(accumulate<std::vector<int>::iterator>, numbers.begin(), numbers.end(),
                            std::move(accumulate_promise));
 
    // future::get() ���ȴ�ֱ���� future ӵ�кϷ������ȡ����
    // ������ get() ǰ���� wait()
    //accumulate_future.wait();  // �ȴ����
    // ���첽���������̻���ͬ������ȥ������ʵ���൱�ڴ��������Ƭ���������߳���ִ�У����ص���������ִ��
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join();  // wait for thread completion
 
    // ��ʾ�� promise<void> ���̼߳��״̬���ź�
    std::promise<void(*)(void)> barrier;
    std::future<void(*)(void)> barrier_future = barrier.get_future();
    std::thread new_work_thread(do_work, std::move(barrier));
    // ���߳������õĺ��������ص����߳���ִ��
    (*barrier_future.get())();
    new_work_thread.join(); 

    //���ݲ������ϵ������̬�������߳����ٽ����Ժ󵽴����̬
    using namespace std::chrono_literals;
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread([&p] {
          std::this_thread::sleep_for(1s);
          p.set_value_at_thread_exit(9);
    }).detach();
    std::cout << "Waiting..." << std::flush;
    f.wait();
    std::cout << "Done!\nResult is: " << f.get() << '\n';
}
