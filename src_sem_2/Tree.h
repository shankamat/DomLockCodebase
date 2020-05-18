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
extern int radius;
extern int shuffleProb;
extern vector<vector<TreeNode*> > Proxy;
extern int N_ary;

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

		void RenumberingAlgorithm(TreeNode* ptr);

        void ConnectDisconnectedNodes();
		//Generates the locking option using Greedy Approximation
		vector<interval*> GenerateLockingOptions( vector<interval *>&IntervalArray, int&bestOption);	

		//Generates all possible locking options
		vector<vector<int> > GenerateAllLockingOptions( TreeNode *root, vector<interval *>&IntervalArray);	

		void CreateN_aryTree(int n); 

		void CreateN_aryDAG(int n); 

		void ShuffleEdges(int n);

		void PrintLevelOrderTraversal();

		void Check_DFS(TreeNode* ptr, vector<int> &checklist);
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

void Tree::RenumberingAlgorithm(TreeNode* ptr){
	int currLevelNum = Proxy.size()-1;
	vector<TreeNode*> currLevel = Proxy[currLevelNum];
	double cnt=1;
	for(int i=0;i<currLevel.size();i++){
		TreeNode* currNode = currLevel[i];
		currNode->preNumber = currNode->postNumber = cnt++;
		currNode->sizeOfSubtree = 1;	
	}		
	while(currLevelNum>0){
		currLevelNum--;
		currLevel.clear();
		currLevel = Proxy[currLevelNum];
	
		for(int i=0;i<currLevel.size();i++){
			TreeNode* currParent = currLevel[i];	
			if(currParent->neighbour.size()>0){
				vector<TreeNode*> first;
				vector<TreeNode*> second;
				vector<TreeNode*> third;
				for(int j=0;j<currParent->neighbour.size();j++){
					TreeNode* currChild = currParent->neighbour[j];
					if(currChild->inDegree==1)						
						second.push_back(currChild);
					else{
						vector<TreeNode*> parentList = currChild->parents;
						for(int k=0;k<parentList.size();k++){
							if(parentList[k]->data < currParent->data){
								first.push_back(currChild);
								break;		
							}else if(parentList[k]->data > currParent->data){
								third.push_back(currChild);
								break;						
							}			
						}
					}
				}
				
				currParent->neighbour.clear();
				double subtreeSize = 1;
				double firstMaxPostNumber = 0;
				//Check this later 
				for(int k=0;k<first.size();k++){
//					cout<<"first"<<endl;					
					if((first[k]->postNumber+first[k]->maxShiftOffset) > firstMaxPostNumber)
						firstMaxPostNumber = first[k]->postNumber+first[k]->maxShiftOffset;
					currParent->neighbour.push_back(first[k]);
					subtreeSize += first[k]->sizeOfSubtree;
				}
				double secondMaxPostNumber = 0;
				for(int k=0;k<second.size();k++){
//					cout<<"C1"<<endl;
					second[k]->minShiftOffset += firstMaxPostNumber;
					second[k]->maxShiftOffset += firstMaxPostNumber;
					if(second[k]->postNumber+second[k]->maxShiftOffset > secondMaxPostNumber)
						secondMaxPostNumber = second[k]->postNumber+second[k]->maxShiftOffset;					
					currParent->neighbour.push_back(second[k]);
					subtreeSize += second[k]->sizeOfSubtree;
				}
				for(int k=0;k<third.size();k++){
//					cout<<"C2 "<<firstMaxPostNumber<<" "<<secondMaxPostNumber<<endl;
					third[k]->minShiftOffset += secondMaxPostNumber;
					third[k]->maxShiftOffset += secondMaxPostNumber;
					currParent->neighbour.push_back(third[k]);
					subtreeSize += third[k]->sizeOfSubtree;
				}		
				for(int k=0;k<second.size();k++){
					vector<double> minSet;
					vector<double> maxSet;					
					for(int k1=0;k1<first.size();k1++){
						if(second[k]->preNumber <= first[k1]->postNumber && second[k]->postNumber >= first[k1]->preNumber){
							minSet.push_back(first[k1]->preNumber+first[k1]->minShiftOffset);
							maxSet.push_back(first[k1]->postNumber+first[k1]->maxShiftOffset);
						}
					}
					for(int k1=0;k1<third.size();k1++){
						if(second[k]->preNumber <= third[k1]->postNumber && second[k]->postNumber >= third[k1]->preNumber){
							minSet.push_back(third[k1]->preNumber+third[k1]->minShiftOffset);
							maxSet.push_back(third[k1]->postNumber+third[k1]->maxShiftOffset);
						}
					}
					if(minSet.size()==0 && maxSet.size()==0)
						continue;
					double targetMin = minSet[0];
					double targetMax = maxSet[0];
					for(int k1=0;k1<minSet.size();k1++){
						if(minSet[k1]<targetMin)
							targetMin = minSet[k1];
						if(maxSet[k1]>targetMax)
							targetMax = maxSet[k1];
					}			
					second[k]->minShiftOffset = targetMin-second[k]->preNumber;
					second[k]->maxShiftOffset = targetMax-second[k]->postNumber;
						
				}
				double minPreNumber = currParent->neighbour[0]->preNumber;
				double maxPostNumber = currParent->neighbour[0]->postNumber;
				
				double minParentOffset = min(currParent->neighbour[0]->preNumber + currParent->neighbour[0]->minShiftOffset,currParent->neighbour[0]->preNumber + currParent->neighbour[0]->minParentOffset);
				double maxParentOffset = max(currParent->neighbour[0]->postNumber + currParent->neighbour[0]->maxShiftOffset,currParent->neighbour[0]->postNumber + currParent->neighbour[0]->maxParentOffset); 
				
				for(int k=0;k<currParent->neighbour.size();k++){
					if(currParent->neighbour[k]->preNumber < minPreNumber)
						minPreNumber = currParent->neighbour[k]->preNumber;
					if(currParent->neighbour[k]->postNumber > maxPostNumber)
						maxPostNumber = currParent->neighbour[k]->postNumber;

					if(currParent->neighbour[k]->preNumber + currParent->neighbour[k]->minShiftOffset < minParentOffset)
						minParentOffset = currParent->neighbour[k]->preNumber + currParent->neighbour[k]->minShiftOffset;
					if(currParent->neighbour[k]->preNumber + currParent->neighbour[k]->minParentOffset < minParentOffset)
						minParentOffset = currParent->neighbour[k]->preNumber + currParent->neighbour[k]->minParentOffset;

					if(currParent->neighbour[k]->postNumber + currParent->neighbour[k]->maxShiftOffset > maxParentOffset)
						maxParentOffset = currParent->neighbour[k]->postNumber + currParent->neighbour[k]->maxShiftOffset;
					if(currParent->neighbour[k]->postNumber + currParent->neighbour[k]->maxParentOffset > maxParentOffset)
						maxParentOffset = currParent->neighbour[k]->postNumber + currParent->neighbour[k]->maxParentOffset;
				}

				currParent->sizeOfSubtree = subtreeSize;
				currParent->preNumber = minPreNumber;
				currParent->postNumber = maxPostNumber;
				currParent->minParentOffset = minParentOffset-currParent->preNumber;
				currParent->maxParentOffset = maxParentOffset-currParent->postNumber;
			}		
		}			
	}
	//PrintLevelOrderTraversal();

}

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
//	cout<<"Tree";
	size = n;
	int MAX_NODES = n;
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

