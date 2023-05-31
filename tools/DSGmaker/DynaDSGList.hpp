#ifndef DYNADSGLIST_H
#define DYNADSGLIST_H

//custom list class,stores the address of DynaPhysDSG in a array,that i create in DSGmaker

class tlDynaPhysDSGChunk ;

class DynaPhysDSGList 
{
    public:
        DynaPhysDSGList();
        virtual ~DynaPhysDSGList();
        int AddDyna(tlDynaPhysDSGChunk*  p_dyna);
        unsigned int GetIndexCount();
        tlDynaPhysDSGChunk* GetDyna(const char* name);
        

    private:
        tlDynaPhysDSGChunk** mp_list;
        unsigned int mindex;        
};
#endif //end of file