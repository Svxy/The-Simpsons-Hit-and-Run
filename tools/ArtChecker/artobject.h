#define max_length 64
#define max_textures 3
#define max_shaders 14

struct  texture_data 
{
	char name[max_length] ;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned int alpha;
};


struct shader_data
{
	char name[max_length];
	unsigned int lit;
    unsigned int twosid;

	texture_data texture_list[max_textures];
};

struct art_data
{
	unsigned int vertex_count;
	unsigned int poly_count;
	unsigned int animation_frames;
	unsigned int shader_count;
	unsigned int bone_count;
};


struct bv_data
{
    char name[max_length];
    unsigned int physpropid;
    unsigned int classtype;
};
   


class art_object
{
	
public:
//data
	shader_data shader_list[max_shaders];
	art_data art_properties;
	char name[max_length];

//methods
	art_object ( );
	~art_object( );
	
};

		

