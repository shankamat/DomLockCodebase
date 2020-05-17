#include<fstream>
#include<map>
#include<unordered_map>
#include<math.h>
#include<vector>
#include<set>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<algorithm>
#include<unistd.h>//For usleep()
#include"lock.h"
#include"interval.h"
using namespace std;

int degree;
int maxdeg=0;
int TTID;
bool XMLFlag=0;
extern int IncrementalUpdates;
extern IntervalCheck ICheck;
extern TreeNode** Array;
int leafNodes;
extern char CSSize;
extern double threshold;
extern double alpha;
extern int maxRequestedNodes;
extern double avgDegOfConc;

bool myfunction (TreeNode* i,TreeNode* j) 
{ 
	return (i->data < j->data); 
}

bool compPreNumber (TreeNode* i,TreeNode* j) 
{ 
	return (i->preNumber < j->preNumber); 
}

bool comp (interval *i,interval *j) 
{ 
	return (i->pre<j->pre); 
}
bool Vcomp (pair<int,int>a, pair<int,int>b)
{
	return (a.second < b.second);  
}

int PinPoint(vector<int>&K)
{
	int n = leafNodes,k;
	int K_Size = K.size();
	vector<double>Batches(K_Size);
	int minIndex;
	double minExecTime = 10000.0;
	for(int i_k = 0; i_k<K_Size;i_k++)
	{
		k = K[i_k];
		double perThreadTime = ((double)i_k*0.00004201) + 0.000551;
		double fr=1;
		int i,j;
		Batches[i_k] = 1 + ceil(15 * ((double)k/(4*n)));
		double newExec = Batches[i_k]*perThreadTime;
		if(minExecTime > newExec)
		{	
			minExecTime = newExec;
			minIndex = i_k;
		}
	}
	return minIndex;
}

class Tree
{
	public:
		pthread_mutex_t rootLock;
		int size;
		TreeNode *head,*nextNode;

		//Leaf node list XML hierarchy
		vector<TreeNode*> LeafList;
		double counter;//dfs counting
		map<int,string> PathRef;  //Path to  a node.
		map<int,TreeNode *> NodeRef;  //map of <key,address> for each tree node.

		//Member functions************
		//Constructor
		Tree();
		//Preorder traversal
		void preorder(TreeNode *);

		//Discover all parents of a node
		void findAllParents(TreeNode *ptr, vector<int>& V, int, int);

		//Insert new node into tree
		void insert(TreeNode *ptr);

		//Set pointer to a for next insertion operation
		void setNextNode();

		//Save direct address of each Node
		void setNodeRef(TreeNode *ptr);

		//Save path from head to node
		void setPathToNode(int n);

		//Print reference map
		void printMap();

		//Print path map
		void printPath();

		//Create Tree
		void CreateTree(int);

		//Concurrent function
		void DummyTask(int node, double &currDegOfConc);

		//Depth first search
		void DFS(TreeNode *ptr);

		//Updated parent intervals according to intervals of child nodes
		bool UpdateParent(TreeNode *parent, TreeNode *ptr);

		//New interval numbering algorithm
		void ModifiedDFS(TreeNode *ptr);

		//Generates the locking option using Greedy Approximation
		vector<interval*> GenerateLockingOptions( vector<interval *>&IntervalArray, int&bestOption);	

		//Generates all possible locking options
		vector<vector<int> > GenerateAllLockingOptions( TreeNode *root, vector<interval *>&IntervalArray);	

		void CreateN_aryTree(int n); 

		TreeNode* FindDominator(TreeNode *ptr, double min, double max);
		//************************************************************************************
		//This function is used as a task to be executed after locking all desired nodes 
		//************************************************************************************
		void NullOperation(int TaskCSSize = -1 )
		{
			int itr;
			if(CSSize == 's' || TaskCSSize == 0)
				itr = 1000;
			else if(CSSize == 'm' || TaskCSSize == 1)
				itr = 10000;
			else if(CSSize == 'l' || TaskCSSize == 2)
				itr = 10000000;

			for(int i=0; i<itr; i++)
			{		
				int x = 5, y;
				y = x;
			}
		}

