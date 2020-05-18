#ifndef Schedular_H
#define Schedular_H

class Task
{
	public:
		//The node to be accessed while ;erforming task
		vector<TreeNode*> RequestedNodes;

		//Request interval range
		double LowRange, HighRange;

		//Access mode: Read or write
		bool AccessMode;

		//Critical section size
		int CriticalSectionSize;

		//Constructor
	 	Task(Tree* hierarchy, int n, int skewness)
		{
			//Generate a skewed request
			GenerateSkewedRequest(hierarchy, n, skewness);

			//Genarate access mode 0:read 1:write
			AccessMode = rand() % 2;

			//Generate size of critical section
			// 0: small
			// 1: medium
			// 2: large
			CriticalSectionSize = 0;//rand() % 3; 
		}
		
		//The generated request is skewed: Each requested node falls into one specific bucket
		void GenerateSkewedRequest(Tree* hierarchy, int n, int skewness)
		{
			//Generate nodes to be accessed
			int SlotSize = ceil((double)(leafNodes)/skewness);
		
		//	int SlotNo = rand() % skewness;
			int SlotNo = 0;//rand() % skewness;
				
			int Offset = SlotSize * SlotNo;
			n = 1;
			LowRange = hierarchy->size;
			HighRange = 0;
			for(int i = 0; i < n; i++)
			{
				int LeafListIndex = ((rand()%SlotSize));

				LeafListIndex += Offset;
				LeafListIndex = min(LeafListIndex, leafNodes - 1);

				TreeNode* node = hierarchy->LeafList[LeafListIndex];
				LowRange = min(LowRange, node->preNumber - 1);//-1 is for ease of bucket hashing
				HighRange = max(HighRange, node->postNumber - 1);
			
				RequestedNodes.push_back(node);
			}
		
		
		}

		void GenerateSingleBucketRequest(Tree* hierarchy, int n, int skewness)
		{
			
		}

};

class TaskPool
{
	public:

		//Termination condition
		int* ScheduledTaskCount;
		//two task queue pointers
		vector<Task**> Queue;

		//Index of Active queue for producer
		int ProdActiveQueue;

		//Index of Active queue for consumer
		int ConsActiveQueue;

		//Producer's current index in the active queue
		int PIndex;

		//Size of active queue for producer
		int PASize;

		//Consumer's current index in the active queue
		int CIndex;

		//size of active queue for consumersi
		int CASize;

		//Constructor

		TaskPool(int TaskPoolSize, int* ScheduledTaskCount)
		{

			this->ScheduledTaskCount = ScheduledTaskCount;

			Queue.resize(2);

			Queue[0] = NULL;
			Queue[1] = NULL;
			//Default active queue
			Queue[0] = (Task**) malloc(sizeof(Task) * TaskPoolSize);

			for(int i=0; i<TaskPoolSize; i++)
				Queue[0][i] = NULL;

			ProdActiveQueue = 0;
			ConsActiveQueue = 0;

			PIndex = 0;
			CIndex = 0;

			PASize = TaskPoolSize;
			CASize = TaskPoolSize;
		
		}

		//Add task to task-pool
		bool AddTask(Task* task)
		{
			if(Queue[ProdActiveQueue][PIndex] != NULL)
			{
				
				ProdActiveQueue = (ProdActiveQueue + 1) % 2;

				while(Queue[ProdActiveQueue] != NULL);
				
				PASize = PASize * 2;
				Queue[ProdActiveQueue] = (Task**) malloc(sizeof(Task) * PASize);

				for(int i=0;i< PASize; i++)
					Queue[ProdActiveQueue][i] = NULL;

				PIndex = 0;
			}
			Queue[ProdActiveQueue][PIndex] = task;

			PIndex = (PIndex + 1) % PASize;
		}

		//Extract task from the task-pool
		Task* ExtractTask()
		{
			// wait if there is no task in current queue AND the second queue is also empty.
			while(Queue[ConsActiveQueue][CIndex] == NULL && Queue[(ConsActiveQueue + 1) % 2] == NULL && *ScheduledTaskCount < TOTAL_NUM_TASKS);
			
			if(Queue[ConsActiveQueue][CIndex] == NULL && Queue[(ConsActiveQueue + 1) % 2] == NULL && *ScheduledTaskCount >= TOTAL_NUM_TASKS)
				return NULL;

			//If current queue is not empty.
			if(Queue[ConsActiveQueue][CIndex] != NULL)
			{
				Task* task = Queue[ConsActiveQueue][CIndex];
				Queue[ConsActiveQueue][CIndex] = NULL;
				CIndex = (CIndex + 1) % CASize; 

				return task;
			}
			//If the current queue is empty and the second queue is not empty
			else if(Queue[(ConsActiveQueue + 1) % 2] != NULL)
			{
				delete Queue[ConsActiveQueue];

				Queue[ConsActiveQueue] = NULL;
				ConsActiveQueue = (ConsActiveQueue + 1) % 2;
				CIndex = 0;
				CASize = PASize;

				while(Queue[ConsActiveQueue][CIndex] == NULL && *ScheduledTaskCount < TOTAL_NUM_TASKS);
				if(Queue[ConsActiveQueue][CIndex] == NULL && *ScheduledTaskCount >= TOTAL_NUM_TASKS)
					return NULL;
				
				Task* task = Queue[ConsActiveQueue][CIndex];
                                Queue[ConsActiveQueue][CIndex] = NULL;
                                CIndex = (CIndex + 1) % CASize;

                                return task; 
			}
		}
};

