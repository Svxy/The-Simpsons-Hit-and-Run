// artchecker.cpp : Defines the entry point for the console application.
// options -strict -add -all 
#pragma warning(disable:4786)


#include <iostream>
#include <cstring>
#include <iomanip>
//#include <mysql++>
#include "artobject.h"
#include "artlimits.h"
#include "badlist.hpp"
#include "outputbuffer.hpp"
#include <toollib.hpp>
#include <tlCollisionObjectChunk.hpp>
#include <tlMeshChunk.hpp>
#include <tlImageChunk.hpp>
#include <tlAnimationChunk.hpp>
#include <tlStatepropChunk.hpp>    
#include "..\..\..\..\game\code\constants\srrchunks.h"

using namespace std;

void debugstring(char * message);

int texturelookup(const texture_data& texturedata, bool add, bool strict,bool showall,badlist& p_badtextures ,bool suppress); 
int shaderlookup(shader_data* lp_shaderdata, bool add, bool strict,bool showall,badlist& badshaders,bool suppress);
//int artobjectlookup(art_object* lp_artobject, Query& query,bool add, bool strict,bool showall,bool suppress);
//int bvobjectlookup(bv_data* lp_bvobject,Query& query,bool add,bool strict,bool showall,bool suppress);

int main(int argc, char* argv[])
{
	
	char* arguements [4];
	char* filename;
	bool strict  =false;
	bool add	=false;
    bool badart=false;
    bool showall =false;
    bool suppress =false;
    bool shin=false;
    char screenoutput [1000];
	int i;

    //lists to track bad textures and shaders.
    badlist bad_textures;
    badlist bad_shaders;

    //list to track meshes using bad shaders and meshes over 500 polys
    outputbuffer* p_mesh_ref=new outputbuffer;
    outputbuffer* p_polycount=new outputbuffer;
    
    
    tlDataChunk::RegisterDefaultChunks();
	
    //processes arguements
	
	//no options 
	if (argc ==1)
	{
		printf("usage:artchecker filename <strict> \n");
		exit (1);
	}

		
	filename= argv[1];
	debugstring(filename);

	
	for (i=2;i<argc;i++)
	{
		arguements[i]=argv[i];

		// user set strict
		if (strcmp(arguements[i],"strict")== 0)
		{
			strict =true;
			debugstring("strict option set");
			break;
		}
						
		//user set add
		
		else if (strcmp (arguements[i],"add")==0)
		{
			add=true;
			debugstring("add option set");
			break;
		}

         else if (strcmp (arguements[i],"sum")==0)
        {
            suppress =true;
            break;
        }

        else if (strcmp (arguements[i],"all")==0)
        {
            showall =true;
            break;
        }
         
        else if (strcmp (arguements[i],"shin")==0)
        {
            shin =true;
            break;
        }
		//illegal option
		else 
		{
			printf("%s is an illegal option \n",arguements[i]);
			exit (1);
		}

	
	} //end for

    //have the pure3d file name

    tlFile input(new tlFileByteStream(filename,omREAD), tlFile::FROMFILE);
    if(!input.IsOpen()) 
    {
        printf("Could not open %s\n", filename);
        exit(-6);
    }

    printf("Processing Filename: %s\n",filename);

	//desend into the file and look at each object until no more objects left
        
    // make a tlDataChunk from the file
    // this is the wrapper chunk for the input
    tlDataChunk* inchunk = new tlDataChunk(&input);
    // we don't need the tlFile anymore
    // this cleans up the tlFile object including the
    // tlFileByteStream
	//printf("Fucking CLose! \n");
    input.Close();
	
	//printf("Closing File proceeding.\n");
    
    //lists to track memory stats for various components.
    badlist texturestats(inchunk->SubChunkCount());
    badlist animationstats(inchunk->SubChunkCount());
    badlist meshstats(inchunk->SubChunkCount());

    // build an output chunk
    tlDataChunk* outchunk = new tlDataChunk;
    int ch;		

    static int counter =0;

	//connect to the DB
	
	
		//setup artrb variables
		bool dbconnect=false;
//		char querybuffer [1000];
//		Row row;
	
	
		

		//creat connection and query
		//Connection con (use_exceptions);
		//Query query=con.query( );
		//dbconnect=con.connect ("srr2test","radit","root","custom47");	
/*
		if (dbconnect == true)
		{
			printf("Connection to Artdb established proceeding.\n");
		}
		else
		{
			printf("Cant connection to art db \n");
			exit (1);
		}
*/
		//printf("Counting chunks !\n");

        // go through all the sub-chunks of the input and
        // process the ones you care about
        for(ch=0; ch < inchunk->SubChunkCount(); ch++)
        {
			//printf("Processing Chunks, #%d \n",ch);
            // create the next sub-chunk
            tlDataChunk* sub = inchunk->GetSubChunk(ch);
            
            // look at the id of the subchunk to decide if we 
            // want to do something with it or not
            switch(sub->ID())
            {                  


            case Pure3D::Texture::TEXTURE:
                {
                    //temp variables
                    tlTexture* p_texture = new tlTexture ();
                    tlImage* p_image2d ;
                    texture_data mytexture;
                    unsigned int texturememsize =0;
                   
					//printf("texuture name: %s \n",p_texture->GetName());

                    int chunkcount1,chunkcount2;

                    for (chunkcount1 =0;chunkcount1<sub->SubChunkCount();chunkcount1++)
                    {
                        tlDataChunk* sub1=sub->GetSubChunk(chunkcount1);

                        switch(sub1->ID())
                        {
                        case Pure3D::Texture::IMAGE:
                            {
                                for (chunkcount2=0;chunkcount2<sub1->SubChunkCount();chunkcount2++)
                                {
                                    tlDataChunk* sub2=sub1->GetSubChunk(chunkcount2);
                                    switch (sub2->ID())
                                    {
                                    case Pure3D::Texture::IMAGE_DATA:
                                        {
                                            tlImageDataChunk* p_imagedatachunk = (tlImageDataChunk*) sub2;
                                            texturememsize += p_imagedatachunk->GetImageDataSize();
                                            //printf("%u bytes \n", p_imagedatachunk->GetImageDataSize());                                                                  
                                        }
                                    }
                                }//end for

                            }//end case
                        }//end switch

                    }//end outer for loop.




                    p_texture->LoadFromChunk(sub);
                                      
                    //cout<<"Checking for "<<p_texture->GetName()<<" Texture ";
                    p_image2d = p_texture->GetImage(0);                                  
    		        
                    //fill in texture struct.
                    strcpy(mytexture.name,p_texture->GetName());
		            mytexture.height=p_texture->GetHeight();
		            mytexture.width=p_texture->GetWidth();
		            mytexture.bpp=p_image2d->GetBpp();
		            mytexture.alpha=p_texture->GetAlphaDepth();      
                    
                    texturestats.add(p_texture->GetName(),1,texturememsize);
     
                    //texture check
		            if (texturelookup (mytexture,add,strict,showall,bad_textures,suppress) ) //
		            {
			            badart= true;
		            }

                  /*     
                    cout<<"Height: "<<p_image2d->GetHeight();
                    cout<<"Width: "<<p_image2d->GetWidth();
                    cout<<"Bpp: "<<mytexture.bpp;
                    cout<<"Alpha: "<<mytexture.alpha;
                    cout<<endl;
                   */
	        
                    delete p_texture;
                    outchunk->AppendSubChunk(sub,FALSE);
                    break;
                }

            case Pure3D::Shader::SHADER:
                {

                    shader_data myshader;
                    tlShader* p_shader =new tlShader ();
                    p_shader->LoadFromChunk(sub);
                    
                    //get data for myshader

                    strcpy(myshader.name,p_shader->GetName());
                    myshader.lit=p_shader->GetIntParam("LIT");
                    myshader.twosid=p_shader->GetIntParam("2SID");
                    
                    //cout<<"Checking for "<<myshader.name<<" Shader ";                 
                                      

                    if (p_shader->GetFloatParam("SHIN")  > 0.0 )
                    {   
                        if (shin)
                        {
                            printf("Shader: %s has shinyness %f greater than 0.0!!!\n",p_shader->GetName(),p_shader->GetFloatParam("SHIN"));
                            //printf("Shader: %s has shinyness %f greater than 0.0!!!, Setting it to 0.0\n",p_shader->GetName(),p_shader->GetFloatParam("SHIN"));
                            //p_shader->SetFloatParam("SHIN",0.0);
                        }
                    }


                    if( strcmp ("lambert",p_shader->GetShaderType() )==0)
                    {
                        printf("     =====> ERROR: %s is of type Lambert, SOMEONE is GETTING the BEATS ! \n",myshader.name);
                        badart=true;
                    }

                    if (p_shader->GetIntParam("2SID") == 0)
                    {
                       //printf("%-30s Shader:   =====>Warning : Double Sided FLAG is 0!\n",p_shader->GetName());
                       //p_shader->SetIntParam("2SID",1);
                       //error=true;
                    }     
                                     
                    if(shaderlookup(&myshader,add,strict,showall,bad_shaders,suppress))
                    {
                        badart=true;
                    }
                  
                    tlDataChunk* p_chunk=p_shader->Chunk();
                    delete p_shader;
                    outchunk->AppendSubChunk(p_chunk);
                    break;
                }
            case SRR2::ChunkID::ENTITY_DSG:
                {
                    tlDataChunk* p_temp = sub->GetSubChunk(0);
                   
                   
                    tlMeshChunk* p_mesh = (tlMeshChunk* )  p_temp;
                    tlPrimGroup* p_primgroup = new tlPrimGroup ();
                    unsigned int vertexmemsize =0;
                    p_primgroup->LoadFromChunk(p_mesh->GetSubChunk(0));                   
                   
                    if( p_primgroup->GetTriangleCount() >MAX_POLYCOUNT)
                    {
                        if(p_primgroup->GetType() == PDDI_PRIM_TRIANGLES)
                        {
                            sprintf(screenoutput,"%-30s mesh => Polycount : %6d \n",p_mesh->GetName(),p_primgroup->GetTriangleCount());
                        }

                        if(p_primgroup->GetType() == PDDI_PRIM_TRISTRIP)
                        {
                            sprintf(screenoutput,"%-30s mesh => Polycount Tristriped: %6d \n",p_mesh->GetName(),p_primgroup->GetTriangleCount());
                        }

                        //cout<< p_primgroup->GetTriangleCount()<<endl;
                        //printf("%-30s mesh => Polycount: %6d \n",p_mesh->GetName(),p_primgroup->GetTriangleCount());
                        p_polycount->add(screenoutput);
                    }

                    if(p_primgroup->GetVertexType()==8209)
                    {
                        //printf("ERROR: %s mesh HAS NO CVB lighting INFO!\n",p_mesh->GetName());
                        sprintf(screenoutput,"%-30s =====>ERROR: Mesh has No CBV (Color by Vertex) Lighting INFO! ", p_mesh->GetName());
                        p_mesh_ref->add(screenoutput);
                        badart=true;
                    }

                    if (bad_shaders.inlist(p_primgroup->GetShader( ) ))
                    {
                        sprintf(screenoutput,"%-30s =====>ERROR: Referencing bad shader : %-30s", p_mesh->GetName(), p_primgroup->GetShader());
                        p_mesh_ref->add(screenoutput);
                        badart =true;
                    }

                    delete p_primgroup;



                    // memory checkfunctionality
                    int chunkcount1,chunkcount2;

                    for (chunkcount1 =0;chunkcount1<p_temp->SubChunkCount();chunkcount1++)
                    {
                        tlDataChunk* sub1=p_temp->GetSubChunk(chunkcount1);

                        switch(sub1->ID())
                        {
                        case Pure3D::Mesh::PRIMGROUP:
                            {
                                for (chunkcount2=0;chunkcount2<sub1->SubChunkCount();chunkcount2++)
                                {
                                    tlDataChunk* sub2=sub1->GetSubChunk(chunkcount2);
                                    switch (sub2->ID())
                                    {
                                    case Pure3D::Mesh::MEMORYIMAGEVERTEXLIST:
                                        {
                                            tlPrimGroupMemoryImageVertexChunk* p_vertexmemorychunk = (tlPrimGroupMemoryImageVertexChunk*) sub2;
                                            vertexmemsize = p_vertexmemorychunk->GetMemoryImageVertexSize();
                                            //printf("%u bytes \n", p_vertexmemorychunk->GetMemoryImageVertexSize());                                                                  
                                        }
                                    }
                                }//end for

                            }//end case
                        }//end switch

                    }//end outer for loop.


                    meshstats.add(p_mesh->GetName(),1,vertexmemsize);
                    outchunk->AppendSubChunk(sub,FALSE);
                    break;

                }



            case Pure3D::Mesh::MESH:               
                {
                    counter++;

                    tlDataChunk* p_sub =NULL;
                    
                    unsigned int i = 0;
                    p_sub = sub->GetSubChunk(i);

                    while (p_sub ->ID( ) == Pure3D::Mesh::PRIMGROUP )
                    {
                        tlMeshChunk* p_mesh = (tlMeshChunk* ) sub;
                        tlPrimGroup* p_primgroup = new tlPrimGroup ();
                        unsigned int vertexmemsize =0;
                        p_primgroup->LoadFromChunk(p_mesh->GetSubChunk(i));                   
                                      
                        if( p_primgroup->GetTriangleCount() >MAX_POLYCOUNT)
                        {
                                               
                            if(p_primgroup->GetType() == PDDI_PRIM_TRIANGLES)
                            {
                                sprintf(screenoutput,"%-30s mesh => Polycount : %6d \n",p_mesh->GetName(),p_primgroup->GetTriangleCount());
                                p_polycount->add(screenoutput);
                            }

                            if(p_primgroup->GetType() == PDDI_PRIM_TRISTRIP)
                            {
                                sprintf(screenoutput,"%-30s mesh => Polycount Tristriped: %6d \n",p_mesh->GetName(),p_primgroup->GetTriangleCount());
                                p_polycount->add(screenoutput);
                            }
                        }
                
                        if(p_primgroup->GetVertexType()==8209)
                        {                        
                            sprintf(screenoutput,"%-30s =====>ERROR: Mesh has No CBV (Color by Vertex) Lighting INFO! ", p_mesh->GetName());
                            p_mesh_ref->add(screenoutput);
                            badart=true;
                        }
                        
                        //hack to make every 3rd shader be char swatch.
                        if (counter == 3)
                        {
                            counter =0;
                            p_primgroup->SetShader("char_swatch_m");
                        }

                        if (bad_shaders.inlist(p_primgroup->GetShader( ) ))
                        {
                            sprintf(screenoutput,"%-30s =====>ERROR: Referencing bad shader : %-30s", p_mesh->GetName(), p_primgroup->GetShader());
                            p_mesh_ref->add(screenoutput);
                            badart=true;
                        }

                        delete p_primgroup;
                        i++;
                    


                        // memory checkfunctionality
                        int chunkcount1,chunkcount2;

                        for (chunkcount1 =0;chunkcount1<sub->SubChunkCount();chunkcount1++)
                        {
                            tlDataChunk* sub1=sub->GetSubChunk(chunkcount1);

                            switch(sub1->ID())
                            {
                            case Pure3D::Mesh::PRIMGROUP:
                                {
                                    for (chunkcount2=0;chunkcount2<sub1->SubChunkCount();chunkcount2++)
                                    {
                                        tlDataChunk* sub2=sub1->GetSubChunk(chunkcount2);
                                        switch (sub2->ID())
                                        {
                                        case Pure3D::Mesh::MEMORYIMAGEVERTEXLIST:
                                            {
                                                tlPrimGroupMemoryImageVertexChunk* p_vertexmemorychunk = (tlPrimGroupMemoryImageVertexChunk*) sub2;
                                                vertexmemsize = p_vertexmemorychunk->GetMemoryImageVertexSize();
                                                //printf("%u bytes \n", p_vertexmemorychunk->GetMemoryImageVertexSize());                                                                  
                                            }
                                        }
                                    }//end for

                                }//end case
                            }//end switch

                        }//end outer for loop.

                    
                        meshstats.add(p_mesh->GetName(),1,vertexmemsize);
                        outchunk->AppendSubChunk(sub,FALSE);
						p_sub = sub->GetSubChunk(i);
                    }//end while loop
                    break;              
                }
            case Pure3D::Animation::AnimationData::ANIMATION:
                {
                
                    unsigned int animationmemsize =0;
                    int chunkcount1;

                    for (chunkcount1 =0;chunkcount1<sub->SubChunkCount();chunkcount1++)
                    {
                        tlDataChunk* sub1=sub->GetSubChunk(chunkcount1);

                        switch(sub1->ID())
                        {
                        case Pure3D::Animation::AnimationData::SIZE:
                            {                                                      
                             
                                tlAnimationSizeChunk* p_animationsizechunk = (tlAnimationSizeChunk*) sub1;
                                animationmemsize = p_animationsizechunk->GetPS2();
                                //printf("%u bytes \n", p_animationsizechunk->GetPS2());                                                                  
                                
                            }//end case
                        }//end switch
                    }//end outer for loop.
                    
                    //printf("name: %s , %u bytes \n",sub->GetName(),animationmemsize);
                    animationstats.add(sub->GetName(),1,animationmemsize);
                    outchunk->AppendSubChunk(sub,FALSE);
                    break;                 
                
                }
            
 /*
            case Simulation::Collision::OBJECT:
                {
                    //temp local variables

                    tlCollisionObjectChunk* p_collisionobject= (tlCollisionObjectChunk*)sub;
                    bv_data bv_object;
                    char source_name[max_length];
                    char object_name1[max_length];
                    char object_name2[max_length];
                    

                    //cout<<"Checking "<<p_collisionobject->GetName()<<" Collision Object "<< endl;
                    
                    //get the name of collision object
                    strcpy(source_name,p_collisionobject->GetName());
                    //cout<<"Source name: "<<source_name<<endl;
                    
                    //smash up string to remove the number appended by Maya
                    strcpy(object_name1,strtok(source_name,"_"));
                    strcpy(object_name2,strtok(NULL,"_"));
                    
                    //reform string
                    strcat(object_name1,"_");
                    strcat(object_name1,object_name2);
                   
                    
                    //set default vaules 
                    strcpy(bv_object.name,object_name1);
                    bv_object.classtype=0;
                    bv_object.physpropid=0;

                    //cout<<"New concated string: " <<object_name1<<endl;
                 
                 
                    if(bvobjectlookup(&bv_object,query,add,strict,showall))
                    {
                        badart=true;
                    }
                    
                    //creating object attribute chunk

                    tlObjectAttributeChunk* p_otc =new tlObjectAttributeChunk ();
                    p_otc->SetClassType(bv_object.classtype);
                    p_otc->SetPhyPropID(bv_object.physpropid);

                    tlPhysWrapperChunk* p_physwrapper = new tlPhysWrapperChunk ();
                    p_physwrapper->SetName(p_collisionobject->GetName());
                   
                    //append the chunks
                    //p_physwrapper->AppendSubChunk(p_otc);
                    //p_physwrapper->AppendSubChunk(sub, FALSE);
                    outchunk->AppendSubChunk(sub,FALSE);
                    break;
                }


  */
            case StateProp::STATEPROP:
                {
                    printf("This File:%s contains a stateprop \n",filename);
                    break;
                }
            default:
                 {
                    // this is not a chunk that we care about
                    // so simply copy it to the output wrapper chunk
                    // the optional parameter to AppendSubChunk
                    // indicates that we have merely copied a pointer
                    // so the destructor of outchunk should not
                    // destroy this sub-chunk as that duty will be
                    // taken care of by the destructor of inchunk
                    outchunk->AppendSubChunk(sub,FALSE);
                    break;

                 }
            }//end switch
        }//end for
	
		//record objects stats faking art object
        art_object target_object;   
		strcpy(target_object.name,"devil car");

		target_object.art_properties.vertex_count= 453;
		target_object.art_properties.poly_count=2345;
		target_object.art_properties.animation_frames=25;
		target_object.art_properties.bone_count=35;
		target_object.art_properties.shader_count=14;

/*
		if( artobjectlookup(&target_object,query,add,strict) )
		{
			cout<<"Art object:" <<target_object.name<< " has failed art check\n ";
			//exit (1);			
		}
				
*/		
		//check for shaders and textures are

	
		//all is good add physic proporties
		
		shader_data hmm;

		strcpy(hmm.name,"trunk");
		hmm.lit=false;
/*
		if (shaderlookup(&hmm,query,add,strict))
		{
			cout<<hmm.name<<"not in art db \n";
			//exit (1);
		}
*/


//}
	 //end of try

//	catch (BadQuery er)
	{
	//	cout<<"Error: " <<er.error<< endl;
	//	return 1;
	}
	
    if (badart)
    {       
        printf("\n");
        printf("\n");

        printf("*****         SUMMARY         ******* \n");
        fprintf(stdout,"ERROR: %-15s contains BAD ART and has FAILED Artchecker! \n",filename);
        
        // cleanup the no-longer-needed wrapper chunks
              
        printf("Total Memory size of    Textures: %20u bytes in %9u textures \n",texturestats.getsize(),texturestats.getindexcount());
        printf("Total Memory size of  Animations: %20u bytes in %9u animations \n",animationstats.getsize(),animationstats.getindexcount());
        printf("Total Memory size of      Meshes: %20u bytes in %9u meshes \n",meshstats.getsize(),meshstats.getindexcount());
        
        printf("*****        END OF SUMMARY         ******* \n");
        printf("\n");
        
        
        if(!suppress)
        {
            printf("\n");
            printf("\n");        
            printf("*****       Bad Meshes      *******\n");
            p_mesh_ref->print();
            printf("\n");
            printf("*****       HIGH POLY Meshes      *******\n");
            p_polycount->print();
            printf("\n");
        }
        if(showall)
        {
            printf("\n");
            printf("*****       Texture Breakdown      *******\n");
            texturestats.printverbose();
            printf("\n");
            printf("*****       Animation Breakdown      *******\n");
            animationstats.printverbose();
            printf("\n");
            printf("*****       Meshes Breakdown      *******\n");
            meshstats.printverbose();
            printf("\n");

        }
        delete p_mesh_ref ;
        delete p_polycount;
        
   /*         
        // create the new output file
        tlFile output(new tlFileByteStream(filename, omWRITE), tlFile::CHUNK32);

        if(!output.IsOpen())  
        {
            printf("Could not open %s for writing,unable to Save\n", filename);
            exit(-1);
        }

        // get the output wrapper chunk to write its data out
        // to the file
        outchunk->Write(&output);
*/
        
        delete inchunk;
        delete outchunk;
        return 1;
    }
    else
    {
        
/*     
        // create the new output file
        tlFile output(new tlFileByteStream(filename, omWRITE), tlFile::CHUNK32);

        if(!output.IsOpen()) 
        {
            printf("Could not open %s for writing\n", filename);
            exit(-1);
        }
*/
        // get the output wrapper chunk to write its data out
        // to the file
       // outchunk->Write(&output);
     
        
        // cleanup the no-longer-needed wrapper chunks
        delete inchunk;
        delete outchunk;
        delete p_mesh_ref;
        delete p_polycount;
        printf("\n");  
        printf("\n");
        printf("\n");

        printf("*****         SUMMARY         ******* \n");
        printf("\n");
        fprintf(stdout,"Filename:%-15s PASSED Artchecker. \n",filename);
        printf("\n");
        
        // cleanup the no-longer-needed wrapper chunks
              
        printf("Total Memory size of    Textures: %20u bytes in %9u textures \n",texturestats.getsize(),texturestats.getindexcount());
        printf("Total Memory size of  Animations: %20u bytes in %9u animations \n",animationstats.getsize(),animationstats.getindexcount());
        printf("Total Memory size of      Meshes: %20u bytes in %9u meshes \n",meshstats.getsize(),meshstats.getindexcount());
        
        printf("*****        END OF SUMMARY         ******* \n");

        if(showall)
        {
            printf("\n");
            printf("*****       Texture Breakdown      *******\n");
            texturestats.printverbose();
            printf("\n");
            printf("*****       Animation Breakdown      *******\n");
            animationstats.printverbose();
            printf("\n");
            printf("*****       Meshes Breakdown      *******\n");
            meshstats.printverbose();
            printf("\n");

        }
        return 0;
    }
} //end of main


