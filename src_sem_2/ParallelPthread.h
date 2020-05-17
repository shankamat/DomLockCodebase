#ifndef ParallelPthread_H
#define ParallelPthread_H
#include <pthread.h>
using namespace std;

extern double degOfConc[32];

class threadData
{
	public:
		Tree * TreePtr;
		scheduler *sc;
		int nodeId;
		int threadID;

};
threadData *TD = new threadData();
class thread
{
	public:
		pthread_mutex_t rootLock;
		int TCS;//checks whether thread created successfully

	
		thread();
		void coarseLock();
		void coarseUnLock();
		void CreateThread(int,Tree *, scheduler *);
		static void* ParallelTask(void *TD)
		{
			threadData *data = (threadData *)TD;
			Tree *p = data->TreePtr;
			scheduler *sc = data->sc;
			int threadID = data->threadID;
			double avgDegOfC=0;
			double tmp=0;
			double divisor = 100;			
			for(int i=0;i<100;i++) //1000
			{	
				tmp=0;
				p->DummyTask(threadID, tmp);
		//		cout<<"threadID "<<threadID<<endl;				
				avgDegOfC+=tmp;
				if(tmp==0)
					divisor--;
				
			}
//			cout<<"TID "<<threadID<<" divisor "<<divisor<<endl;
			degOfConc[threadID] = divisor<=0 ? 0: avgDegOfC/divisor;
			//cout<<"Check : "<<degOfConc[threadID]<<endl;
		}

};



thread::thread()
{
	rootLock= PTHREAD_MUTEX_INITIALIZER;
}

void thread::CreateThread(int NUM_THREADS,Tree *ptr, scheduler *sc)
{
	threadData *TD[NUM_THREADS];

	pthread_t threadArray[NUM_THREADS];

	for(int i=0; i < NUM_THREADS; i++ )
	{
		rand();
		TD[i] = new threadData();
		TD[i]->TreePtr = ptr;
		TD[i]->sc = sc;
		TD[i]->nodeId = 0;
		TD[i]->threadID = i;
		TCS = pthread_create(&threadArray[i], NULL, &thread::ParallelTask,(void *)TD[i]);
		if (TCS!=0)
		{
			cout << "Error:unable to create thread," << TCS << endl;
			exit(-1);
		}
	}

	//cout<<"WAIT \n";
	for(int i=0;i<NUM_THREADS;i++)
		pthread_join( threadArray[i], NULL );

	sc->TestInit(); 
}



void thread::coarseLock()
{
	pthread_mutex_lock(&rootLock);
}


void thread::coarseUnLock()
{
	pthread_mutex_unlock(&rootLock);
}

#endif







