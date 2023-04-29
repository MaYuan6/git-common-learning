/*
    std::memory_order ����ָ���ڴ���ʣ�
    ���������Χ��ԭ�Ӳ�������

    �����¼��ַ�ʽ��
    memory_order_relaxed��
        ���ɲ�����û��ͬ����˳����Լ�����Դ˲���Ҫ��ԭ���ԣ����·�����˳�򣩡�
    memory_order_consume��
        �д��ڴ�˳��ļ��ز���������Ӱ����ڴ�λ�ý������Ѳ�����
        ��ǰ�߳��������ڵ�ǰ���صĸ�ֵ�Ķ���д���ܱ����ŵ��˼���ǰ��
        �����ͷ�ͬһԭ�ӱ������̵߳Ķ���������������д�룬Ϊ��ǰ�߳����ɼ���
        �ڴ����ƽ̨�ϣ���ֻӰ�쵽�������Ż������·��ͷ�����˳�򣩡�
    memory_order_acquire��
        �д��ڴ�˳��ļ��ز���������Ӱ����ڴ�λ�ý��л�ò�����
        ��ǰ�߳��ж���д���ܱ����ŵ��˼���ǰ�������ͷ�ͬһԭ�ӱ������̵߳�����д�룬
        ��Ϊ��ǰ�߳����������·��ͷŻ��˳��
    memory_order_release:
    	�д��ڴ�˳��Ĵ洢���������ͷŲ�����
        ��ǰ�߳��еĶ���д���ܱ����ŵ��˴洢�󡣵�ǰ�̵߳�����д�룬
        �ɼ��ڻ�ø�ͬһԭ�ӱ����������̣߳����·��ͷŻ��˳�򣩣�
        ���ҶԸ�ԭ�ӱ����Ĵ�����д���ö�����������ͬһԭ�Ӷ�����߳̿ɼ������·��ͷ�����˳��
    memory_order_acq_rel:
    	�����ڴ�˳��Ķ��޸�д�������ǻ�ò��������ͷŲ�����
        ��ǰ�̵߳Ķ���д�ڴ治�ܱ����ŵ��˴洢ǰ���
        �����ͷ�ͬһԭ�ӱ������̵߳�д��ɼ����޸�֮ǰ�������޸Ŀɼ����������ͬһԭ�ӱ������̡߳�
    memory_order_seq_cst:	
        �д��ڴ�˳��ļ��ز������л�ò������洢���������ͷŲ�����
        �����޸�д�������л�ò������ͷŲ������ټ��ϴ���һ������ȫ��
        ���������߳���ͬһ˳��۲⵽�����޸ģ����·�����һ��˳�򣩡�            
*/

/**************************************************/

#if 0
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>
//ʹ�ÿ��ɵķ�ʽʵ��ԭ�Ӳ�������Ҫ��ִ�е�˳���ͬ����ֻ��֤���ݵ�ԭ�����Ӻ��޸�˳���һ����
// std::atomic<int> cnt = {0};
std::atomic<int> cnt = ATOMIC_VAR_INIT(0);

void f(){
    for(int i =0; i <1000; ++i){
        //ֻ��֤ԭ���Ժ��޸�˳���һ����,�����ڼ�������������Ҫ���߳�ִ�е�˳���ͬ��
        cnt.fetch_add(1,std::memory_order_relaxed);
    }
}

int main(int argc,char ** argv)
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(f);
    }
    for (auto& t : v) {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n';    

    return 0;
}
#endif

/**************************************************/
/*
    ������������ģ��
    memory_order_release��memory_order_acquireһ��ɶ�ʹ��

    һ��ԭ�Ӽ�����ɣ��ܹ���֤�߳�B�ܹ��۲쵽�߳�Aд���ڴ����������
*/
#if 0
#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>
#include <string>

std::atomic<std::string *> ptr;
int data;

void producer()
{
    std::string* p = new std::string("hello");
    data = 42;
    //��store�洢���ݺ�ʹ�� memory_order_release���߳��е����в�����consumer�ɼ�
    ptr.store(p,std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    // ��֤ʱ���� producer ��������󣬻��ptr�е�����
    while(!(p2 = ptr.load(std::memory_order_acquire)));

    //��Ȼֻ��ȡ��ptr�е����ݣ����� producer �еĹ��̶��� consumer ��˵���ǿɼ���
    assert(*p2 == "hello");
    assert(data == 42);
    std::cout << *p2 << data;
}

int main(int argc,char ** argv)
{  
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
#endif
/**************************************************/


/**************************************************/
/*
    ��ʾ�����̼߳䴫���Ե��ͷŻ��˳��
*/
#if 0
#include <thread>
#include <atomic>
#include <cassert>
#include <vector>
#include <iostream>

std::vector<int> data;
std::atomic<int> flag = ATOMIC_VAR_INIT(0);

// �̵߳�ִ��˳�� 1 -> 2 ->3
void thread_1()
{
    data.push_back(42);
    flag.store(1,std::memory_order_release);
    std::cout << "thread_1" << std::endl;
}

void thread_2()
{
    int expected =1;
    //memory_order_acq_rel �ǻ�ȡ��Ҳ���ͷ�
    //����expected 2���������н����������������м���
    //��������״̬����ֻ�е��޸ĳɹ���Ż᷵��ture����֤״̬�ܹ������л�
    while(!flag.compare_exchange_strong(expected,2,std::memory_order_acq_rel));
    std::cout << flag.load(std::memory_order_acquire) << std::endl;
    std::cout << "thread_2" << std::endl;
   
}

void thread_3()
{
    while (flag.load(std::memory_order_acquire) < 2);
    assert(data.at(0) == 42); // ��������
    std::cout << "thread_3" << std::endl;
}

int main()
{
    std::thread a(thread_1);
    std::thread b(thread_2);
    std::thread c(thread_3);
    a.join(); 
    b.join(); 
    c.join();

    return 0;
}
#endif
/**************************************************/


/**************************************************/
/*
    ʹ�� memory_order_seq_cst ��֤�̰߳�����ȫ˳����ִ�У�һ��������� z Ϊ 0������
*/
#if 1
#include <thread>
#include <atomic>
#include <cassert>
#include <iostream>
 
std::atomic<bool> x = {false};
std::atomic<bool> y = {false};
std::atomic<int> z = {0};
 
void write_x()
{
    x.store(true, std::memory_order_seq_cst);
    std::cout << "write_x" << std::endl;
}
 
void write_y()
{
    y.store(true, std::memory_order_seq_cst);
    std::cout << "write_y" << std::endl;
}
 
void read_x_then_y()
{
    //�̱߳��밴�� x -> y ���й۲죬������ǵĻ����ܻ����� z.load() assert
    while (!x.load(std::memory_order_seq_cst));
    if (y.load(std::memory_order_seq_cst)) {
        ++z;
        std::cout << "read_x_then_y" << std::endl;
    }
}
 
void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst));
    if (x.load(std::memory_order_seq_cst)) {
        ++z;
        std::cout << "read_y_then_x" << std::endl;
    }
}
 
int main()
{
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join(); b.join(); c.join(); d.join();
    assert(z.load() != 0);  // ��������
    std::cout << z.load() << std::endl;
}
#endif
/**************************************************/
