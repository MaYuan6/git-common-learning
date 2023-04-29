/*
    std::call_once ����
    ����std::once_flag��׼ȷִ��һ�οɵ��� (Callable) ���� f����ʹͬʱ�Ӷ���̵߳���
*/

#include <iostream>
#include <mutex>
#include <thread>
#include <exception>
 
std::once_flag flag1, flag2;
 
void simple_do_once()
{
    std::call_once(flag1, [](){ std::cout << "������������һ��\n"; });
}
 
void may_throw_function(bool do_throw)
{
    if (do_throw)
    {
        std::cout << "�׳�:call_once ������\n"; // �����ֲ�ֹһ��
        throw std::exception();
    }
    std::cout << "û���׳�,call_once ����������\n"; // ��֤һ��
}
 
void do_once(bool do_throw)
{
    try
    {
        // std::call_once(flag2, may_throw_function, do_throw);
        //��ʹ�� flag1 �����º���Ҳ������ִ����
        std::call_once(flag1, may_throw_function, do_throw);
    }
    catch(const std::exception & e){
        std::cout << "error :" << e.what() << "\n";
    }
    catch (...) {}
}
 
int main()
{
    std::thread st1(simple_do_once);
    std::thread st2(simple_do_once);
    std::thread st3(simple_do_once);
    std::thread st4(simple_do_once);
    st1.join();
    st2.join();
    st3.join();
    st4.join();
 
    std::thread t1(do_once, true);
    std::thread t2(do_once, true);
    std::thread t3(do_once, false);
    std::thread t4(do_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}