void Tree::PrintLevelOrderTraversal(){
	for(int i=0;i<Proxy.size();i++){
		cout<<"Level "<<i+1<<" : ";
		vector<TreeNode*> curr = Proxy[i];
		for(int j=0;j<curr.size();j++){
			cout<<curr[j]->data<<":"<<curr[j]->inDegree;
			//cout<<":("<<curr[j]->preNumber<<","<<curr[j]->postNumber<<")";
			//cout<<":("<<curr[j]->minShiftOffset<<","<<curr[j]->maxShiftOffset<<") ";
			cout<<":("<<curr[j]->minParentOffset<<","<<curr[j]->maxParentOffset<<") ";
		}	
		cout<<endl;
	}

}

void Tree::CreateN_aryDAG(int n)
{
//	cout<<"DAG";
	size = n;
	int MAX_NODES = n;
	leafNodes = (size/2)-1;
	//create all tree nodes at once and store their addresses in "Array"	
	for(int i = 1; i<=MAX_NODES;i++)
	{
		TreeNode *temp=new TreeNode(i);
		Array[temp->data] = temp;
	}
	head = Array[1];
	vector<TreeNode*> level;
	level.push_back(Array[1]);
	Proxy.push_back(level);
	level.clear();
	int ptr = 2;
	int power = 1;
	int listSize = pow(N_ary, power);
	
	//create links according to N_ary
	for(int i=1;(i<=MAX_NODES && ptr<=MAX_NODES);i++)
	{
		for(int j=0; (j<N_ary && ptr<=MAX_NODES); j++)
		{
			(Array[i]->neighbour).push_back(Array[ptr]);
			(Array[ptr]->parents).push_back(Array[i]);
			if(!listSize){
				Proxy.push_back(level);
				level.clear();			
				listSize = pow(N_ary,++power);			
			}
			level.push_back(Array[ptr]);
			listSize--;
			ptr++;
		}
	}
	if(level.size())
		Proxy.push_back(level);
	
	//PrintLevelOrderTraversal();
	
	for(int i=1;i<=MAX_NODES;i++)
	{
		if(Array[i]->neighbour.size() ==  0)
			LeafList.push_back(Array[i]);
	}	
	leafNodes = LeafList.size();

/*	cout<<"Before shuffle:"<<endl;
	for(int i=1;i<=MAX_NODES;i++){
		cout<<Array[i]->data<<" : ";
		for(int j=0;j<Array[i]->neighbour.size();j++){
			cout<<Array[i]->neighbour[j]->data<<" ";		
		}
		cout<<endl;
	}
	cout<<"After shuffle:"<<endl;*/
	ShuffleEdges(MAX_NODES);
	ConnectDisconnectedNodes();
	//PrintLevelOrderTraversal();
	
}

