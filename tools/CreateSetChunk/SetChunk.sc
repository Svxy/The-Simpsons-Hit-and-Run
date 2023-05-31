tool SetChunk :
    description( "This tool turns a texture animation into a set chunk." )
{    
}
 
parameter OutputFile :
    short( o ),
    long( "output-file" ),
    arg( name ),
    description( "Specify output file." ),
    type( string )
{
}

parameter ShaderName :
   short( s ),
   long( "shader-name" ),
   arg( name ),
   description( "Specify the shader with texture animation whose textures will become set chunk." ),
   type( string )
{
}
