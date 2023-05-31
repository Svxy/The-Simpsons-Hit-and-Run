#ifndef __NodeFLL_H__
#define __NodeFLL_H__
/*
#include <radmath/vector.hpp>

#include <render/DSG/IEntityDSG.h>



class ISortPriority
{
public:
    ISortPriority(){}
    ~ISortPriority(){}

    virtual void  SetRank(rmt::Vector& irRefPosn)=0;
    virtual float Rank()=0;
    virtual void Display()=0;
};


class NodeFLL
{
public:
    NodeFLL(){}
    ~NodeFLL(){}

    void Clear()
    {
        mpNext = NULL;
        mpData = NULL;
    }

    void ClearSetData(IEntityDSG* ipData)
    {
        mpNext = NULL;
        mpData = ipData;
    }
    //
    // Returns Head Node
    //
    NodeFLL* PlaceLowestFirst( NodeFLL* ipNode )
    {
        if(ipNode->mpData->Rank() < mpData->Rank())
        {
            //add prev to this node
            ipNode->mpNext = this;
            return ipNode;
        }

        if(mpNext)
        {
            //add after this node
            mpNext = mpNext->PlaceLowestFirst(ipNode);
            return this;
        }
        else 
        {
            //add to end
            mpNext = ipNode;
            ipNode->mpNext = NULL;
            return this;
        }
    }

    //
    // Returns Head Node
    //
    NodeFLL* PlaceHighestFirst( NodeFLL* ipNode )
    {
        if(ipNode->mpData->Rank() > mpData->Rank())
        {
            //add prev to this node
            ipNode->mpNext = this;
            return ipNode;
        }

        if(mpNext)
        {
            //add after this node
            mpNext = mpNext->PlaceHighestFirst(ipNode);
            return this;
        }
        else 
        {
            //add to end
            mpNext = ipNode;
            ipNode->mpNext = NULL;
            return this;
        }
    }

    IEntityDSG* mpData;
    NodeFLL* mpNext;
protected:

};*/
#endif