		//Overloaded function for null operation with Round-robin scheduler
		void NullOperation(int low, int high, int tid, int cs_size)
		{
			interval *inv = new interval(low, high, 1);
		
			//Acquire DomLock
			ICheck.IsOverlap(inv, 1, tid);
		

			//Call dummay null operation
			NullOperation(cs_size);

			//Unlock DomLock
			ICheck.Delete(tid);
		
		}

};//end of class "Tree"



TreeNode* Tree::FindDominator(TreeNode *ptr, double min, double max)
{
	for(int i=0;i<ptr->neighbour.size();i++)
	{
		if(ptr->neighbour[i] != NULL && ptr->neighbour[i]->preNumber <= min && ptr->neighbour[i]->postNumber >= max)
		{   
			ptr = ptr->neighbour[i];
			i=-1;
		}
	}
	return ptr;
}

//************************************************************************************
//constructor
//************************************************************************************
Tree::Tree()
{
	head = NULL;
	nextNode = NULL;
	counter = 0.0;
	size = 0;
}

//************************************************************************************
//This function creates Tree with number of nodes as parameter
//************************************************************************************
void Tree::CreateTree(int n)
{
	for(int i=1;i<=n;i++)
	{
		TreeNode *newNode=new TreeNode(i);

		this->insert(newNode);
	}
}



//************************************************************************************
//This function saves path from root to perticular node: l=letf pointer r=right pointer
//stores path in string e.g. node 5=lr i.e "root->right->left" starts from end of string.
//************************************************************************************
void Tree::setPathToNode(int n)
{

}


//************************************************************************************
//This function saves direct reference to nodes in tree
//e.g. node 5= #12h13
//************************************************************************************
void Tree::setNodeRef(TreeNode *ptr)
{

	NodeRef.insert(std::pair<int,TreeNode *>(ptr->data,ptr));
	//  NodeVector.push_back(ptr);
}


//************************************************************************************
//This function inserts new node in tree
//************************************************************************************
void Tree::insert(TreeNode *ptr)
{
}


//************************************************************************************
//This function updates pointer to a node for next insert operation
//************************************************************************************
void Tree::setNextNode()
{
}


//************************************************************************************
//Prints references to each node
//************************************************************************************
void Tree::printMap()
{
	//Print graph
	cout<<"\ngraph {"<<endl;
	for(int i=1;i<=size;i++)
	{	
		cout<<i<<" -- {";
		for(int j=0;j<Array[i]->neighbour.size();j++)
		{
			if(i>Array[i]->neighbour[j]->data)
				cout<<i<<"-"<<(Array[i]->neighbour[j])->data<<"\n";
		}
		cout<<"};"<<endl;
	}

	cout<<"}"<<endl;
}


//************************************************************************************
//Prints path in lr string form
//************************************************************************************
void Tree::printPath()
{
}

//************************************************************************************
//Prints preorder sequence for the binary tree  .. Substituted it as printing visual graph
//************************************************************************************

void Tree::preorder(TreeNode *p)
{
	cout<<"graph {"<<endl;
	for(int i=1;i<=size;i++)
	{	
		cout<<i<<" -- {";
		for(int j=0;j<Array[i]->neighbour.size();j++)
		{
			cout<<(Array[i]->neighbour[j])->data<<" ";

		}
		cout<<"};"<<endl;
	}

	cout<<"}"<<endl;
}

//**************************************************************************************
//This function finds all parents  of vertex pointed by *ptr and stores them in vector V
//**************************************************************************************
void Tree::findAllParents(TreeNode *p, vector<int>&V, int d, int dc)
{

	int pdata = p->data;
	int psize = (p->parents).size();
	if(pdata > 1)
	{
		bool IsNew = std::find(V.begin(), V.end(), pdata) == V.end();
		if ( IsNew && pdata != d && pdata != dc)				
		{
			IXLock(Array[pdata]);

			V.push_back(pdata);

		}	
		if(IsNew)
		{
			for(int i=0; i<psize; i++)		
			{
				//	if(p->parents[i])
				//	{	
				//		int x = (p->parents[i])->data;	
				//	if ( std::find(V.begin(), V.end(), x) == V.end() )
				findAllParents(p->parents[i],V, d, dc);
				//	}
			}
		}
	}
}

