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
int main(int argc, char *argv[])
{	
	//srand( time(NULL) );
	srand(0);	
	err *e=new err();
	Tree *T=new Tree();
	thread *th=new thread();
	int NoOfNodesDS = atoi(argv[1]);//size of hierarchy
	int NoOfThreads = atoi(argv[2]); // No of threaads
	caseParameter = atoi(argv[3]); // DomLock: 5
	NoOfRequestedNode = atoi(argv[4]); //exp: 32
	distribution = atoi(argv[5]); //1, 2, 4,8, 16 -> 1 is full ramdom
	CSSize = argv[6][0]; // s, m, l
	option = atoi(argv[7]); // it is for NumLock
	N=atoi(argv[1]); // Size of hierarchy
	char InputHierarchy = argv[8][0];   
	threshold = atoi(argv[9]);
	alpha = atoi(argv[10]);
	threshold/=10;	
	alpha/=10;
	Array = new TreeNode*[N+1];
	for(int i=0;i<32;i++)
		degOfConc[i] = 0;	
	if(e->errCheck(argc, argv))
	{
		if(InputHierarchy == 't')
			T->CreateN_aryTree(NoOfNodesDS);
	}
	cout<<NoOfNodesDS<<", ";
	cout<<NoOfThreads<<", ";
	if(caseParameter==5)
		cout<<"D, ";
	else if(caseParameter==10)
		cout<<"MD, ";
	cout<<NoOfRequestedNode<<", ";
	cout<<distribution<<", ";
	cout<<threshold<<", ";
	cout<<alpha<<", ";
	//Data Structure creation finished 
	//Numbering Algorithm started
	T->ModifiedDFS(T->head);
	sort(T->LeafList.begin(), T->LeafList.end(), compPreNumber);
	//cout<<"scheduler:"<<endl;
	NUM_WORKER_THREADS = NoOfThreads - 2;
	scheduler *sc = new scheduler(T, 100, NoOfThreads - 2);
	double average=0;
	
	//Thread creation start
	struct timeval tp;
	gettimeofday(&tp,NULL);
	double start = tp.tv_sec + tp.tv_usec/1000000.0;
	th->CreateThread(NoOfThreads, T, sc);//defined in "parallelPthread.h"
	gettimeofday(&tp,NULL);
	double finish = tp.tv_sec + tp.tv_usec/1000000.0;
	double result = finish - start;
	average += result;    
	ITNode *root = NULL;

//	sc->PrintStats();
	cout<<average/NoOfThreads<<", "; //5
	double avgDegOfConc = 0;	
	int nonZeroCnt=0;
	for(int i=0;i<NoOfThreads-2;i++){
		avgDegOfConc+=degOfConc[i];
		if(degOfConc[i]>0)
			nonZeroCnt++;
	}
	
	avgDegOfConc = nonZeroCnt==0 ? 0 : avgDegOfConc/nonZeroCnt;
	cout<<avgDegOfConc<<endl;
//	T->preorder(T->head);
}