//********************************************
//              Function Bodies
//********************************************

//helper function to print error messages
void debugstring (char * message)
{
#ifdef DEBUG
	cout<<message<<endl;
#endif
}


//helper funtion to check texutre return 0 if texture matches db ,1 if their is a mismatch or error
int texturelookup(const texture_data& texturedata, bool add, bool strict,bool showall,badlist& badtextures,bool suppress)  //badlist* p_badtextures
{
		bool error=false ;
		//char querybuffer [1000];
        char output [1000];
//		Result::iterator sql_iterator;
//		Row row;
        outputbuffer texture_errors;

		//sprintf(querybuffer, "SELECT * FROM textures WHERE name = '%s' ",texturedata.name);
		//query<< querybuffer;
		//Result myresult= query.store ( );
		
	
	
		//cant find texture in the database	
		//if (myresult.size ( ) ==0 )
		if (1)
		{
			
			if ( add == true)
			{
				//add object to the DB
				cout<< "Adding:" << texturedata.name << " texture to the ArtDB \n";

				//todo add stuff here
            }
			//texture not found in the the DB 
		
			if (strict)
			{
				//cout<<",Not Found !" <<endl;
                sprintf(output,"    ERROR :%s texture not found in the ArtDB",texturedata.name);
                texture_errors.add(output);
                //cout<<"ERROR :"<<texturedata.name<< " texture not found in the ArtDB \n";
				error = true;
			}

            //check texture attributes against the default limits
				              
            //check against art limits
            if(texturedata.height>MAX_TEXTURE_SIZE)
            {
                error=true;
                sprintf(output,"    ERROR : %u  Height exceeds Max Texture SIZE OF %u",texturedata.height,MAX_TEXTURE_SIZE);
                texture_errors.add(output);
             // cout<<"     =====>Warning "<<texturedata.name<<" : "<< texturedata.height << " Height exceeds Max Texture SIZE OF "<<MAX_TEXTURE_SIZE<<endl;            
            }

            if(texturedata.width>MAX_TEXTURE_SIZE)
            {
                error=true;
                sprintf(output,"    ERROR : %u  Width exceeds Max Texture SIZE OF %u",texturedata.width,MAX_TEXTURE_SIZE);
                texture_errors.add(output);
             // cout<<"     =====>Warning "<<texturedata.name<<": "<< texturedata.width << " Width exceeds Max Texture SIZE OF "<<MAX_TEXTURE_SIZE<<endl;            
            }

            if(texturedata.bpp>MAX_BPP)
            {         
                error=true;
                sprintf(output,"    ERROR : %u  BPP exceeds Max BPP SIZE OF %u",texturedata.bpp,MAX_BPP);
                texture_errors.add(output);
             //cout<<"     =====>Warning "<<texturedata.name<<": "<< texturedata.bpp << " BPP exceeds Max BPP SIZE OF "<<MAX_BPP<<endl;            
             }			
             //	  cout<<"     Warning :"<<texturedata.name<< " texture not found in the ArtDB \n";    
		
		}//end if  in artdb block

        //if(myresult.size()!=0)
		if(0)
        {
		    //cout <<",Found . " << endl;

          /*
			for (sql_iterator=myresult.begin ( ); sql_iterator!= myresult.end ( ); sql_iterator++)
		    {
		                
			    row =*sql_iterator;
                int height= row["height"];
                int width=row["width"];
                int bpp=row["bpp"];
                int alpha=row["alphabits"];
                              

                //check texture height
                if( texturedata.height > height)
                {
			        if (strict)
                    {
                        error=true;
                        sprintf(output,"    ERROR :Texture height %u exceeds expected %u",texturedata.height,height);
                        texture_errors.add(output);
                    }
                    else
                    {
                        sprintf(output,"    Warning :Texture height %u exceeds expected %u",texturedata.height,height);
                        texture_errors.add(output);
                    }
                }//end if

                //check width
                if( texturedata.width > width)
                {
			        if (strict)
                    {
                        error=true;
                        sprintf(output,"    ERROR :Texture width %u exceeds expected %u",texturedata.width,width);
                        texture_errors.add(output);
                    }
                    else
                    {
                        sprintf(output,"    Warning :Texture width %u exceeds expected %u",texturedata.width,width);
                        texture_errors.add(output);
                    }
                }//end if

                //check bpp
                if( texturedata.bpp > bpp)
                {
			        if (strict)
                    {
                        error=true;
                        sprintf(output,"    ERROR :Texture BPP %u exceeds expected %u",texturedata.bpp,bpp);
                        texture_errors.add(output);
                    }
                    else
                    {                        
                        sprintf(output,"    Warning :Texture BPP %u exceeds expected %u",texturedata.bpp,bpp);
                        texture_errors.add(output);
                    }
                }//end if

                          
                
                
            //just printing out the rows
			//for (int j=0;j<row.size ( ); j++)
			//{
			//	cout<< "[" << row[j] << "]" ;
			//}
			//cout<<endl;

		    }//end for iterator loop

  */
       
        }//end if found in the Artdb block

	
   

    if (error)
    {
        if(!suppress)
        {
            printf("\n");
            printf("%s Texture:\n",texturedata.name);                             
            texture_errors.print();
            printf("\n");
        }
            badtextures.add(texturedata.name,1,0);
        return 1;
    }
    else
    {
        return 0;
    }

}// end texture lookup


