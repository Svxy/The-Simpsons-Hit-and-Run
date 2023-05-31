#include <cstring>
#include <stdio.h>
#include <..\..\..\tools\dsgmaker\DSGList.hpp>
#include <tlDataChunk.hpp>
#include <..\constants\srrchunks.h>


const int MAX_DSG = 100;

DSGList::DSGList ()
{
    mbNoInstanceChunks = false;
    mindex =0;
    mp_list = new tlDataChunk* [MAX_DSG];
    mp_list[0]=NULL;
}

DSGList::~DSGList ()
{
    delete [] mp_list;
}

//input DynaDSG name and it should return a pointer to the dsg or null if failure
tlDataChunk* DSGList::GetDSG(const char * name)
{
    
    //search the array for the name.
    for(unsigned int i=0;i<mindex;i++)
    {
        //printf("    Searching for %s, against %s\n",name,mp_list[i]->GetName());
        //if we find it return pointer otherwise it will return null by default 
        if (strcmp ( mp_list[i]->GetName(),name)==0)
         {
             return( mp_list[i] );
         }
    }
    return NULL;

}

// add a dynaphyschunk  to the list
int DSGList::AddDSG (tlDataChunk* p_dsg)
{
    if(mindex < MAX_DSG -1)
    {
        mp_list[mindex++]=p_dsg;
        return 0;
    }
    else
    {
        printf("    ERROR: ====> DSGList if FULL! Unable to add entery! \n");
        return 1;
    }

}


//returns the number of enteries in list.
unsigned int DSGList::GetIndexCount()
{
    return mindex;
}
  
//input index and it should return a pointer to the dsg or null if failure
tlDataChunk* DSGList::GetDSGByIndex(unsigned int i)
{
    
    //search the array for the name.
    if ( i >=0 && i <mindex)
    {
        return( mp_list[i] );
    }
    else
    {
        return NULL;
    }
}


//resorts the DSGchunks so according the the priority set in the toollib_extras.hpp file
void DSGList::ReSortDSGChunks(void)
{
    printf ("Resorting DSG Chunks\n");
    //sort the DSGchunks internally so InstanceChunks are last

        for( unsigned int i=0;i<GetIndexCount();i++)
        {          

            tlDataChunk* p_dsg=NULL;
            p_dsg=GetDSGByIndex(i);
            if (p_dsg == NULL)
            {
                printf("    ERROR: ==== > Sorting DSG! \n"); 
            }
            else
            {
                p_dsg->SortSubChunks();
            }
        }

}

