#include<iostream>
#include"Global.h"
#include"Tree.h"
#include"Scheduler.h"
#include"errorHandler.h"
#include<stdlib.h>
#include<string.h>
#include"ParallelPthread.h"
#include <sys/time.h>
using namespace std;
IntervalCheck ICheck;

TreeNode** Array;
vector<vector<TreeNode*> > Proxy;

int main(int argc, char *argv[])
{	
	//srand( time(NULL) );
	srand(0);	
	err *e=new err();
	Tree *T=new Tree();
	thread *th=new thread();
	N_ary = atoi(argv[1]);
	MaxLevel = atoi(argv[2]);
	NoOfRequestedNode = atoi(argv[3]); // Exp: 32
	radius = atoi(argv[4]);
	shuffleProb = atoi(argv[5]);		
	//int NoOfNodesDS = atoi(argv[1]);// Size of hierarchy
	int NoOfThreads = atoi(argv[6]); // No of threaads
	caseParameter = atoi(argv[7]); // DomLock: 5
	distribution = atoi(argv[8]); //1, 2, 4,8, 16 -> 1 is full ramdom
	CSSize = argv[9][0]; // s, m, l
	option = atoi(argv[10]); // it is for NumLock
	N= (pow(N_ary,MaxLevel)-1)/(N_ary-1); // Size of hierarchy
	int NoOfNodesDS = N;	
	char InputHierarchy = argv[11][0];   
	Array = new TreeNode*[N+1];
	
	srand(0);

	for(int i=0;i<32;i++)
		degOfConc[i] = 0;	
	if(e->errCheck(argc, argv))
	{
		if(InputHierarchy == 't')
			T->CreateN_aryTree(NoOfNodesDS);
		if(InputHierarchy == 'd')
			T->CreateN_aryDAG(NoOfNodesDS);
	}
	//Data Structure creation finished 
	//Numbering Algorithm started
//	T->ModifiedDFS(T->head);
	//cout<<"Complete"<<endl;
	struct timeval tp;
	gettimeofday(&tp,NULL);
	double start = tp.tv_sec + tp.tv_usec/1000000.0;
	T->RenumberingAlgorithm(T->head);
	gettimeofday(&tp,NULL);
	double finish = tp.tv_sec + tp.tv_usec/1000000.0;
	double result1 = finish - start;
	
	sort(T->LeafList.begin(), T->LeafList.end(), compPreNumber);
	//cout<<"scheduler:"<<endl;
	NUM_WORKER_THREADS = NoOfThreads - 2;
	scheduler *sc = new scheduler(T, 100, NoOfThreads - 2);
	double average=0;
	
	//Thread creation start
	gettimeofday(&tp,NULL);
	start = tp.tv_sec + tp.tv_usec/1000000.0;
	th->CreateThread(NoOfThreads, T, sc);//defined in "parallelPthread.h"
	gettimeofday(&tp,NULL);
	finish = tp.tv_sec + tp.tv_usec/1000000.0;
	double result2 = finish - start;
	average += result2;    
	ITNode *root = NULL;

//	sc->PrintStats();
	cout<<N_ary<<" "<<MaxLevel<<" ";
	cout<<NoOfRequestedNode<<" "<<radius<<" "<<shuffleProb<<" ";
	cout<<NoOfThreads<<" "<<caseParameter<<" "<<distribution<<" "; 
	cout<<result1<<" "<<average/NoOfThreads<<" "; //5
	double avgDegOfConc = 0;	
	int nonZeroCnt=0;
	for(int i=0;i<NoOfThreads-2;i++){
		avgDegOfConc+=degOfConc[i];
		if(degOfConc[i]>0)
			nonZeroCnt++;
	}
	
	avgDegOfConc = nonZeroCnt==0 ? 0 : avgDegOfConc/nonZeroCnt;
	cout<<avgDegOfConc<<endl;
//	T->preorder(T->head);*/
}