void Tree::ConnectDisconnectedNodes(){
	int level = Proxy.size()-1;
	vector<TreeNode*> currLevel;
	while(level>0){
		currLevel = Proxy[level];
		for(int i=0;i<currLevel.size();i++){
			if(currLevel[i]->inDegree==0){
				double parentIndex = i/N_ary;
				if(!parentIndex){
					parentIndex = 1;
				}
				else if(parentIndex==(Proxy[level-1].size()-1)){
					parentIndex = Proxy[level-1].size()-2;
				}
				else{
					if(rand()%2)
						parentIndex++;
					else
						parentIndex--;							
				}   	
				Proxy[level-1][parentIndex]->neighbour.push_back(currLevel[i]);
				currLevel[i]->parents.push_back(Proxy[level-1][parentIndex]);
				currLevel[i]->inDegree++;		
			}		
		}
		level--;
	}
}

void Tree::ShuffleEdges(int MAX_NODES){
	//cout<<"Complete";
	int level = 1;
	int currPos = 0;
	for(int i=2;i<=MAX_NODES;i++){
		
		if(Array[i]->neighbour.size()!=0){
			int NoOfChildren = Array[i]->neighbour.size();
			vector<TreeNode*> tmp;			
			for(int j=0;j<NoOfChildren;j++){
				if(rand()%10>=shuffleProb){
					int childStartPos = N_ary*currPos;
					int flg = 0;
					int nextChildPos= childStartPos;
					Proxy[level+1][childStartPos+j]->inDegree--;
					vector<TreeNode*> parentList = Proxy[level+1][childStartPos+j]->parents;
					for(int k=0;k<parentList.size();k++){
						if(parentList[k]->data == Array[i]->data){
							parentList.erase(parentList.begin()+k);
						}
					}
					//Check this later		
					Proxy[level+1][childStartPos+j]->parents.clear();
					Proxy[level+1][childStartPos+j]->parents = parentList;			
										
					while(!flg){
						//cout<<flg<<endl;
						nextChildPos= childStartPos;
						if(rand()%2){
							nextChildPos+=(N_ary+rand()%radius);
							if(nextChildPos<Proxy[level+1].size()){
								tmp.push_back(Proxy[level+1][nextChildPos]);
								Proxy[level+1][nextChildPos]->inDegree++;
								Proxy[level+1][nextChildPos]->parents.push_back(Array[i]);
								flg = 1;							
							}
	//						cout<<nextChildPos<<endl;
						}else{
							nextChildPos-=(rand()%radius+1);
							if(nextChildPos>=0){
								tmp.push_back(Proxy[level+1][nextChildPos]);
								Proxy[level+1][nextChildPos]->inDegree++;
								Proxy[level+1][nextChildPos]->parents.push_back(Array[i]);
								flg = 1;							
							}
	//						cout<<nextChildPos<<endl;
						}					
					}			
				}else
					tmp.push_back(Array[i]->neighbour[j]);
			}
			Array[i]->neighbour.clear();
			Array[i]->neighbour = tmp;
		}
		currPos++;
		if(currPos==pow(N_ary,level)){
			currPos=0;
			level++;
		}
	}

}

