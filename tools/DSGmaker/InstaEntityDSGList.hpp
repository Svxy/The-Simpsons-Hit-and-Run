#ifndef INSTAENTITYDSGLIST_H
#define INSTAENTITYDSGLIST_H

//custom list class,stores the address of InstaEntityDSG's in a array,that i create in DSGmaker

class tlInstaEntityDSGChunk ;

class InstaEntityDSGList 
{
    public:
        InstaEntityDSGList();
        virtual ~InstaEntityDSGList();
        int AddInstaEntity(tlInstaEntityDSGChunk*  p_insta);
        tlInstaEntityDSGChunk* GetInstaEntity(const char* name);         

    private:
        tlInstaEntityDSGChunk** mp_list;
        unsigned int mindex;        
};
#endif //end of file