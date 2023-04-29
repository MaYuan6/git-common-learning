/*
    �� unique_lock ��ͨ�û����װ���������ӳ���������������ʱ�޳��ԡ��ݹ�����������Ȩת�ƺ�����������һͬʹ�á�

    �� unique_lock ���ƶ��������ɸ��ơ�����������ƶ����� (MoveConstructible) �Ϳ��ƶ���ֵ (MoveAssignable) ��������ɸ��ƹ��� (CopyConstructible) ��ɸ��Ƹ�ֵ (CopyAssignable) ��

    �� unique_lock ������������� (BasicLockable) Ҫ���� Mutex ��������� (Lockable) Ҫ���� unique_lock ����������� (Lockable) Ҫ�����磺������ std::lock �� ���� Mutex ����ɶ�ʱ���� (TimedLockable) Ҫ���� unique_lock ������ɶ�ʱ���� (TimedLockable) Ҫ��
*/

#include <mutex>
#include <thread>
#include <chrono>
 
struct Box {
    explicit Box(int num) : num_things{num} {}
 
    int num_things;
    std::mutex m;
};
 
void transfer(Box &from, Box &to, int num)
{

    /*
        std::defer_lock : ����û��������Ȩ ����֤lock�ܱ�֤������unique_lockֻ��֤����
        std::adopt_lock ��������÷��߳���ӵ�л��������Ȩ�� lock �Ѿ���ǰ���ù�����unique_lockֻ��֤����
    */
#if 0
    // ��δʵ��ȡ��
    std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
 
    // ������ unique_lock ��������
    std::lock(lock1, lock2);   
#else
    std::lock(from.m,to.m);
    std::unique_lock<std::mutex> lock1(from.m,std::adopt_lock);
    std::unique_lock<std::mutex> lock2(to.m,std::adopt_lock);
#endif

    from.num_things -= num;
    to.num_things += num;
 
    // 'from.m' �� 'to.m' ��������� 'unique_lock' ��������
}
 
int main()
{
    Box acc1(100);
    Box acc2(50);
 
    std::thread t1(transfer, std::ref(acc1), std::ref(acc2), 10);
    std::thread t2(transfer, std::ref(acc2), std::ref(acc1), 5);
 
    t1.join();
    t2.join();
}