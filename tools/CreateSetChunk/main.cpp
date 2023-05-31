/*===========================================================================
   File: main.cpp 

   This tool turns a greyscale image into a raw look up table.

   Copyright (c)  Radical Entertainment, Inc.  All rights reserved.

===========================================================================*/

#include <stdio.h>
#include <assert.h>
#include <toollib.hpp>
#include <dospath.hpp>
#include <tlFrameControllerChunk16.hpp>
#include <tlAnimationChunk.hpp>
#include <atenum.hpp>
#include <chunkids.hpp>

#include "Param.hpp"
#include <tlSetChunk.hpp>

Parameters* Param;
char outFile[P3DMAXNAME];

struct TextureData
{
    TextureData() :
		m_TextureID( 0 ),
		m_NumIDs( 0 ),
		m_ShaderID( 0 ),
		m_AnimationChunk( 0 ),
		m_ControllerChunk( 0 ),
//		m_MultiControllerChunk( 0 ),
		m_pSetChunk( 0 ),
		m_WritenOut( false ) {};
    ~TextureData() { delete m_TextureID; } // Set chunk is deleted by the output chunk.
    TLUID* m_TextureID;           // List of texture chunks to put into set chunk.
    int m_NumIDs;                 // Number in list. If number is negative then just the animation/controller chunk are removed and multicontroller modified.
    TLUID m_ShaderID;             // The shader this data is for.
    TLUID m_AnimationChunk;       // Texture animation chunk to remove.
    TLUID m_ControllerChunk;      // Texture frame controller chunk to remove.
//	TLUID m_MultiControllerChunk; // MultiController to remove texture frame controller from.
    tlSetChunk* m_pSetChunk;      // Set chunk which will be written to file.
    bool m_WritenOut;             // Has the set chunk been written out?
};

static const int TEXTURE_COLLECTION_SIZE = 256;
static TextureData* g_TextureCollection[ TEXTURE_COLLECTION_SIZE ];
static int g_TextureCollectionCount = 0;