//helper funtion to check shader, returns 0 if shader matches db ,1 if their is a mismatch
int shaderlookup(shader_data* lp_shaderdata, bool add, bool strict,bool showall,badlist& badshaders,bool suppress)
{

    bool error=false;
	char querybuffer [1000];
    char output [1000];
	//Result::iterator sql_iterator;
	//Row row;
    outputbuffer shader_errors;
	sprintf(querybuffer, "SELECT * FROM shaders WHERE name = '%s' ",lp_shaderdata->name);
	//query<< querybuffer;
	//Result myresult= query.store ( );
	
			
//	if (myresult.size ( ) ==0 )
	if (1)
	{
	
	    if ( add == true)
		{
		//add object to the DB
		printf("Adding: %s  shader to the ArtDB \n",lp_shaderdata->name);
		//todo add stuff here
		}
		        
		if (strict)
		{         
            sprintf(output,"    ERROR: %s Shader not found in the ArtDB",lp_shaderdata->name);
            shader_errors.add(output);
         }					                 
                
         //check for lit
         if ( lp_shaderdata->lit)
         {                             
               shader_errors.add("    =====>ERROR   : LIT shader FOUND!");
               error = true;
              
               if(badshaders.inlist(lp_shaderdata->name) ==0)
               {
                   badshaders.add(lp_shaderdata->name,1,0);
                }  
         }         
/*
         if ((lp_shaderdata->twosid) == 0)
         {
             shader_errors.add("    =====>Warning : Double Sided FLAG is 0!");
             error=true;
          }                                    

 */

	}//end if shader in artdb check
		
	//cout << "Records Found: " << myresult.size() << endl ;
/*		

	for (sql_iterator=myresult.begin ( ); sql_iterator!= myresult.end ( ); sql_iterator++)
		{	
			row =*sql_iterator;
   			
			//just printing out the rows
			for (int j=0;j<row.size ( ); j++)
			{
				cout<< "[" << row[j] << "]" ;
			}
			cout<<endl;
  		}//end for iterator loop
*/

    if (error)
    {
        if(!suppress)
        {

            printf("\n");
            printf("%s shader : \n",lp_shaderdata->name);
            shader_errors.print();
        }
        if(strict)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    { 
        return 0;
    }

}// end shader lookup

/*


//helper funtion to check object, returns 0 if shader matches db ,1 if their is a mismatch
int artobjectlookup(art_object* lp_artobject, Query& query,bool add, bool strict,bool showall,bool suppress)
{
		
		char querybuffer [1000];
		Result::iterator sql_iterator;
		Row row;
		unsigned int artpropid,shaderid;
        bool error= false;

		//setup a query to artdb ask if object is in Artdb
		
		sprintf(querybuffer, "SELECT * FROM objects WHERE name = '%s' ",lp_artobject->name);	
		query<< querybuffer;
		Result myresult= query.store ( );
		//cout << "Records Found: " << myresult.size() << endl ;
		
        //if result size is zero, object not found
		//if ( myresult.size ( ) ==0 )
		if(1)
		{		
			if ( add == true)
			{
				//add object to the DB
				cout<< "Adding:" << lp_artobject->name << " to the ArtDB \n";
				//todo add stuff here

				return 0;
			}
			//object not found in the the DB halt
			else
			{
				if (strict)
				{
					printf("ERROR: %s object not found in the ArtDB, \n",lp_artobject->name);
					error =true;
				}
				else
				{
					printf("Warning:%s object not found in the ArtDB \n",lp_artobject->name);
				}
			}
		}//end if object in artdb check
						
	
		for (sql_iterator=myresult.begin ( ); sql_iterator!= myresult.end ( ); sql_iterator++)
		{
			row =*sql_iterator;
    		artpropid = row["artpropid"];
			shaderid = row["shaderid"];
			//just printing out the rows
			for (int j=0;j<row.size ( ); j++)
			{
				cout<< "[" << row[j] << "]" ;
			}
			cout<<endl;
		}//end for iterator loop
		
		
		//check check the art proporties  if less than max vertices,etc then we ok
		sprintf(querybuffer, "SELECT * FROM artprops WHERE id = '%i' ",artpropid);
		query<< querybuffer;
		myresult= query.store ( );

		cout << "Records Found: " << myresult.size() << endl ;
		//check if artprops in the ArtDB
		//if ( myresult.size ( ) ==0 )
		if(1)
		{
			if ( add == true)
			{
				//add object to the DB
				cout<< "Adding:" <<lp_artobject->name << " art props to the ArtDB \n";
				cout<< "Adding:  Artprops to artdb \n";
				return 0;

				//todo add stuff here
			}

			//object not found in the the DB halt
			else
			{
				if (strict)
				{
					cout<<"ERROR: "<<lp_artobject->name<< " Art properties not found in the ArtDB, Halting \n";
					error=true;
				}
				else
				{
					cout<<"Warning: "<<lp_artobject->name<< " Art properties not found in the ArtDB, using default physic properties\n";
				}//end else
			}//end else
		}//end if		
		
		for (sql_iterator=myresult.begin ( ); sql_iterator!= myresult.end ( ); sql_iterator++)
		{
			row =*sql_iterator;
			
			unsigned int vertexcount,polycount,shadercount,bonecount,animationframes;
			vertexcount=row["VertexCount"];
			polycount=row["PolyCount"];
			shadercount=row["ShaderCount"];
			bonecount=row["BoneCount"];
			animationframes=row["Animframes"];
		
			if (strict)
			{
				if(	lp_artobject->art_properties.vertex_count > vertexcount)
				{
					cout<<"ERROR:"<< lp_artobject->name<<" has exceeded " << row["VertexCount"]<<" Vertexcount, HALTING \n";
					error =true;
				}
				
				if(	lp_artobject->art_properties.poly_count > polycount)
				{
					cout<<"ERROR:"<< lp_artobject->name<<" has exceeded " << row["PolyCount"]<<" Polycount, HALTING \n";
					error =true;
				}
			
				if(	lp_artobject->art_properties.animation_frames > animationframes)
				{
					cout<<"ERROR:"<< lp_artobject->name<<" has exceeded " << row["AnimFrames"]<< " Animation frames, HALTING \n";
					error =true;
				}
				
				if(	lp_artobject->art_properties.bone_count > bonecount)
				{	
					cout<<"ERROR:"<< lp_artobject->name<<" has exceeded " << row["BoneCount"]<<" Bonecount, HALTING \n";
					error =true;
				}
				
				if(	lp_artobject->art_properties.shader_count > shadercount)
				{
					cout<<"ERROR:"<< lp_artobject->name<<" has exceeded " << row["ShaderCount"]<<" Shadercount, HALTING\n";
					error =true;
				}
			}//end if artprops check
		else
			{
				if(	lp_artobject->art_properties.vertex_count > vertexcount)
				{
					cout<<"Warning:"<< lp_artobject->name<<" has exceeded " << row["VertexCount"]<<" Vertexcount \n";
				}
				
				if(	lp_artobject->art_properties.poly_count > polycount)
				{
					cout<<"Warning:"<< lp_artobject->name<<" has exceeded " << row["PolyCount"]<<" Polycount \n";
				}
			
				if(	lp_artobject->art_properties.animation_frames > animationframes)
				{
					cout<<"Warning:"<< lp_artobject->name<<" has exceeded " << row["AnimFrames"]<< " Animation frames \n";
				}
				
				if(	lp_artobject->art_properties.bone_count > bonecount)
				{	
					cout<<"Warning:"<< lp_artobject->name<<" has exceeded " << row["BoneCount"]<<" Bonecount  \n";
				}
				
				if(	lp_artobject->art_properties.shader_count > shadercount)
				{
					cout<<"Warning:"<< lp_artobject->name<<" has exceeded " << row["ShaderCount"]<<" Shadercount \n";
				}
			}// end else artprops check


		
		}//for loop end artobjectlookup

        
		//printing art props
		
		for (int j=0;j<row.size ( ); j++)
		{
			cout<< "[" << row[j] << "]" ;
		}

  
		printf("\n");

    if (error)
    {
        return 1;
    }
    else
    {    
	    return 0;
    }

}//end artobject lookup


//*************************************************************************
// Check's if root of Bound Volume exists, if not then treats it as a strict object
		
int bvobjectlookup(bv_data* lp_bvobject,Query& query,bool add,bool strict,bool showall,bool suppress)
{
    bool errors =false;
    char querybuffer [1000];
    char output[1000];
    outputbuffer bv_errors;
	Result::iterator sql_iterator;
	Row row;


    sprintf(querybuffer, "SELECT * FROM objects WHERE name = '%s' ",lp_bvobject->name);
	query<< querybuffer;
	Result myresult= query.store ( );
	
			
	if (myresult.size ( ) ==0 )
	{	
        if ( add == true)
		{
				//add object to the DB
				//cout<< "Adding:" << lp_shaderdata->name << " shader to the ArtDB \n";

				//todo add stuff here
		}
			//object not found in the the DB halt
		else
		{
		    if (strict)
		    {
					sprintf(output,"ERROR: %s object not found in the ArtDB \n",lp_bvobject->name);
                    bv_errors.add(output);
                    //cout<<"     ERROR:"<<lp_bvobject->name<< " object not found in the ArtDB \n";
					//return 1;
                    errors =true;
			}
			else
			{
                    errors =true;
                    sprintf(output,"Warning: %s object not found in the ArtDB , using  defaults vaules for ObjectAttributeChunk \n",lp_bvobject->name);
                    bv_errors.add(output);
					//cout<<"     Warning:"<<lp_bvobject->name<< " bv object not found in the ArtDB, using  defaults vaules for ObjectAttributeChunk \n";
                    lp_bvobject->physpropid=0;
                    lp_bvobject->classtype=2; // set to dsg_static_phys
			}

		}

	}//end if object check in artdb check
		
	//	cout << "Records Found: " << myresult.size() << endl ;
	
	for (sql_iterator=myresult.begin ( ); sql_iterator!= myresult.end ( ); sql_iterator++)
		{
			
			row =*sql_iterator;
            lp_bvobject->classtype =2;
            lp_bvobject->physpropid=row["physpropid"];

			
			//just printing out the rows
			for (int j=0;j<row.size ( ); j++)
			{
				cout<< "[" << row[j] << "]" ;
			}

			cout<<endl;


	}//end for iterator loop

    if (errors)
    {
        if(strict)
        {
            bv_errors.print();
            return 1;
        }
        else
        {
            if(!suppress)
            {
                bv_errors.print ();
            }
            return 0;
        }
    }
    else
    {
        return 0;
    }
}// end of Bv object check


*/