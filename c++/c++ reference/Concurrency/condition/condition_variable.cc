/*
    condition_variable ����ͬ��ԭ�����������һ���̣߳���ͬʱ��������̣߳�ֱ����һ�߳��޸Ĺ����������������֪ͨ condition_variable ��
    condition_variable ����Ҫ��� std::unique_lock<std::mutex> ʹ����ʵ�֣��ź���Ч��
*/

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;
 
void worker_thread()
{
    // �ȴ�ֱ�� main() ��������
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});
 
    // �ȴ�������ռ������
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
 
    // �������ݻ� main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";
 
    // ֪ͨǰ����ֶ��������Ա���ȴ��̲߳ű����Ѿ�������ϸ�ڼ� notify_one ��
    lk.unlock();
    cv.notify_one();
}
 
int main()
{
    std::thread worker(worker_thread);
 
    data = "Example data";
    // �������ݵ� worker �߳�
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();
 
    // �Ⱥ� worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    std::cout << "Back in main(), data = " << data << '\n';
 
    worker.join();
}