void Tree::Check_DFS(TreeNode* ptr, vector<int> &checklist){
	checklist[ptr->data-1] = 1;
	for(int i=0;i<ptr->neighbour.size();i++){
		Check_DFS(ptr->neighbour[i], checklist);	
	}
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
	srand(0);

	TreeNode* NodeArray[NoOfRequestedNode];
	set<TreeNode*> nodeSet;
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
			nodeSet.insert(NodeArray[i]);
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

		double requiredNodesCnt = 0;		
		double greyCnt = 0;
		double lockPreNumber = ptr->preNumber;
		double lockPostNumber = ptr->postNumber;
		vector<int> check(size, 0);
		set<TreeNode*> :: iterator itr;		
		for(itr=nodeSet.begin();itr!=nodeSet.end();itr++){
			Check_DFS(*itr, check);
		}
		for(int s=0;s<size;s++){
			if(check[s])
				requiredNodesCnt++;		
		}				
		for(int k=1;k<=size;k++){
			TreeNode* currPtr = Array[k];
			if(currPtr->preNumber >= lockPreNumber && currPtr->postNumber <= lockPostNumber)
				greyCnt++;	
		}		
		currDegOfConc = requiredNodesCnt/greyCnt;
		
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
	else if( caseParameter == 11)
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
			nodeSet.insert(NodeArray[i]);
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
		double requiredNodesCnt = 0;		
		double greyCnt = 0;
		double lockPreNumber = ptr->preNumber + ptr->minParentOffset;
		double lockPostNumber = ptr->postNumber + ptr->maxParentOffset;
		vector<int> check(size, 0);
		set<TreeNode*> :: iterator itr;		
		for(itr=nodeSet.begin();itr!=nodeSet.end();itr++){
			Check_DFS(*itr, check);
		}
		for(int s=0;s<size;s++){
			if(check[s])
				requiredNodesCnt++;		
		}				
		for(int k=1;k<=size;k++){
			TreeNode* currPtr = Array[k];
			if((currPtr->preNumber+currPtr->minShiftOffset) >= lockPreNumber && (currPtr->postNumber+currPtr->maxShiftOffset) <= lockPostNumber)
				greyCnt++;	
		}		
		currDegOfConc = requiredNodesCnt/greyCnt;

		interval *parent = new interval(ptr->preNumber + ptr->minParentOffset, ptr->postNumber + ptr->maxParentOffset, accessType);
		interval *shift = new interval(ptr->preNumber + ptr->minShiftOffset, ptr->postNumber + ptr->maxShiftOffset, accessType);
x11y11:	if(!ICheck.IsOverlapModified(shift, parent, 1, threadID))
	{	
		pthread_rwlock_wrlock(&ptr->rwlock);

		//Call dummay null operation
		NullOperation();

		ICheck.Delete(threadID);
		pthread_rwlock_unlock(&ptr->rwlock);

	}
	else { usleep(10);goto x11y11;}
	}
	
}//end of DummyTask()













