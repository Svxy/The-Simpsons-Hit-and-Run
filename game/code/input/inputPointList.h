#if !defined(AFX_LIST_H__F5C121CA_0F5B_4CE3_91D5_B7B24AEB8D37__INCLUDED_)
#define AFX_LIST_H__F5C121CA_0F5B_4CE3_91D5_B7B24AEB8D37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/******************************************************************************
	List

	Date: 19-11-01

	Name: Jeff Giles 
		  Ezeikeil@Hotmail.com

	Notes:	   A Simple, doubly linked list which returns NULL when run off 
			either end.

		 
******************************************************************************/
template <class type>	 
class List  
{	
	//*Node struct*******************************************************************
	//		Notes:		The workhorse of the Linked List
	//*****************************************************************************
	struct Node
	{
		type * thedata;			//pointer to the data
		Node * nextnode;		//pointer to the next node in the list
		Node * lastnode;		//pointer to the previous node in the list

		//Ctors
		Node():	thedata(0),nextnode(0),lastnode(0) {};
		Node(type* data, Node* next):	thedata(data),nextnode(next),lastnode(0) {};
	};

public:
	//*Ctor/Dtor*****************************************************************
	List():		myHead(0),myTail(0), numNodes(0) {};
	virtual ~List(){};

	//*GetSize*********************************************************************
	//		Notes:	returns the current number of nodes
	//*****************************************************************************
	int GetSize()	
	{
		return numNodes;
	}
	
	//*PushFront*******************************************************************
	//		Notes:		Pushes an element onto the top of the list
	//*****************************************************************************
	void PushFront(type * data)
	{
		myHead = new Node(data, myHead);

		if (myTail == NULL)
			myTail = myHead;
		
		if(myHead->nextnode !=NULL)					//redirect the node following myHead 
			myHead->nextnode->lastnode = myHead;	//to point to the new head
		
		numNodes++;
	}

	//*PushBack******************************************************************
	//		Notes:		Pushes an element onto the bottom of the list
	//***************************************************************************
	void PushBack(type * data)
	{		
		if (myHead == NULL) //then the list must be empty
			PushFront(data);
		else
		{	
			Node * temp = new Node;
			temp->thedata = data;
			temp->lastnode = myTail;
				
			myTail= temp;						//redirect myTail
			myTail->lastnode->nextnode = temp;	//redirect the nextnode before 
												//myTail to point to the new Node
			numNodes++;
		}				
	}

	//*PopBack*********************************************************************
	//		Notes:		Removes an element from the top of the list
	//*****************************************************************************
	type * PopBack()
	{
		if(myTail != NULL)
		{
			type * temp = myTail->thedata;
			Node * oldtail = myTail;		//keep a hold of the data to prevent memory leak
			myTail = myTail->lastnode;		
			numNodes--;
			delete oldtail;
			return temp;
		}
		else
			return NULL;
	};

	//*PopFront*******************************************************************
	//		Notes:		Removes an element from the bottom of the list
	//****************************************************************************
	type * PopFront()
	{
	
		if(myHead != NULL)
		{
			type * temp = myHead->thedata;
			Node * oldHead = myHead;		//keep a hold of the data to prevent memory leak
			myHead = myHead->nextnode;		
			numNodes--;
			delete oldHead;
			return temp;
		}
		else
			return NULL;

	};

	//*PeekFront******************************************************************
	//		Notes:		Accesses the top of the list without removing the node
	//****************************************************************************
	type * PeekFront()
	{
		if(myHead)
			return myHead->thedata;
		else
			return NULL;
	};

	//*PeekBack*******************************************************************
	//		Notes:		Accesses the bottom of the list without removing the node
	//****************************************************************************
	type * PeekBack()
	{
		if(myTail)
			return myTail->thedata;
		else
			return NULL;
	};

	//*operator []****************************************************************
	//		Notes:		Accesses the any node of the list without removing it
	//					Position is from top to 
	//****************************************************************************
	type * operator[] (int index)
	{
		if(numNodes < 0 || index >= numNodes)	//prevent overrun
			return NULL;		
		else
		{
			Node * temp = myHead;
			int counter = 0;

			while (counter < index)
			{
				temp = temp->nextnode;		//point temp at the next node in the list
				counter++;
			}
			return temp->thedata;
		}
	};

private:
	Node * myTail;
	Node * myHead;
	int numNodes;
};

#endif // !defined(AFX_LIST_H__F5C121CA_0F5B_4CE3_91D5_B7B24AEB8D37__INCLUDED_)
