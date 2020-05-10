#include <iostream> 
#include <stdlib .h> 
#include <omp.h> 
#include <QSemaphore> 
#include <QMutex>
using namespace std;

/*
=====================================用“生产者-消费者”的模式解决积分计算的问题============================================
一个更简单“纯净”的例子可以帮助理解：https://doc.qt.io/qt-5/qtcore-threads-semaphores-example.html


QSemaphore: 提供一个计数的信号量。是互斥量(mutex)的概括。虽然互斥锁只能被锁定一次，但是有可能多次获取信号量。
信号量通常用于保护一定数量的相同资源

QMutex:目的是保护对象，数据结构或代码段，以便一次只能有一个线程可以访问它（这类似于Java synchronized关键字）。


*/
const int BUFFER_SIZE = 10; //定义一个缓冲区
const double LOWER_LIMIT = 0; //定义定积分的下界为0
const double UPPER_LIMIT = 10; //定义定积分的上界为10
const int NUM_COMSUMERS = 2; //消费者的数量为2

/*
构造一个数据结构，这个数据结构用于记录在每一个积分步长（小梯形）的积分上界，下界
*/
typedef struct Slice
{
  double start; //下界
  double end; //下界
  int divisions;
} Slice ;

double func (double x)
{
  return (sin (x));  // 返回一个三角函数的正弦值
}

/*
消费者线程进行积分运算
*/
void integrate (Slice ∗buffer , QSemaphore &buff_used, QSemaphore &buff_avail, QMutex &mutex, int &out, 
                QMutex &result_lock, double &result)
{
  while(1)
  {
    buff_avail.acquire();//尝试获取由信号量保护的资源（这里是指的控制缓冲区存放待处理数据区域的锁）
    mutex.lock();//获取锁后，其他消费者线程会被阻塞。被mutex.lock()和mutex.unlock()之间的代码将会互斥的执行
    
    int tmp_out = out;
    out = (out + 1) % BUFFER_SIZE;
    mutex.unlock(); //执行完临界区的数据操作后，释放锁
   
    //消费者从缓冲区中读取需要处理的数据
    double st = buffer[tmp_out].start;
    double en = buffer[tmp_out].end;
    double div = buffer[tmp_out].divisions;
    
    buff_used.release();//释放由信号量保护的n种资源
    
    if (div == 0)
    {
      break ;
    }
    
    double step = (en − st) / div;
    double x = st;
    double local_res = (func(st) + func (en)) / 2;
    for (int i = 0; i < div; i++)
    {
      x += step;
      local_res += func(x);
    }
    local_res ∗= step ;
    
    //result是一个共享变量，需要一个锁对象对这个变量进行互斥访问
    result_lock.lock ();
    result += local_res;
    result_lock.unlock();
  }
}

int main (int argc , char ∗∗argv)
{
  if (arc != 2)
  {
    cerr<<”Usage: prodcons#jobs”<<endl;
    exit (1);
  }
  
  int batch_size = atoi (argv[1]); //把从外部传入的第二个字符串转换成Integer类型的数据，这里定义的是要把这个区间分成多少批次执行
  Slice ∗buffer = new Slice[BUFF_SIZE];
  int in=0; 
  int out=0;
  QSemaphore buff_avail; //这个信号量控制没有存入待处理数据的缓冲区
  QSemaphore buff_used(BUFF_SIZE); //这个信号两控制已经存入待处理数据，并且消费者线程还没读取的缓冲区区域
  //这两个信号量共同确保了生产者线程在消费者线程之前不会生产出超过BUFF_SIZE的数据量，并且确保消费者线程永远不会读取生产者尚未生成的数据
  
  QMutex mutex;
  QMutex result_lock;
  double integral = 0; //存储积分结果
  
  /*
    parallel sections 定义了一个大的并行区，使得内部的section块可以由多个线程并行执行
  */
  #pragma omp parallel sections default(none) shared(buffer, in, out, buff_avail, buff_used, mutex, integ_lock, integeral, batch_size)
  {
    // producer：这里的producer的主要任务是定义好每个梯形的上边界、下界和。。。
    #pragma omp section
    {
      double div_len = (UPPER_LIMIT − LOWER_LIMIT) /batch_size;
      double st, end = LOWER_LIMIT;
      
      for (int i = 0; i < batch_size; i++)
      {
        st = end;
        end += div_len;
        if ( i == ( batch_size − 1 ) ) //如果当前迭代执行到最后一个批次了，那么把积分上界定义为最后一个sclice的上界
        {
          end = UPPER_LIMIT;
        }
        
        // 进入临界区后，生产者线程往缓冲区中投放待处理数据。当缓冲区满了的时候，生产者线程会阻塞，不会往缓冲区投放待处理数据
        buff_used.acquire();
        buffer[in].start = st; //把每个批次的积分上界和下界存到对应位置的缓冲区中，
                                //比如：对于下界是100，下界是0，要分成10个批次的定积分来说，第一个批次/迭代的上界和下界是0~10，
                                //第二个批次/迭代的上界和下界是11~20，依此类推
        buffer[in].end = end;
        buffer[in].divisions = 1000;//把每个批次切分成1000个小区间的小梯形来计算
        in = (in + 1) % BUFF_SIZE；
        buff_avail.release();  //释放待处理数据的互斥锁，这意味着消费者线程可以开始从缓冲区中读取待处理数据
        
        // add termination sentinels to the buffer
        for (i = 0; i < NUM_CONSUMERS; i++)
        {
          buff_used.acquire();
          buffer[in].divisions = 0;
          in = (in + 1) % BUFF_SIZE;
          buff_avail.release ();
        }
      }
    }
    
    // 1st consumer
    #pragma omp section
    {
      integrate(buffer, buff_used, buff_avail, mutex, out, result_lock, integral);
    }
    
    // 1nd consumer
    #pragma omp section
    {
      integrate(buffer, buff_used, buff_avail, mutex, out, result_lock, integral);
    }
    
    cout << ”Result is ” << integral << endl ;
    delete [] buffer ;
    return (0);
  }
}
