#include <cstring>
#include <stdio.h>
#include <..\..\..\tools\dsgmaker\InstaEntityDSGList.hpp>
#include <tlInstaEntityDSGChunk.hpp>


const int MAX_INSTA_DSG = 50;

InstaEntityDSGList::InstaEntityDSGList ()
{
    mindex =0;
    mp_list = new tlInstaEntityDSGChunk* [MAX_INSTA_DSG];
    mp_list[0]=NULL;
}

InstaEntityDSGList::~InstaEntityDSGList ()
{
    delete [] mp_list;
}

//input DSG name and it should return a pointer to the dsg or null if failure
tlInstaEntityDSGChunk* InstaEntityDSGList::GetInstaEntity(const char * name)
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

// add a InstaEntitychunk  to the list
int InstaEntityDSGList::AddInstaEntity (tlInstaEntityDSGChunk* p_insta)
{
    if(mindex < MAX_INSTA_DSG -1)
    {
        mp_list[mindex++]=p_insta;
        return 0;
    }
    else
    {
        printf("ERROR: InstaEntityDSGList if FULL! Unable to add entery! \n");
        return 1;
    }

}
