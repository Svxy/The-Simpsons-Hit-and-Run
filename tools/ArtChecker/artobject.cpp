#include <cstring>
#include "artobject.h"


art_object::art_object ( )
{
	int i,j;
	
	for (i=0;i<max_shaders;i++)
	{
		for (j=0;j<max_textures;j++)
		{
			strcpy(shader_list[i].texture_list[j].name, "no name");
			shader_list[i].texture_list[j].width =0;
			shader_list[i].texture_list[j].height =0;
			shader_list[i].texture_list[j].bpp =0;
			shader_list[i].texture_list[j].alpha =0;
		}//end for

		strcpy (shader_list[i].name, "no name");
		shader_list[i].lit =false;
	}//end for

	//setting default art vaules
	art_properties.vertex_count =0;
	art_properties.poly_count =0;
	art_properties.animation_frames =0;
	art_properties.shader_count =0;
	art_properties.bone_count =0;
	
	//set default art_object vaules

	strcpy(name,"no name");

}

art_object::~art_object( )
{

}






