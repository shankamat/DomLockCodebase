#ifndef SB7_INTERVAL_H_
#define SB7_INTERVAL_H_

#include <vector>
#include <stdlib.h>
using namespace std;
#define SIZE 20

pthread_mutex_t mutex;
long Seq;
class interval{

	public: double pre, post; 
		int mode;
		long MySeq;
		interval( double a, double b, int m){
			pre = a; post = b; mode = m;
		}
};

//**************************************************************************************************************
class IntervalCheck{
	public:
		interval *Array[SIZE];
		int VectorSize[SIZE];
		//Lock variable, one per lock-pool location
		pthread_rwlock_t ArrayLock[SIZE];

		//Lock pool for Multi-DomLock, .i.e., multiple lock node per thread
		vector<interval**>MultiLockPool;

		//Sequence number per thread for fairness and less contention
		int MySeq[SIZE];

		//*****************************************************************************
		//Constructor for initialization of class variables
		//*****************************************************************************
		IntervalCheck()
		{
			Seq = 0;
			for(int i = 0;i<SIZE; i++)
			{

				Array[i] = NULL;
				pthread_rwlock_t ArrayLock[i];
			}
			MultiLockPool.resize(SIZE, NULL);



		}

		bool IsOverlap(interval *inv, int m, int threadID)
		{
			//cout<<"m=1";
			pthread_mutex_lock(&mutex);
			inv->MySeq = ++Seq;
			Array[threadID] = inv;
			pthread_mutex_unlock(&mutex);

			if(m == 1)
			{	
				for(int i=0; i< SIZE; i++)
				{
					if(Array[i] != NULL)
					{	interval *ptr = Array[i];	
						//wait untill there is an overlap and my sequence number is greater
						while(ptr !=NULL && (ptr->pre <= inv->post && ptr->post >= inv->pre) && ptr->MySeq < inv->MySeq)
						{
							ptr = Array[i];
						}

					}
				}

				return false;

			} 
		}

		void Insert(interval *inv, int index)
		{

			//pthread_rwlock_wrlock(&ArrayLock[index]);	
			Array[index] = inv;
			//pthread_rwlock_unlock(&ArrayLock[index]);

		}

		void Delete(int index)
		{

			Array[index] = NULL;


		}

		//*****************************************************************************
		//This function checks whether the multiple query nodes overlap with the lock pool
		//*****************************************************************************
		bool MultiOverlap(interval **QueryNodes, int Qsize, int m, int threadID)
		{

			VectorSize[threadID] = Qsize;

			pthread_mutex_lock(&mutex);
			MySeq[threadID] = ++Seq;
			MultiLockPool[threadID] = QueryNodes;
			pthread_mutex_unlock(&mutex);

			if(m == 1)
			{	
				for(int i=0; i< SIZE; i++)
				{

					interval** ptr = MultiLockPool[i];
					if(ptr != NULL)
					{		
						for(int j=0;j<VectorSize[i];j++)
						{
							for(int k=0;k<Qsize;k++)
							{
								while(ptr!=NULL && MySeq[threadID] > MySeq[i] && ptr[j]->pre <= QueryNodes[k]->post && ptr[j]->post >= QueryNodes[k]->pre)
								{
									ptr = MultiLockPool[i];
									if(ptr == NULL){
									k = Qsize;
									break;
									}


								}
							}

						}
					}

				}


				return false;

			} 

		}


		//This function deletes the vector entry from MultiLockPool, i.e., UnLock
		void MultiDelete(int index)
		{


			MultiLockPool[index] = NULL;

		}
};

#endif
