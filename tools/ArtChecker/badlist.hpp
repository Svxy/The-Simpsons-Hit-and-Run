//#define max_items 200
struct entity_data 
{

	char name [40];
	unsigned int occurances;
    unsigned int size;
};


class badlist
{
	public:


		int add (const char * name,const unsigned int occurances,const unsigned int size);
		int inlist (const char * name);
        int getsize();
        int getindexcount();
        int printverbose();
          
        badlist ();
        badlist (const unsigned int arraysize);
        ~badlist();

	private:
		unsigned int max_items;
		entity_data* data_array ;
		unsigned int index;
        unsigned int totalmemoryusage;

};