//************************************************************************************
//DFS algorithm for interval numbering 
//************************************************************************************
void Tree::DFS(TreeNode* ptr)
{
}//End of DFS

//************************************************************************************
//Function ModifiedDFS() uses this function to update the parents interval value
//************************************************************************************
bool Tree::UpdateParent(TreeNode *par, TreeNode *ptr)
{
	if(par == NULL || (par->preNumber == ptr->preNumber && par->postNumber == ptr->postNumber))
		return true;

	if(par->preNumber == 0 && par->postNumber == 0)
	{
		par->preNumber = ptr->preNumber;
		par->postNumber = ptr->postNumber;	
	}
	else 
	{	
		if(par->preNumber > ptr->preNumber && ptr->preNumber!=0)
			par->preNumber = ptr->preNumber;
		if(par->postNumber < ptr->postNumber &&ptr->postNumber!=0)
			par->postNumber = ptr->postNumber;	

	}
	//Special case for cycles
	if(par->parentUpdated == true)
	{
		for(int i=0;i<par->parents.size();i++)
			UpdateParent(par->parents[i], par);
	}
	return true;

}//EndOf UpdateParents()

//************************************************************************************
//Modified DFS algorithm for assigning intervals to data structure nodes.
//************************************************************************************
void Tree::ModifiedDFS(TreeNode* ptr)
{
	// check if root is NULL
	if(ptr == NULL)
		return;

	//send updates to parent node if the node is already explored
	if(ptr->IsExplored)	
		return;

	//if node is a leaf node or node is a part of some cycle, consider it as leaf node and assign intervals
	if(ptr->neighbour.size() == 0  || ptr->active == true)
	{
		ptr->sizeOfSubtree = 1;
		counter+=1;
		ptr->preNumber = counter;
		ptr->postNumber = counter;		
		//insert into vector for multiple intervals
		ptr->multiInterval.push_back(make_pair(counter,counter));
	}
	else
	{
		ptr->active = true;
		//call recursively modifiedDfS() function for all the child nodes. 
		//As we use binary tree, here for left and right pointers only.
		ptr->sizeOfSubtree = 1;		
		for(int i=0;i<ptr->neighbour.size();i++)
		{		
			ModifiedDFS(ptr->neighbour[i]);
			ptr->sizeOfSubtree += (ptr->neighbour[i]->sizeOfSubtree);			
		}
	}
//	cout<<ptr->data<<" "<<ptr->sizeOfSubtree<<" [ "<<ptr->preNumber<<" , "<<ptr->postNumber<<" ]"<<endl;

	ptr->IsExplored = true;
	ptr->active = false;

	//propagate interval information to all parents	
	for(int i=0;i<ptr->parents.size();i++)
		UpdateParent(ptr->parents[i], ptr);
	ptr->parentUpdated = true;
	return;


}//end of modifiedDFS();

//************************************************************************************
//This function creates N-ary tree 
//************************************************************************************
void Tree::CreateN_aryTree(int n)
{
	size = n;
	int MAX_NODES = n;
	int N_ary = 4;//2 for binary tree
	leafNodes = (size/2)-1;
	//create all tree nodes at once and store their addresses in "Array"	
	for(int i = 1; i<=MAX_NODES;i++)
	{
		TreeNode *temp=new TreeNode(i);
		Array[temp->data] = temp;
	}
	head = Array[1];	
	int ptr = 2;
	//create links according to N_ary
	for(int i=1;(i<=MAX_NODES && ptr<=MAX_NODES);i++)
	{
		for(int j=0; (j<N_ary && ptr<=MAX_NODES); j++)
		{
			(Array[i]->neighbour).push_back(Array[ptr]);
			(Array[ptr]->parents).push_back(Array[i]);
			ptr++;
		}
	}
//	cout<<"Leaf list: "<<endl;
	for(int i=1;i<=MAX_NODES;i++)
	{
		if(Array[i]->neighbour.size() ==  0)
			LeafList.push_back(Array[i]);
//		cout<<Array[i]->data<<" "<<endl;
	}	
	leafNodes = LeafList.size();
//	cout<<"\n"<<leafNodes<<" leaf nodes..!!"<<endl;
}

