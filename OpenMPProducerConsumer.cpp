#include <iostream> 
#include <stdlib .h> 
#include <omp.h> 
#include <QSemaphore> 
#include <QMutex>
using namespace std;

/*
=====================================用“生产者-消费者”的模式解决积分计算的问题============================================
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
QSemaphore: 提供一个计数的信号量。是互斥量(mutex)的概括。虽然互斥锁只能被锁定一次，但是有可能多次获取信号量。
信号量通常用于保护一定数量的相同资源

QMutex:目的是保护对象，数据结构或代码段，以便一次只能有一个线程可以访问它（这类似于Java sync关键字）。
通常最好将互斥锁与QMutexLocker一起使用，因为这样可以轻松确保锁定和解锁一致地执行。

*/
void integrate (Slice ∗buffer , QSemaphore &buff_slots, QSemaphore &avail, QMutex &mutex, int &out, 
                QMutex &res lock, double &res)
{
  while(1)
  {
    avail.acquire();//尝试获取由信号量保护的n种资源（这里只有1种资源）
    mutex.lock();//获取锁后，锁定并进入临界区
    int tmp_out = out;
    out = (out + 1) % BUFFER_SIZE;
    mutex.unlock(); //执行完临界区的数据操作后，释放锁
   
    double st = buffer[tmp_out].start;
    double en = buffer[tmp_out].end;
    double div = buffer[tmp_out].divisions;
    
    buff_slots.release();//释放由信号量保护的n种资源
    
    if (div == 0)
    {
      break ;
    }
    
    double step = (en − st) / div;
    double x = st;
    double local_res = (func (st) + func (en)) / 2;
    for (int i = 0; i < div; i++)
    {
      x += step;
      local_res += func (x);
    }
    local_res ∗= step ;
    
    res_lock.lock ();
    res += local_res;
    res_lock.unlock();
  }
}

int main (int argc , char ∗∗argv)
{
  if (arc != 2)
  {
    cerr<<”Usage: prodcons#jobs”<<endl;
    exit (1);
  }
  
  int J = atoi (argv[1]); //把从外部传入的第二个字符串转换成Integer类型的数据，这里定义的是要把这个区间分成多少个小梯形
  Slice ∗buffer = new Slice[BUFF_SIZE];
  int in=0; out=0;
  QSemaphore avail , buff_slots(BUFF_SIZE);
  QMutex mutex , integ_lock ;
  double integral = 0;
  
  /*
    parallel sections 定义了一个大的并行区，使得内部的section块可以由多个线程并行执行
  */
  #pragma omp parallel sections default(none) shared(buffer, in, out, avail, buff_slots, mutex, integ_lock, integeral, J)
  {
    // producer：这里的producer的主要任务是定义好每个小梯形的上边界、下界和。。。
    #pragma omp section
    {
      double div_len = (UPPER_LIMIT − LOWER_LIMIT /J;
      double st, end = LOWER_LIMIT;
      
      for (int i = 0; i < J; i++)
      {
        st = end;
        end += div_len;
        if ( i == ( J − 1 ) ) //如果梯形的总个数恰好是当前迭代，说明这个上界和下界的积分只相差一个小梯形的面积。这时候，直接把积分上界值设置为本次迭代的积分上界的
        {
          end = UPPER_LIMIT;
        }
        
        // 进入临界区
        buff_slots.acquire ();
        buffer[in].start = st;
        buffer[in].end = end;
        buffer [in]. divisions = 1000;
        in = (in + 1) % BUFF_SIZE；
        avail.release ();
        
        // add termination sentinels to the buffer
        for (i = 0; i < NUM_CONSUMERS; i++)
        {
          buff_slots.acquire ();
          buffer[in].divisions = 0; 
          in = (in + 1) % BUFF_SIZE; 
          avail.release ();
        }
      }
    }
    
    // 1st consumer
    #pragma omp section
    {
      integrate(buffer, buff_slots, avail, mutex, out, integ_lock, integral);
    }
    
    // 1nd consumer
    #pragma omp section
    {
      integrate(buffer, buff_slots, avail, mutex, out, integ_lock, integral);
    }
    
    cout << ”Result is ” << integral << endl ;
    delete [] buffer ;
    return (0);
  }
}