int main(int argc, char* argv[])
{
	Param = new Parameters(argc,argv);
    bool sameFile = false; // Have we switched to a new output file.

	tlDataChunk::RegisterDefaultChunks();

	tlFile* output = 0;

    for( int i = 0; i < TEXTURE_COLLECTION_SIZE; ++i )
    {
        g_TextureCollection[ i ] = 0;
    }

	if( Param->Files.Count() < 1 )
	{
		printf( "Must specify at least one input file.\n" );
		exit( -1 );
	}
    if( Param->ShaderName == 0 )
    {
        printf( "Must specify the name of the shader with texture animation.\n" );
        exit( -1 );
    }

    bool wildCardShader = false;
    int shaderNameLen = strlen( Param->ShaderName );
    if( Param->ShaderName[ shaderNameLen - 1 ] == '*' )
    {
        wildCardShader = true;
        --shaderNameLen;
        Param->ShaderName[ shaderNameLen ] = '\0';
    }

	// for each file on the command-line, do the following:
	for(int curFile = 0; curFile < Param->Files.Count(); curFile++) 
	{
		tlFile input(new tlFileByteStream(Param->Files[ curFile ], omREAD), tlFile::FROMFILE);
		if(!input.IsOpen()) 
		{
			printf("Could not open %s\n", Param->Files[ curFile ]);
			exit( -1 );
		}
        tlDataChunk* inChunk = new tlDataChunk( &input );

		char outFileName[256];
		if( output == 0 )
		{
			if( Param->OutputFile )
			{
				strcpy(outFileName, Param->OutputFile);
			}
			else
			{
				strcpy(outFileName, Param->Files[curFile]);
				RemoveExtension(outFileName);
				strcat(outFileName, ".p3d");
			}
			output = new tlFile(new tlFileByteStream(outFileName, omWRITE), tlFile::CHUNK32);
			if( ( output == 0 ) || ( !output->IsOpen() ) )
			{
				printf("Could not open %s for writing\n", outFileName);
				exit(-1);
			}
            sameFile = false;
		}
        for( int i = 0; i < g_TextureCollectionCount; ++i )
        {
            delete g_TextureCollection[ i ];
            g_TextureCollection[ i ] = 0;
        }

        /*************************************************/
        // First pass for input file, nose through the frame controllers.
        // Go through all the sub-chunks of the input and look for shader name
        //in heirarchy of texture frame controller.
        int inChunkIndex;
        for( inChunkIndex = 0; inChunkIndex < inChunk->SubChunkCount(); inChunkIndex++ )
        {
            tlDataChunk* sub = inChunk->GetSubChunk( inChunkIndex );
            if( sub->ID() == Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER )
            {
                // We have a frame controller...
                tlFrameControllerChunk* controller = static_cast<tlFrameControllerChunk*>( sub );
                if( controller->GetType() == Pure3DAnimationTypes::TEXTURE_TEX )
                {
                    // and it's a texture controller. So check the hierarchy name.
                    bool foundShader = false;
                    const char* heirName = controller->HierarchyName();
                    if( wildCardShader )
                    {
                        foundShader = strncmp( heirName, Param->ShaderName, shaderNameLen ) == 0;
                    }
                    else
                    {
                        foundShader = strcmp( heirName, Param->ShaderName ) == 0;
                    }
                    if( foundShader )
                    {
                        // We found the shader so remember the texture animation data.
                        TextureData* texData = new TextureData();
                        if( texData == 0 )
                        {
                            printf( "Unable to allocate texture data.\n" );
                            exit( -1 );
                        }
                        texData->m_ShaderID = tlEntity::MakeUID( heirName );
                        texData->m_ControllerChunk = tlEntity::MakeUID( controller->GetName() );
                        texData->m_AnimationChunk = tlEntity::MakeUID( controller->AnimationName() );
                        // Look to see if we already have this shader.
                        //If we do, we'll just erase the controller and animation and modify the multicontroller.
                        //The first texData will be used to create the chunk set.
                        for( int i = 0; i < g_TextureCollectionCount; ++i )
                        {
                            if( g_TextureCollection[ i ]->m_ShaderID == texData->m_ShaderID )
                            {
                                texData->m_NumIDs = -1;
                                break;
                            }
                        }
                        g_TextureCollection[ g_TextureCollectionCount ] = texData;
                        ++g_TextureCollectionCount;
                    }
                }
            }
        }
        /*************************************************/
        // Second pass. Now we look for the animation to go with all those texture frame controllers
        //we found.
        for( inChunkIndex = 0; inChunkIndex < inChunk->SubChunkCount(); inChunkIndex++ )
        {
            tlDataChunk* sub = inChunk->GetSubChunk( inChunkIndex );
			// Is this an animation?
            if( sub->ID() != Pure3D::Animation::AnimationData::ANIMATION )
            {
				// No, not interested.
                continue;
            }
            tlAnimationChunk* anim = static_cast<tlAnimationChunk*>( sub );
			// Is it a texture animation?
            if( anim->GetAnimationType() != Pure3DAnimationTypes::TEXTURE_TEX )
            {
				// No, not interested.
                continue;
            }
            // We've found a texture animation. Check if it's one we are looking for.
            TLUID animID = tlEntity::MakeUID( anim->GetName() );
            for( int i = 0; i < g_TextureCollectionCount; ++i )
            {
                if( ( g_TextureCollection[ i ]->m_NumIDs == 0 ) && ( g_TextureCollection[ i ]->m_AnimationChunk == animID ) )
                {
                    // We've found the animation to match a frame controller. Let the mess begin.
                    TextureData* texData = g_TextureCollection[ i ]; // just for convience.
                    // Examine the sub chunks...there should be a goup list in there...
                    for( int subChunkIndex = 0; subChunkIndex < anim->SubChunkCount(); ++subChunkIndex )
                    {
                        tlDataChunk* animSub = anim->GetSubChunk( subChunkIndex );
                        if( animSub->ID() == Pure3D::Animation::AnimationData::GROUP_LIST )
                        {
                            // Found the group list. Now we'll nose around the groups...
                            tlAnimationGroupListChunk* groupList = static_cast<tlAnimationGroupListChunk*>( animSub );
                            for( int listChunkIndex = 0; listChunkIndex < groupList->SubChunkCount(); ++listChunkIndex )
                            {
                                // Look at all the channels in the groups in the group list.
                                tlDataChunk* listSub = groupList->GetSubChunk( listChunkIndex );
                                if( listSub->ID() == Pure3D::Animation::AnimationData::GROUP )
                                {
                                    tlAnimationGroupChunk* group = static_cast<tlAnimationGroupChunk*>( listSub );
                                    for( int channelIndex = 0; channelIndex < (int)group->GetNumChannels(); ++channelIndex )
                                    {
                                        int i;
                                        // In the group (whew), look at all the channels.
										// Is it an entity channel?
                                        tlDataChunk* channelSub = group->GetSubChunk( channelIndex );
                                        if( channelSub->ID() != Pure3D::Animation::ChannelData::ENTITY )
                                        {
											// No, not interested.
                                            continue;
                                        }
										// Is the entity channel a texture?
                                        tlEntityChannelChunk* entityChannel = static_cast<tlEntityChannelChunk*>( channelSub );
                                        if( entityChannel->Param() != Pure3DAnimationTypes::TEXTURE_TEX )
                                        {
											// No, not interested.
                                            continue;
                                        }
                                        // Okay we have the entity channel for a texture animation, now we
                                        //record the texture names.
                                        // We might end up allocating extra space for textures, but we'll
                                        //leave the UIDs at zero at the end of the array.
                                        texData->m_TextureID = new TLUID[ entityChannel->GetNumFrames() ];
                                        if( texData->m_TextureID == 0 )
                                        {
                                            printf( "Unable to allocate texture array.\n" );
                                            exit( -1 );
                                        }
                                        for( i = 0; i < (int)entityChannel->GetNumFrames(); ++i )
                                        {
                                            texData->m_TextureID[ i ] = 0;
                                        }
                                        // Get all the frame values.
                                        char** values = entityChannel->GetValues();
                                        for( i = 0; i < (int)entityChannel->GetNumFrames(); ++i )
                                        {
                                            TLUID textureID = tlEntity::MakeUID( values[ i ] );
                                            // We have the texture tUID, look if it's a duplicate.
                                            bool textureDup = false;
                                            for( int j = 0; j < texData->m_NumIDs; ++j )
                                            {
                                                if( texData->m_TextureID[ j ] == textureID )
                                                {
													// this one is a duplicate, skip it.
                                                    textureDup = true;
                                                    break;
                                                }
                                            }
                                            if( !textureDup )
                                            {
                                                // It's not a duplicate so remember it.
                                                texData->m_TextureID[ texData->m_NumIDs ] = textureID;
                                                ++texData->m_NumIDs;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        /*************************************************/
        // Third pass, create the set chunks from the textures we got from those texture animations.
        for( inChunkIndex = 0; inChunkIndex < inChunk->SubChunkCount(); inChunkIndex++ )
        {
            tlDataChunk* sub = inChunk->GetSubChunk( inChunkIndex );
            if( sub->ID() == Pure3D::Texture::TEXTURE )
            {
                // We've found a texture, check to see if it's one of the textures we are looking for.
                TextureData* texData = 0;
                int textureIDIndex = 0;
                TLUID textureID = tlEntity::MakeUID( sub->GetName() );
                for( int i = 0; i < g_TextureCollectionCount; ++i )
                {
                    texData = g_TextureCollection[ i ];
                    for( textureIDIndex = 0; textureIDIndex < texData->m_NumIDs; ++textureIDIndex )
                    {
                        if( textureID == texData->m_TextureID[ textureIDIndex ] )
                        {
                            if( texData->m_pSetChunk == 0 )
                            {
                                texData->m_pSetChunk = new tlSetChunk();
                                if( texData->m_pSetChunk == 0 )
                                {
                                    printf( "Unable to create tlSetChunk.\n" );
                                    exit( -1 );
                                }
                            }
                            if( textureIDIndex == 0 )
                            {
                                // This is the first texture in the set so we'll use the name for
                                //the set.
                                texData->m_pSetChunk->SetName( sub->GetName() );
                            }
                            texData->m_pSetChunk->AppendSubChunk( sub, 0 );
                            break;
                        }
                    }
                }
            }
        }       

        /*************************************************/
        // Forth pass, process the file. Copy all the chunks over,
        //except the texture frame controllers, texture animations,
        //and textures we've used. Also, modify the multicontrollers
		//to exclude the texture frame controllers.
		tlDataChunk* outChunk = new tlDataChunk;

        if( Param->WriteHistory && !sameFile )
        {
            // put a history chunk in the output
            // a history chunk shows what version of the tool
            // was run on the file with what command-line 
            // parameters
            outChunk->AppendSubChunk(Param->HistoryChunk());
            sameFile = true;
        }
        
        if( outChunk == 0 )
        {
            printf( "Unable to create output chunk.\n" );
            exit( -1 );
        }
        for( inChunkIndex = 0; inChunkIndex < inChunk->SubChunkCount(); inChunkIndex++ )
        {
            tlDataChunk* sub = inChunk->GetSubChunk( inChunkIndex );
            if( sub->ID() == Pure3D::Texture::TEXTURE )
            {
                bool inAnySet = false;
                for( int i = 0; i < g_TextureCollectionCount; ++i )
                {
                    // Theorically a texture can go into multiple sets so go through them all.
                    bool found = false;
                    TextureData* texData = g_TextureCollection[ i ];
                    for( int j = 0; j < texData->m_NumIDs; ++j )
                    {
                        // Look to see if this texture is in this set.
                        if( tlEntity::MakeUID( sub->GetName() ) == texData->m_TextureID[ j ] )
                        {
                            found = true;
                            inAnySet = true;
                            break;
                        }
                    }
                    if( found && ( texData->m_NumIDs > 0 ) )
                    {
                        // This texture has gone into this set chunk. We'll write the
                        //set chunk out at this point so it's roughly in the same place in
                        //file to protect against load order dependency problems.
                        if( texData->m_WritenOut == false )
                        {
                            texData->m_pSetChunk->SetChildCount( texData->m_NumIDs );
                            outChunk->AppendSubChunk( texData->m_pSetChunk );
                            texData->m_WritenOut = true; // Only write it once.
                        }
                    }
                }
                if( !inAnySet )
                {
                    // This isn't a texture we're interested in so write it out.
                    outChunk->AppendSubChunk( sub );
                }
            }
            else if( sub->ID() == Pure3D::Animation::FrameControllerData::FRAME_CONTROLLER )
            {
				// This is a frame controller. If it's one we've used then don't include it.
                bool found = false;
                TLUID controllerID = tlEntity::MakeUID( sub->GetName() );
                for( int i = 0; i < g_TextureCollectionCount; ++i )
                {
                    if( controllerID == g_TextureCollection[ i ]->m_ControllerChunk )
                    {
                        found = true;
                        break;
                    }
                }
                if( !found )
                {
                    outChunk->AppendSubChunk( sub );
                }
            }
            else if( sub->ID() == Pure3D::Animation::AnimationData::ANIMATION )
            {
				// This is an animation. If it's one we've used then don't include it.
                bool found = false;
                TLUID animID = tlEntity::MakeUID( sub->GetName() );
                for( int i = 0; i < g_TextureCollectionCount; ++i )
                {
                    if( animID == g_TextureCollection[ i ]->m_AnimationChunk )
                    {
                        found = true;
                        break;
                    }
                }
                if( !found )
                {
                    outChunk->AppendSubChunk( sub );
                }
            }
			else if( sub->ID() == P3D_MULTI_CONTROLLER )
			{
				// This is a multicontroller. If a track references an framecontroller we
				//used then exclude that track.
				tlMultiControllerChunk16* multiCont = static_cast<tlMultiControllerChunk16*>( sub );
				for( int subIndex = 0; subIndex < multiCont->SubChunkCount(); ++subIndex )
				{
					tlDataChunk* subChunk = multiCont->GetSubChunk( subIndex );
					if( subChunk->ID() == P3D_MULTI_CONTROLLER_TRACKS )
					{
						tlMultiControllerTracksChunk16* tracks = static_cast<tlMultiControllerTracksChunk16*>( subChunk );
						unsigned long numTracks = tracks->GetNumTracks();
						tlMultiControllerTrackData* trackArray = tracks->GetTracks();
						for( int i = 0; i < numTracks; ++i )
						{
							TLUID trackID = tlEntity::MakeUID( trackArray[ i ].name );
							for( int j = 0; j < g_TextureCollectionCount; ++j )
							{
								if( trackID == g_TextureCollection[ j ]->m_ControllerChunk )
								{
									// Remove this track.
									--numTracks;
									tracks->SetNumTracks( numTracks );
									multiCont->SetNumTracks( multiCont->GetNumTracks() - 1 );
									if( i < numTracks )
									{
										trackArray[ i ] = trackArray[ numTracks ];
										--i;
									}
								}
							}
						}
					}
					else if( subChunk->ID() == P3D_MULTI_CONTROLLER_TRACK )
					{
						tlMultiControllerTrackChunk16* track = static_cast<tlMultiControllerTrackChunk16*>( subChunk );
						TLUID trackID = tlEntity::MakeUID( track->GetName() );
						for( int i = 0; i < g_TextureCollectionCount; ++i )
						{
							if( trackID == g_TextureCollection[ i ]->m_ControllerChunk )
							{
								multiCont->RemoveSubChunk( subIndex );
								multiCont->SetNumTracks( multiCont->GetNumTracks() - 1 );
							}
						}
					}
				}
				outChunk->AppendSubChunk( sub );
			}
            else
            {
                outChunk->AppendSubChunk( sub );
            }
        }
		outChunk->Write( output );
		delete outChunk;
		outChunk = 0;
		if( Param->OutputFile == 0 )
		{
			// We have multiple output files so get rid of the
			//the output file now.
			delete output;
			output = 0;
		}
        for( i = 0; i < g_TextureCollectionCount; ++i )
        {
            delete g_TextureCollection[ i ];
            g_TextureCollection[ i ] = 0;
        }
	} // end of main for loop

   delete Param;
   return 0;
}