//************************************************************************************
//this function is a dummy task which gets called parallelly.
//************************************************************************************
void Tree::DummyTask(int node, double &currDegOfConc)
{
	//return;
	int threadID = node;
	extern int caseParameter;
	extern int NoOfRequestedNode;
	extern int distribution;
	// write access:1, read access:0
	int accessType = 1;

	TreeNode* NodeArray[NoOfRequestedNode];
	set<int> nodeSet;
	int NoOfLeafNodes = leafNodes;

	int SlotSize = size/distribution;
	if(SlotSize==0){
		//cout<<"Check";		
		currDegOfConc = 0;
		return;
	}
	int SlotNo = rand()%distribution;

	// Dominator locking ***************************************case 5
	if( caseParameter == 5)
	{
		double min=0, max=0;
		for(int i = 0;i < NoOfRequestedNode;i++)
		{
			int NodeIndex = 1+SlotSize*SlotNo+((rand()%SlotSize));
			if(NodeIndex>size)
				NodeIndex = size;
			//cout<<NodeIndex<<endl;
			//getchar();			
			NodeArray[i] = Array[NodeIndex];
			nodeSet.insert(NodeArray[i]->data);
			if(min == 0 || NodeArray[i]->preNumber < min)
			{
				min = NodeArray[i]->preNumber;
			}
			if(max == 0 || NodeArray[i]->postNumber > max)
			{
				max = 	NodeArray[i]->postNumber;
			}

		}
		
		TreeNode *ptr = head;
	//	cout<<"Before: "<<ptr->data<<" "<<ptr->preNumber<<" "<<ptr->postNumber<<endl;
//		cout<<"[min, max] = [ "<<min<<" , "<<max<<" ] "<<endl;
		for(int i=0;i<ptr->neighbour.size();i++)
		{
			if(ptr->neighbour[i] != NULL && ptr->neighbour[i]->preNumber <= min && ptr->neighbour[i]->postNumber >= max)
			{   
				ptr = ptr->neighbour[i];
				i = -1;
			}

		}
//		cout<<"# "<<nodeSet.size()<<" "<<ptr->sizeOfSubtree<<endl;
		currDegOfConc = nodeSet.size()/ptr->sizeOfSubtree;
//		cout<<"DOC "<<currDegOfConc<<endl;
//		if(nodeSet.size()>(ptr->sizeOfSubtree) && ptr!=NULL)
//			cout<<"Wrong!! Data: "<<ptr->data<<" [ "<<min<<" , "<<max<<" ] "<<nodeSet.size()<<" "<<ptr->sizeOfSubtree<<endl;
//		cout<<"Dominator:"<<endl;
//		cout<<ptr->data<<" "<<ptr->preNumber<<" "<<ptr->postNumber<<endl;
		//Inserts interval of dominator into interval data structure. ptr is a dominator node.
		interval *inv = new interval(ptr->preNumber,ptr->postNumber,accessType);
xy:	if(!ICheck.IsOverlap(inv, 1, threadID))
	{	
		pthread_rwlock_wrlock(&ptr->rwlock);

		//Call dummay null operation
		NullOperation();

		ICheck.Delete(threadID);
		pthread_rwlock_unlock(&ptr->rwlock);

	}
	else { usleep(10);goto xy;}
	}
	else if( caseParameter == 10){
		double min=0, max=0;
		for(int i = 0;i < NoOfRequestedNode;i++)
		{
			int NodeIndex = 1+SlotSize*SlotNo+((rand()%SlotSize));
			if(NodeIndex>size)
				NodeIndex = size;
			//cout<<NodeIndex<<endl;
			//getchar();			
			NodeArray[i] = Array[NodeIndex];
			nodeSet.insert(NodeArray[i]->data);
			if(min == 0 || NodeArray[i]->preNumber < min)
			{
				min = NodeArray[i]->preNumber;
			}
			if(max == 0 || NodeArray[i]->postNumber > max)
			{
				max = 	NodeArray[i]->postNumber;
			}

		}
		TreeNode *ptr = head;
		
		for(int i=0;i<ptr->neighbour.size();i++)
		{
			if(ptr->neighbour[i] != NULL && ptr->neighbour[i]->preNumber <= min && ptr->neighbour[i]->postNumber >= max)
			{   
				ptr = ptr->neighbour[i];
				i = -1;
			}

		}

//		cout<<"Dominator:"<<endl;
//		cout<<ptr->data<<" "<<ptr->preNumber<<" "<<ptr->postNumber<<endl;
		
		//Calculate current degOfConc for respective node 
		// degOfConc = alpha*oldVal + (1-alpha)*currentVal
		ptr->degOfConc = (ptr->degOfConc == 0) ? 
						 (nodeSet.size()/(ptr->sizeOfSubtree)) : 
						 (alpha*(ptr->degOfConc)+(1-alpha)*(nodeSet.size()/(ptr->sizeOfSubtree)));
		if( (ptr->degOfConc)>threshold){		
//			cout<<"Path 1"<<endl;			
			currDegOfConc = nodeSet.size()/ptr->sizeOfSubtree;			
			//Inserts interval of dominator into interval data structure. ptr is a dominator node.
			interval *inv = new interval(ptr->preNumber,ptr->postNumber,accessType);
x1y1:		if(!ICheck.IsOverlap(inv, 1, threadID))
			{	
				pthread_rwlock_wrlock(&ptr->rwlock);
				//Call dummay null operation
				NullOperation();
				ICheck.Delete(threadID);
				pthread_rwlock_unlock(&ptr->rwlock);
			}
			else { usleep(10);goto x1y1;}
		}else{
//			cout<<"Path 2"<<endl;
			vector<pair<int, int> > listOfRanges;
			double totalSubtreeSize = 0;			
						
			for(int i=0;i<NoOfRequestedNode;i++){
				TreeNode* ptr = NodeArray[i];
				int j=0;
				for(;j<listOfRanges.size();j++){
					pair<int, int> currRange = listOfRanges[j];
					if(ptr->preNumber <= currRange.second && ptr->postNumber >= currRange.first){
						int leftRange = (ptr->preNumber)<currRange.first ? (ptr->preNumber) : currRange.first;
						int rightRange = (ptr->postNumber)>currRange.second ? (ptr->postNumber) : currRange.second;
						listOfRanges[j] = make_pair(leftRange, rightRange);
						break;
					}else if(ptr->preNumber >= currRange.first && ptr->postNumber <= currRange.second)
						break;			
				}
				if(j==listOfRanges.size())
					listOfRanges.push_back(make_pair(ptr->preNumber, ptr->postNumber));
			}

			for(int j=0;j<listOfRanges.size();j++){
				pair<int, int> currRange = listOfRanges[j];
				TreeNode *ptr = head;	
				for(int i=0;i<ptr->neighbour.size();i++)
				{
					if(ptr->neighbour[i] != NULL && ptr->neighbour[i]->preNumber <= currRange.first && ptr->neighbour[i]->postNumber >= currRange.second)
					{   
						ptr = ptr->neighbour[i];
						i = -1;
					}
				}

				totalSubtreeSize+=(ptr->sizeOfSubtree);
				interval *inv = new interval(ptr->preNumber,ptr->postNumber,accessType);
x2y2:			if(!ICheck.IsOverlap(inv, 1, threadID))
				{	
					pthread_rwlock_wrlock(&ptr->rwlock);
					//Call dummay null operation
					NullOperation();
					ICheck.Delete(threadID);
					pthread_rwlock_unlock(&ptr->rwlock);
				}
				else { usleep(10);goto x2y2;}				
			}
			currDegOfConc = nodeSet.size()/totalSubtreeSize;			

			}

	}

}//end of DummyTask()