//checks the DSG chunks for missing Chunks that will cause the game loaders to crash.
unsigned int DSGList::VerifyChunks(void)
{
    tlDataChunk* p_datachunk=NULL;
    bool badart = false;
    
    printf("\nVerifying Chunks arent missing components....\n");

    for (unsigned int i=0; i<mindex;i++)
    {
        p_datachunk=mp_list[i];
        switch(p_datachunk->ID())
        {
        case SRR2::ChunkID::DYNA_PHYS_DSG:
            {
                tlDataChunk* p_subchunk=NULL;
                int subcount=p_datachunk->SubChunkCount();

                if (subcount<5)
                {
                    bool meshchunk =false;
                    bool physicschunk=false;
                    bool collisionchunk= false;
                    bool otc = false;
                    bool instancechunk = false;

                    
                    for( int j=0;j<subcount;j++)
                    {
                        p_subchunk=p_datachunk->GetSubChunk(j);
                        
                        switch(p_subchunk->ID())
                        {
                            case Pure3D::Mesh::MESH:
                                {
                                    meshchunk = true;
                                    break;
                                }
                            case Simulation::Physics::OBJECT:
                                {
                                    physicschunk = true;
                                    break;
                                }
                            case SRR2::ChunkID::OBJECT_ATTRIBUTES:
                                {
                                    otc = true;
                                    break;
                                }
                            case Simulation::Collision::OBJECT:
                                {
                                    collisionchunk = true ;
                                    break;
                                }
                            case SRR2::ChunkID::INSTANCES:
                                {
                                    instancechunk= true;
                                    break;
                                }
                            default:
                                {
                                    printf(" Unexpected ChunkType %d \n",p_subchunk->ID() );
                                    break;
                                }
                        }//end of switch for sub chunk check
                    }//end of for loop to subchunk iteration

                    if (meshchunk != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s DynaDSGChunk is missing MeshChunk\n",p_datachunk->GetName());                      
                    }

                    if (physicschunk != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s DynaDSGChunk is missing PhysicsChunk\n",p_datachunk->GetName());
                    }

                    if(otc != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s DynaDSGChunk is missing Object Attribute Chunk\n",p_datachunk->GetName());
                        
                    }

                    if(collisionchunk != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s DynaDSGChunk is missing BoundingVolume\n",p_datachunk->GetName());
                       
                    }

                    if( instancechunk != true)
                    {
                         if (mbNoInstanceChunks == true)
                        {

                        }
                        else
                        {
                            badart =true;
                            DSGList::PrintErrors();
                            printf ("       ERROR:=====> %-30s DynaPhysDSGChunk is missing Instanced Chunk\n",p_datachunk->GetName());
                        }
                    }
                }//end if
                break;
            }//end case
        case SRR2::ChunkID::INSTA_STATIC_PHYS_DSG:
            {
                tlDataChunk* p_subchunk=NULL;
                int subcount=p_datachunk->SubChunkCount();

                if (subcount<4)
                {
                    bool meshchunk =false;
                    bool collisionchunk= false;
                    bool otc = false;
                    bool instancechunk = false;

                    for( int j=0;j<subcount;j++)
                    {
                        p_subchunk=p_datachunk->GetSubChunk(j);
                        
                        switch(p_subchunk->ID())
                        {
                            case Pure3D::Mesh::MESH:
                                {
                                    meshchunk = true;
                                    break;
                                }
                            case SRR2::ChunkID::OBJECT_ATTRIBUTES:
                                {
                                    otc = true;
                                    break;
                                }
                            case Simulation::Collision::OBJECT:
                                {
                                    collisionchunk = true ;
                                    break;
                                }
                            case SRR2::ChunkID::INSTANCES:
                                {
                                    instancechunk= true;
                                    break;
                                }
                            default:
                                {
                                    printf(" Unexpected ChunkType %d \n",p_subchunk->ID() );
                                    break;
                                }
                        }//end of switch for sub chunk check
                    }//end of for loop to subchunk iteration

                    if (meshchunk != true)
                    {   
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s InstaStaticPhysDSGChunk is missing MeshChunk\n",p_datachunk->GetName());
                    }
                    
                    if(otc != true)
                    {   
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s InstaStaticPhysDSGChunk is missing Object Attribute Chunk\n",p_datachunk->GetName());
                        
                    }
                    
                    if(collisionchunk != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s InstaStaticPhysDSGChunk is missing Bounding Volume\n",p_datachunk->GetName());
                        
                    }
                    
                    if( instancechunk != true)
                    {
                        if (mbNoInstanceChunks == true)
                        {

                        }
                        else
                        {
                            badart =true;
                            DSGList::PrintErrors();
                            printf ("       ERROR:=====> %-30s InstaStaticPhysDSGChunk is missing Instanced Chunk\n",p_datachunk->GetName());
                        }
                    }
                }//end if
                break;
            }//end of InstaStatPhyscase

        case SRR2::ChunkID::INSTA_ENTITY_DSG:
            {
                tlDataChunk* p_subchunk=NULL;
                int subcount=p_datachunk->SubChunkCount();

                if (subcount<2)
                {
                    bool meshchunk =false;
                    bool instancechunk = false;
                   

                    for( int j=0;j<subcount;j++)
                    {
                        p_subchunk=p_datachunk->GetSubChunk(j);
                        
                        switch(p_subchunk->ID())
                        {
                            case Pure3D::Mesh::MESH:
                                {
                                    meshchunk = true;
                                    break;
                                }                            
                           case SRR2::ChunkID::INSTANCES:
                                {
                                    instancechunk= true;
                                    break;
                                }
                            default:
                                {
                                    printf(" Unexpected ChunkType %d \n",p_subchunk->ID() );
                                    break;
                                }
                        }//end of switch for sub chunk check
                    }//end of for loop to subchunk iteration

                    if (meshchunk != true)
                    {
                        badart =true;
                        DSGList::PrintErrors();
                        printf ("       ERROR:=====> %-30s InstaEntityDSGChunk is missing MeshChunk\n",p_datachunk->GetName());
                    }                   
                    
                    if( instancechunk != true)
                    {
                         if (mbNoInstanceChunks == true)
                        {

                        }
                        else
                        {
                            badart =true;
                            DSGList::PrintErrors();
                            printf ("       ERROR:=====> %-30s InstaStaticPhysDSGChunk is missing Instanced Chunk\n",p_datachunk->GetName());
                        }
                    }
                }//end if
                break;
            }//end case

        }//end switch

    }//end for
if (badart == true)
{
    return 1;
}

return 0;

}//end of VerifyChunks method


// formatted IO Header for errors.
void DSGList::PrintErrors(void)
{
    printf("\n");
    printf("===============================================================================================\n");
    printf("\n");
}







        


