#include <cstring>
#include <stdio.h>
#include <..\..\..\tools\dsgmaker\DynaDSGList.hpp>
#include <tlDynaPhysDSGChunk.hpp>


const int MAX_DYNA_DSG = 50;

DynaPhysDSGList::DynaPhysDSGList ()
{
    mindex =0;
    mp_list = new tlDynaPhysDSGChunk* [MAX_DYNA_DSG];
    mp_list[0]=NULL;
}

DynaPhysDSGList::~DynaPhysDSGList ()
{
    delete [] mp_list;
}

//input DynaDSG name and it should return a pointer to the dsg or null if failure
tlDynaPhysDSGChunk* DynaPhysDSGList::GetDyna(const char * name)
{
    
    //search the array for the name.
    for(unsigned int i=0;i<mindex;i++)
    {
        printf("    Searching for %s, against %s\n",name,mp_list[i]->GetName());
        //if we find it return pointer otherwise it will return null by default 
        if (strcmp ( mp_list[i]->GetName(),name)==0)
         {
             return( mp_list[i] );
         }
    }
    return NULL;

}

// add a dynaphyschunk  to the list
int DynaPhysDSGList::AddDyna (tlDynaPhysDSGChunk* p_dyna)
{
    if(mindex < MAX_DYNA_DSG -1)
    {
        mp_list[mindex++]=p_dyna;
        return 0;
    }
    else
    {
        printf("ERROR: DynaDSGList if FULL! Unable to add entery! \n");
        return 1;
    }

}
