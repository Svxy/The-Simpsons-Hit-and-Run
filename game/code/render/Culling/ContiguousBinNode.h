#ifndef __CONTIGUOUS_BIN_NODE_H___
#define __CONTIGUOUS_BIN_NODE_H___

#include <raddebug.hpp>


//////////////////////////////////////////////
// These classes are defined in one file pair
// because they're very tightly bound
//////////////////////////////////////////////

template <class T> class CBinNodeL;
template <class T> class CBinNodeR;

template <class T> class ContiguousBinNode
{
public:
   ContiguousBinNode();
   ContiguousBinNode( T& irData );
   ~ContiguousBinNode();

   ContiguousBinNode<T>* LChild();
   ContiguousBinNode<T>* RChild();
   
   int LChildOffset();
   int RChildOffset();

   //void LinkRChild( int iRChild );

   int  GetSubTreeSize(); //SubTreeSize is the number of nodes in the subtree not including the node called
   void SetSubTreeSize( int iSubTreeSize ); 

   bool IsRoot();

   ContiguousBinNode<T>*   Parent();
   void                    LinkParent( int iParentOffset );
   
   //Previously Asymetric
   ContiguousBinNode<T>* LSibling();
   ContiguousBinNode<T>* RSibling();
   int                   RSiblingOffset();


   enum
   {
      //msNoChildren = -1, 
      msNoChildren = 0, 

      msNoParent  = 0
   };

   T mData;
protected:
   int mSubTreeSize;
   int mParentOffset;
   
};

/*
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T> class CBinNodeL : public ContiguousBinNode<T>
{
public:
   //Constuction
   //Navigation Accessors
   virtual CBinNodeR<T>*   RSibling();
   virtual int             RSiblingOffset();
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T> class CBinNodeR : public ContiguousBinNode<T>
{
public:
   //Constuction
   //Navigation Accessors
   virtual CBinNodeL<T>*         LSibling();
};
*/








///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
#ifndef NULL
#define NULL 0
#endif
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>::ContiguousBinNode()
{
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>::ContiguousBinNode( T& irData )
:
   mData(irData)
{
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>::~ContiguousBinNode()
{
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>* ContiguousBinNode<T>::LChild()
{
   rAssert( mSubTreeSize != msNoChildren );
   return (this+1);

   /*
   if( mSubTreeSize != msNoChildren )
      return (this+1);
   else
      return NULL;
      */
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>* ContiguousBinNode<T>::RChild()
{
   rAssert( mSubTreeSize != msNoChildren );
   return (this+1)->RSibling();
/*
   if( mSubTreeSize != msNoChildren )
      return (this+1)->RSibling();
   else
      return NULL;
*/
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
int ContiguousBinNode<T>::LChildOffset()
{
   rAssert( mSubTreeSize != msNoChildren );
   return 1;
/*
   if( mSubTreeSize != msNoChildren )
      return (1);
   else
      return 0;
*/
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
int ContiguousBinNode<T>::RChildOffset()
{
   rAssert( mSubTreeSize != msNoChildren );
   return LChild()->RSiblingOffset() + 1;
/*
   if( mSubTreeSize != msNoChildren )
   //   return ((CBinNodeL<T>*)(this+1))->RSiblingOffset() + 1;
      return LChild()->RSiblingOffset() + 1;
   else
      return 0;
*/
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
bool ContiguousBinNode<T>::IsRoot()
{
   if( mParentOffset == msNoParent )
      return true;
   else
      return false;
}


///////////////////////////////////////////////////////////////
//SubTreeSize is the number of nodes in the subtree not including the node called
///////////////////////////////////////////////////////////////
template <class T>
int ContiguousBinNode<T>::GetSubTreeSize()
{
   return mSubTreeSize;
}

///////////////////////////////////////////////////////////////
//SubTreeSize is the number of nodes in the subtree not including the node called
///////////////////////////////////////////////////////////////
template <class T>
void ContiguousBinNode<T>::SetSubTreeSize( int iSubTreeSize )
{
   mSubTreeSize = iSubTreeSize;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
void ContiguousBinNode<T>::LinkParent( int iParentOffset )
{
   mParentOffset = iParentOffset;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>* ContiguousBinNode<T>::Parent()
{
   return (this+mParentOffset);
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>* ContiguousBinNode<T>::LSibling()
{
   return (this+mParentOffset)->LChild();
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
ContiguousBinNode<T>* ContiguousBinNode<T>::RSibling()
{
   //
   // Even unbalanced binary trees should have siblings
   // Though there are exceptions, I'm not dealing with them right now
   //
   return (this+mSubTreeSize+1);
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
int ContiguousBinNode<T>::RSiblingOffset()
{
   //
   // Even unbalanced binary trees should have siblings
   // Though there are exceptions, I'm not dealing with them right now
   //
   return mSubTreeSize+1;
}

/*
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
CBinNodeR<T>* ContiguousBinNode<T>::RSibling()
{
   rAssert(false);
   return NULL;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
int ContiguousBinNode<T>::RSiblingOffset()
{
   rAssert(false);
   return 0;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
CBinNodeL<T>* ContiguousBinNode<T>::LSibling()
{
   rAssert(false);
   return NULL;
}

////////-------------CBinNodeR-------------////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
CBinNodeL<T>* CBinNodeR<T>::LSibling()
{
   return (this+mParentOffset)->LChild();
}


////////-------------CBinNodeL-------------////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
CBinNodeR<T>* CBinNodeL<T>::RSibling()
{
   //
   // Even unbalanced binary trees should have siblings
   // Though there are exceptions, I'm not dealing with them right now
   //
   return (CBinNodeR<T>*)(this+mSubTreeSize);
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
template <class T>
int CBinNodeL<T>::RSiblingOffset()
{
   //
   // Even unbalanced binary trees should have siblings
   // Though there are exceptions, I'm not dealing with them right now
   //
   return mSubTreeSize;
}

*/
#endif