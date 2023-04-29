/*
    condition_variable_any ���� std::condition_variable �ķ����������ֻ�� std::unique_lock<std::mutex> �Ϲ����� std::condition_variable �� 
    condition_variable_any �����κ�������������� (BasicLockable) Ҫ������Ϲ�����
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>
#include <mutex>

#define mycout std::cout << std::this_thread::get_id() << ":"

std::mutex  mtx;
std::string data;
std::condition_variable_any cv;

bool ready=false;
bool processed=false;

/*
    ʵ��һ��֧��lock �� unlock����������ʹ�� std::condition_variable_any
*/
template<class T>
class mylock : public std::lock_guard<T>
{
public:
    mylock(T &t) 
            : std::lock_guard<T>(t),_t(t){}
    ~mylock(){}

    void lock(){
        _t.lock();
    }

    void unlock(){
        _t.unlock();
    }

    T &_t;
};

void worker()
{
    {
        mylock<std::mutex>  lock(mtx);
        cv.wait(lock,[](){
            return ready;
        });
        mycout <<"worker start"<< std::endl;

        data = "ready=" + std::to_string(ready);
        processed = true;
    }

    cv.notify_one();
}

int main()
{
    std::thread thread(worker);

    {
        mylock<std::mutex> lock(mtx);
        ready =true;
        data = "main() signal to worker";
    }
    mycout << data<< std::endl;
    cv.notify_one();

    {
        mylock<std::mutex> lock(mtx);
        cv.wait(lock,[](){
            return processed;
        });
    }
    mycout << "callback " << data << std::endl;

    thread.join();
    return 0;
}

