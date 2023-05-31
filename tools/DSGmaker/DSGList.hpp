#ifndef DSGLIST_H
#define DSGLIST_H

//custom list class,stores the address of DynaPhysDSG in a array,that i create in DSGmaker

class tlDataChunk ;



class DSGList 
{
    public:

        bool mbNoInstanceChunks;
        DSGList();
        virtual ~DSGList();
        int AddDSG(tlDataChunk*  p_dsg);
        unsigned int GetIndexCount();
        tlDataChunk* GetDSG(const char* name);
        tlDataChunk* GetDSGByIndex(unsigned int i);
        unsigned int VerifyChunks(void);        
        void ReSortDSGChunks(void);

    private:
        void PrintErrors(void);
        tlDataChunk** mp_list;
        unsigned int mindex;        
};
#endif //end of file