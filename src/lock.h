#ifndef Lock_H
#define Lock_H
#include"TreeNode.h"
#include"intervalTree.h"



ITNode *root = NULL;
pthread_mutex_t IntervalTreeLock;

void Lock(pthread_mutex_t *rootLock)
{
  pthread_mutex_lock(rootLock);
}

void UnLock(pthread_mutex_t *rootLock)
{
  pthread_mutex_unlock(rootLock);
}

void ISLock(TreeNode *node)
{
check : while(node->IsIx == 4);
  
  Lock(&node->Lock);
  if(node->IsIx != 4)
  {
    pthread_rwlock_rdlock(& node->rwlock);
    if(node->IsIx < 1)
      node->IsIx=1;
    node->refCounter++;
  }
  else
  {
    UnLock(&node->Lock);
    goto check;
  }
    
  UnLock(&node->Lock);
  
  
}


void IXLock(TreeNode *node)
{
check : while(node->IsIx >2);
  
  Lock(&node->Lock);
  if(node->IsIx <= 2)
  {
    pthread_rwlock_rdlock(& node->rwlock);
    if(node->IsIx < 2)
      node->IsIx=2;
    node->refCounter++;
  }
  else
  {
    UnLock(&node->Lock);
    goto check;
  }
    
  UnLock(&node->Lock);
  
  
}

void SLock(TreeNode *node)
{
check : while(node->IsIx == 2 || node-> IsIx == 4);
  
  Lock(&node->Lock);
  if(node->IsIx != 2 && node->IsIx != 4)
  {
    pthread_rwlock_rdlock(& node->rwlock);
    if(node->IsIx < 3)
      node->IsIx=3;
    node->refCounter++;
  }
  else
  {
    UnLock(&node->Lock);
    goto check;
  }
    
  UnLock(&node->Lock);
}

void XLock(TreeNode *node)
{
check : while(node->refCounter != 0);
  
  Lock(&node->Lock);
  if(node->refCounter == 0)
  {
    pthread_rwlock_wrlock(& node->rwlock);
    node->IsIx=4;
    node->refCounter++;
  }
  else
  {
    UnLock(&node->Lock);
    goto check;
  }
    
  UnLock(&node->Lock);
  
  
}


void IUnLock(TreeNode *node)
{
  Lock(&node->Lock);
  pthread_rwlock_unlock(&node->rwlock);
  node->refCounter--;
  if( node->refCounter == 0)
    node->IsIx = 0;

  UnLock(&node->Lock);
  
}

void RdLock(TreeNode *node)
{
  Interval l = {node->preNumber , node->postNumber};
 // printf("entered in lock \n");
  checkLabel : while(intervalSearch(root, l, 0) != NULL);
  
  Lock(& IntervalTreeLock);
 // printf(" locked \n");
  if(intervalSearch(root, l, 0) == NULL)
    {
      insert(&root , l, 0);
      
      pthread_rwlock_rdlock(& node->rwlock);
    //  cout<<"good";
    }
  else
  {
    UnLock(& IntervalTreeLock);
   // printf(" unlocked goto \n");
    goto checkLabel;
  }
  UnLock(& IntervalTreeLock);
  //printf(" unlocked \n");
}

void WrLock(TreeNode *node)
{
 
   Interval l = {node->preNumber , node->postNumber};
 // printf("entered in lock \n");
  checkLabel : while(intervalSearch(root, l, 1) != NULL);
  
  Lock(& IntervalTreeLock);
  //printf(" locked \n");
  if(intervalSearch(root, l, 1) == NULL)
    {
      insert(&root , l, 1);
      
      pthread_rwlock_wrlock(& node->rwlock);
    //  cout<<"good";
    }
  else
  {
    UnLock(& IntervalTreeLock);
  //  printf(" unlocked goto \n");
    goto checkLabel;
  }
  UnLock(& IntervalTreeLock);
 // printf(" unlocked \n");
  
}

void RdWrUnLock(TreeNode *node)
{ Lock(& IntervalTreeLock);
  deleteTreeNode(&root,node->preNumber);
 
  pthread_rwlock_unlock(&node->rwlock);
   UnLock(& IntervalTreeLock);
}
















#endif
