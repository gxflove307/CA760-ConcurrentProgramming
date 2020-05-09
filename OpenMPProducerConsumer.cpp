#include <iostream> 
#include <stdlib .h> 
#include <omp.h> 
#include <QSemaphore> 
#include <QMutex>
using namespace std;
const int BUFFER SIZE = 10;
const double LOWER LIMIT = 0;
const double UPPER LIMIT = 10;
const int NUM_COMSUMERS = 2;

typedef struct Slice
{
  double start;
  double end;
  int divisions;
} Slice ;

double func (double x)
{
  return (sin (x)); 
}

void integrate (Slice ∗buffer , QSemaphore &buff slots, QSemaphore &avail, QMutex &l, int &out, 
                QMutex &res lock, double &res)
{
  while(1)
  {
    avail.acquire();
    l.lock();
    int tmp out = out;
    out = (out + 1) % BUFFER_SIZE;
    l.unlock();
   
    double st = buffer[tmp out].start;
    double en = buffer[tmp out].end;
    double div = buffer[tmp out].divisions;
    
    buff slots.release();
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
      local res += func (x);
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
  
  int J = atoi (argv[1]);
  Slice ∗buffer = new Slice[BUFF SIZE];
  int in=0; out=0;
  QSemaphore avail , buff_slots (BUFF SIZE);
  QMutex l , integ_lock ;
  double integral = 0;
  
  #pragma omp parallel sections default(none) shared(buffer, in, out, avail, buff_slots, l, integ_lock, integeral, J)
  {
    // producer
    #pragma omp section
    {
      double div_len = (UPPER LIMIT − LOWER LIMIT /J;
      double st , end = LOWER LIMIT;
      
      for (int i = 0; i < J; i++)
      {
        st = end;
        end += div en;
        if ( i == ( J − 1 ) )
        {
          end = UPPER LIMIT;
        }
        
        buff_slots.acquire ();
        buffer[in].start = st;
        buffer[in].end = end;
        buffer [ in ]. divisions = 1000;
        in = (in + 1) % BUFF SIZE；
        avail.release ();
        
        
        // add termination sentinels to the buffer
        for (i = 0; i < NUMCONSUMERS; i++)
        {
          buff_slots.acquire ();
          buffer[in].divisions = 0; 
          in = (in + 1) % BUFF SIZE; 
          avail . release ();
        }
      }
    }
    
    // 1st consumer
    #pragma omp section
    {
      integrate(buffer , buff slots , avail , l , out , integ lock , integral);
    }
    
    // 1nd consumer
    #pragma omp section
    {
      integrate(buffer , buff slots , avail , l , out , integ lock , integral);
    }
    
    cout << ”Result is ” << integral << endl ;
    delete [] buffer ;
    return (0);
  }
}
