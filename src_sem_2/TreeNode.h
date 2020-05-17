#ifndef TREENODE_H
#define TREENODE_H
#include <pthread.h>
#include <vector>
#include "interval.h"
using namespace std;

class TreeNode {
public:	
	bool IsExplored;
	bool active;
	bool parentUpdated;
	int data;
	pthread_mutex_t Lock;
	pthread_rwlock_t rwlock;
	int IsIx;
	int refCounter;
	double preNumber;
	double postNumber;
	double minShiftOffset;
	double maxShiftOffset;
	double minParentOffset;
	double maxParentOffset;
	double sizeOfSubtree;
	double degOfConc;	
	double inDegree;
	vector<TreeNode*> immediateParents;
	vector <TreeNode*> neighbour;
	class TreeNode *right;
	class TreeNode *left;
	class TreeNode *parent;
	vector <TreeNode* >parents;
	vector< pair<double,double> > multiInterval;
	TreeNode(int);
};

TreeNode::TreeNode(int key)
{
  	int rc = pthread_rwlock_init(&rwlock, NULL);  
	IsExplored = false;
	active = false;
	parentUpdated = false;
	sizeOfSubtree = 0;
	degOfConc = 0.0;
	inDegree = 1;	
	minShiftOffset = 0;
	maxShiftOffset = 0;
	minParentOffset = 0;
	maxParentOffset = 0;
	data=key;
  	IsIx=0;
  	refCounter=0;
}
#endif
