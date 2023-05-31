#include "badlist.hpp"
#include <cstring>
#include<stdio.h>

badlist::badlist ()
{
    max_items=200;
    data_array=new entity_data[max_items];
	index = 0;
    totalmemoryusage =0;
    for(unsigned int i=0;i<max_items;i++)
    {             
        strcpy(data_array[i].name," ");
        data_array[i].occurances=0;
        data_array[i].size=0;
    }
}
badlist::badlist (const unsigned int arraysize)
{
    max_items=arraysize;
    data_array=new entity_data[max_items];
	index = 0;
    totalmemoryusage =0;
    for(unsigned int i=0;i<max_items;i++)
    {             
        strcpy(data_array[i].name," ");
        data_array[i].occurances=0;
        data_array[i].size=0;
    }
}




badlist::~badlist ()
{
}

int badlist::add(const char* name,const unsigned int occurances,const unsigned int size)
{
	if (index<max_items-1)
	{
		strcpy( data_array[index].name,name);
        data_array[index].occurances=occurances;
        data_array[index].size=size;
        totalmemoryusage+=size;
		index++;
		return 0;
	}
	else
	{
		return 1;
	}
}
	
int badlist::inlist(const char* name)
{
    unsigned int i=0;
    do 
    {
        if(strcmp(name,data_array[i].name) ==0)
        {
            //printf("%s found in list \n",name);
            return 1;
        }
        i++;
    } while (i<index);

    return 0;
}

int badlist::getsize()
{   	
   return totalmemoryusage;		
}

int badlist::getindexcount()
{
    return index;
}


int badlist::printverbose()
{
    for (unsigned int i=0;i<index;i++)
    {
        printf("%-30s: size in bytes: %6i \n",data_array[i].name,data_array[i].size);
    }
    return 0;
}