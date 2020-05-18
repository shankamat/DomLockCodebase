#ifndef intervalTree_H
#define intervalTree_H

#include <iostream>

using namespace std;

using namespace std;

 

struct Interval

{

    double low, high;

};

struct ITNode

{

    Interval *i; 

    int max;
    int mode;
    int count;

    ITNode *left, *right;

};

ITNode * newNode(Interval i)

{

    ITNode *temp = new ITNode;

    temp->i = new Interval(i);

    temp->max = i.high;

    temp->left = temp->right = NULL;

};

void insert(ITNode **root, Interval i, int mode)

{
 ITNode *temp = new ITNode;
    if ((*root) == NULL)

    {
       

    temp->i = new Interval(i);

    temp->max = i.high;
    temp->count = 1;
    temp->mode = mode;
    temp->left = temp->right = NULL;
	(*root) = temp;
       // return newNode(i);
	return;

    }

    double l = (*root)->i->low;

    if (i.low < l)

    {

//        (*root)->left = 
	insert(&((*root)->left), i,mode);

    } 

    else if (i.low > l)

    {

  //      (*root)->right = 
	insert(&((*root)->right), i, mode);

    } 
    else //node exists
    {
      if((*root)->count != 0 && (*root)->mode ==0 && mode == 0) // shared node
      {
	(*root)->count ++;
        // printf("count now %d",(*root)->count);
      }
      else if((*root)->count == 0)// deleted node again re-activating 
      {
	(*root)->count ++;
	(*root)->mode = mode;
      }
    }

    if ((*root)->max < i.high)

    {

        (*root)->max = i.high;

    }

    //return root;
	return;

}

bool doOVerlap(Interval i1, Interval i2)

{

    if (i1.low <= i2.high && i2.low <= i1.high)

    {

        return true;

    }

    return false;

}

Interval *intervalSearch(ITNode *root, Interval i, int mode)

{

    if (root == NULL)

    {

        return NULL;

    }

    if (doOVerlap(*(root->i), i) && root->count != 0)

    {
        if(mode == root->mode && mode == 0)
	  return NULL;
	else
        return root->i;

    }

    if (root->left != NULL && root->left->max >= i.low)

    {

        return intervalSearch(root->left, i, mode);

    }

    return intervalSearch(root->right, i, mode);

}


void deleteTreeNode (ITNode **root, int low)
{//  cout<<" Wntered to DELETE\n";
  if((*root)->i->low == low)
  { 
    
   // cout<<" Inside IF";
    (*root)->count --;
    if((*root)->count == 0)
    {
      (*root)->mode = -1;
    }
  }
  else if(low < (*root)->i->low)
  {
    deleteTreeNode(&((*root)->left), low);
  }
  else if(low > (*root)->i->low)
  {
    deleteTreeNode(&((*root)->right), low);
  }
  
  if( (*root)->left != NULL)
  {
    if( (*root)->right != NULL)
    {
      if( (*root)->left->max > (*root)->right->max)
        (*root)->max = (*root)->left->max;
      else
	(*root)->max = (*root)->right->max;
    }
    else
       (*root)->max = (*root)->left->max;
  }
  else if( (*root)->right != NULL)
  {
    (*root)->max = (*root)->right->max;
  }
  //cout<<"deleted\n";
}


void inorder(ITNode *root)

{

    if (root == NULL)

    {

        return;

    }

    inorder(root->left);

 //   cout << "[" << root->i->low << ", " << root->i->high << "]"<< " max = " << root->max << endl;

    inorder(root->right);

}
#endif