class scheduler
{
	public:
		//hierarchical data-structure
		Tree* hierarchy;
		
		//Number of worker threads
		int NWorkers;

		//Task-pool: circular queue implemented as array
		TaskPool* taskPool;

		//buckets per worker thread
		vector<TaskPool*> Buckets;

		//The count of No. of tasks performed per thread
		vector<int> TasksPerThread;
	
		//Size of each bucket: Number of tasks mapped to single bucket
		int BucketSize;

		//Max number of tasks to be created
		int TotalNumTasks;

		//Count of the number of tasks created
		int CreatedTasks;

		//Count of the scheduled tasks
		int ScheduledTasks;

		//constructor
		scheduler(Tree* hierarchy,int TaskPoolSize, int NWorkers)
		{
			this->hierarchy = hierarchy;
			this->NWorkers = NWorkers;
			TotalNumTasks = TOTAL_NUM_TASKS;
			CreatedTasks = 0;
			ScheduledTasks = 0;
			
			//init task-pool
			this->taskPool = new TaskPool(TaskPoolSize, &ScheduledTasks);

			//init task-buckets for each worker thread
			Buckets.resize(NWorkers); 

			//init task count vector
			TasksPerThread.resize(NWorkers, 0);
			for(int i = 0; i<Buckets.size(); i++)
			{
				Buckets[i] = new TaskPool(TaskPoolSize, &ScheduledTasks);
			}

			BucketSize = ceil((double)(hierarchy->LeafList).size()/NWorkers);
			//TestInit();
		}

		//Repetitively generates tasks and add it into the task-pool
		//Caller: Task generator thread
		void TaskGen()
		{
			while(CreatedTasks < TotalNumTasks)
			{
				//The function parameters are input dependent
				taskPool->AddTask(new Task(hierarchy, NoOfRequestedNode, distribution));
				CreatedTasks++;
			}

		}

		//Scheduler interface
		//0: Round-robin
		//1: Bucketization
		void TaskScheduler(int SchedulingAlgo)
		{
			if(SchedulingAlgo == 0)
			{
				RRScheduler();
			}
			else if(SchedulingAlgo == 1)
			{
				TaskScheduler();
			}
		
		}

		//Round-robin scheduling
		void RRScheduler()
		{
			int NextThread = 0;
			while(ScheduledTasks < TotalNumTasks)
			{
				Task* task = taskPool->ExtractTask();
				Buckets[NextThread]->AddTask(task);
				ScheduledTasks++;
				NextThread = (NextThread + 1) % NWorkers;

			}
		
		}

		//Extracts tasks from task-pool and schedule it to worker thread
		//Caller: Task scheduler thread
		void TaskScheduler()
		{
			while(ScheduledTasks < TotalNumTasks)
			{
				Task* task = taskPool->ExtractTask();

				//Insert task into hash-bucket
				//The task may span across multiple buckets, therefore the BucketLowIndex, BucketHighIndex
				//refer to the range of buckets.
				int BucketLowIndex = (task->LowRange / BucketSize);
				int BucketHighIndex = (task->HighRange / BucketSize);
			
				//if task spans single bucket
				if(BucketLowIndex == BucketHighIndex)
				{
					Buckets[BucketLowIndex]->AddTask(task);
					ScheduledTasks++;

					//printf("Bucket %d: %d\n", BucketLowIndex, Buckets[BucketLowIndex]->Producer);
				}
				else
				{
					//decide policy..
					Buckets[BucketLowIndex]->AddTask(task);
					ScheduledTasks++;
				}
				
			}
		}

		//Worker thread processing
		void WorkerThread(int ThreadId)
		{
			int mycount = 0;
			TaskPool *taskPool = Buckets[ThreadId];
			while(true)
			{
				//Thread-specific bucket
				Task* task = taskPool->ExtractTask();
				if(task == NULL)
				{
					//printf("Thread %d: Bucket size %d\n", ThreadId, taskPool->Producer);
					TasksPerThread[ThreadId] = mycount;
					return;
				}
				(this->hierarchy)->NullOperation(task->LowRange, task->HighRange, ThreadId, task->CriticalSectionSize);
				mycount++;
				delete task;
			}
		}

		//Output runtime statistics 
		void PrintStats()
		{
			cout<<"------------- Runtime stats -------------------\n";
			//print thread specific stats
			for(int i=0; i<NWorkers; i++)
			{
				cout<<"Thread "<<i<<":\t"<<TasksPerThread[i]<<endl;
			}
			
			int x = 5;
		}

		//Testing: initialization
		void TestInit()
		{
		}

};


#endif
