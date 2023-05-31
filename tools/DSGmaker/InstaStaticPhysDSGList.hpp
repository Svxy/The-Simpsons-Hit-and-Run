#ifndef INSTAPHYSDSGLIST_H
#define INSTAPHYSDSGLIST_H

//custom list class,stores the address of InstaEntityDSG's in a array,that i create in DSGmaker

class tlInstaStaticPhysDSGChunk ;

class InstaStaticPhysDSGList 
{
    public:
        InstaStaticPhysDSGList();
        virtual ~InstaStaticPhysDSGList();
        int AddInstaStaticPhys(tlInstaStaticPhysDSGChunk*  p_insta);
        tlInstaStaticPhysDSGChunk* GetInstaStaticPhys(const char* name);         

    private:
        tlInstaStaticPhysDSGChunk** mp_list;
        unsigned int mindex;        
};
#endif //end of file