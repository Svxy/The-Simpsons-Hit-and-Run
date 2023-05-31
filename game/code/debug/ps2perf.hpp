// Performance counter class for PS2
// Nov 2001 - Amit Bakshi

#ifndef PS2PERF_HPP
#define PS2PERF_HPP

/************************************************************************************/
// nov6/2001 amb - Performance Counter class
//                 based on David Coombes' (david_coombes@playstation.sony.com) code
// sample usage : 
//
// void TestSinCos()
// {
//     ps2Perf perf("mySinCos");
//
//     for(int i =0; i < 16; i++)
//     {
//         perf.StartSample();
//
//         float s,c;
//         mySinCos( i, &s , &c );
//
//         perf.EndSample();
//     }
//     perf.PrintStats();
// };
/************************************************************************************/

#define ClocksToMs 294912

inline unsigned int GetCycleCounter(void)
{
    unsigned int ret;
    asm __volatile__ ("mfc0 %0,$9" : "=r" (ret) );
    return ret;
}

// amb nov6/2001 - caution : do not call in middle of the code,
// it'll mess up PDDI stats
inline void ResetCycleCounter(void)
{
    asm __volatile__ ("mtc0 $0,$9 ");
};
   

class ps2Perf
{
public:

    ps2Perf(const char* what)
    {
        strncpy( desc,what,sizeof(desc)-1);
        Reset();
    };

    ~ps2Perf()
    {
        asm __volatile__ ("mtps    $0,0");
    };

    inline void Reset()
    {
        count = 0;
        for(int i = 0; i < PC0_NO_EVENT; i++)
        {
            pc0[i].min = pc1[i].min = 0xffff;
            pc0[i].max = pc1[i].max = 0;
            pc0[i].cur = pc1[i].cur = 0;
            pc0[i].tot = pc1[i].tot = 0;
            pc0[i].num = pc1[i].num = 0;
        }
    }

    inline void BeginSample()
    {
        int evt = count % PC0_NO_EVENT;
        BeginSample(evt);
    };

    inline void BeginSample(int evt)
    {
        pccr.cl0 = 0x8; // only user mode
        pccr.event0 = evt;
        pccr.cl1 = 0x8;
        pccr.event1 = evt;
        pccr.cte = 1;

        int hack = *((int*)(&pccr));

        asm __volatile__("
            .set    noreorder
            .set    noat
            mtps    $0,0    # halt performance counters
            sync.p          # 
            mtpc    $0,0    # set perfcounter 0 to zero
            mtpc    $0,1    # set perfcounter 1 to zero
            sync.p          # 
            mtps    %0,0    # truly - we rule ( well stewart does anyway...)         
            .set    reorder
            .set    at
        "
        : // no output
        : "r"(hack)
        );
    }
    inline void EndSample()
    {
        int evt = count % PC0_NO_EVENT;
        EndSample(evt);
        count++;
    };

    inline void EndSample(int evt)
    {
        register unsigned int    ret_pc0=0;
        register unsigned int    ret_pc1=0;

        asm __volatile__("
            .set    noreorder
            .set    noat
            mfpc    %0,0     
            mfpc    %1,1     
            .set    reorder
            .set    at
        ":"=r"(ret_pc0),"=r"(ret_pc1));                                     

        if(ret_pc0<pc0[evt].min)    pc0[evt].min = ret_pc0;
        if(ret_pc0>pc0[evt].max)    pc0[evt].max = ret_pc0;
        pc0[evt].cur = ret_pc0;
        pc0[evt].tot+= ret_pc0;
        pc0[evt].num++;

        if(ret_pc1<pc1[evt].min)    pc1[evt].min = ret_pc1;
        if(ret_pc1>pc1[evt].max)    pc1[evt].max = ret_pc1;
        pc1[evt].cur = ret_pc1;
        pc1[evt].tot+= ret_pc1;
        pc1[evt].num++;

        asm __volatile__ ("mtps $0,0");
    };

    inline void StopCounters()
    {
        asm __volatile__ ("mtps $0,0");
    }

    void PrintStats()
    {
        #define PRINT_STAT_0(desc,i) if (pc0[i].num) printf(desc "%6d, %6d, %6d, %6d\n",pc0[i].min, pc0[i].max, pc0[i].cur, pc0[i].tot/pc0[i].num);
        #define PRINT_STAT_1(desc,i) if (pc1[i].num) printf(desc "%6d, %6d, %6d, %6d\n",pc1[i].min, pc1[i].max, pc1[i].cur, pc1[i].tot/pc1[i].num);
        
        printf("==== %s ====(total iterations)%d (per event)%d (frame%%) ===========\n",desc,count,(count/PC0_NO_EVENT));
        printf("Event                            , min,    max,    cur,    ave  \n");   

        PRINT_STAT_0("Processor cycle               ,", 1 );
        PRINT_STAT_0("Single instructions issue     ,", 2 );
        PRINT_STAT_0("Branch issued                 ,", 3 );
        PRINT_STAT_0("BTAC miss                     ,", 4 );
        PRINT_STAT_0("ITLB miss                     ,", 5 );
        PRINT_STAT_0("Instruction cache miss        ,", 6 );
        PRINT_STAT_0("DTLB accessed                 ,", 7 );
        PRINT_STAT_0("Non-blocking load             ,", 8 );
        PRINT_STAT_0("WBB single request            ,", 9 );
        PRINT_STAT_0("WBB burst request             ,",10 );
        PRINT_STAT_0("CPU address bus busy          ,",11 );
        PRINT_STAT_0("Instruction completed         ,",12 );
        PRINT_STAT_0("Non-BDS instruction completed ,",13 );
        PRINT_STAT_0("COP2 instruction completed    ,",14 );
        PRINT_STAT_0("Load completed                ,",15 );

        PRINT_STAT_1("Low-order branch issued       ,", 0 );
        PRINT_STAT_1("Processor cycle               ,", 1 );
        PRINT_STAT_1("Dual instructions issue       ,", 2 );
        PRINT_STAT_1("Branch miss-predicted         ,", 3 );
        PRINT_STAT_1("TLB miss                      ,", 4 );
        PRINT_STAT_1("DTLB miss                     ,", 5 );
        PRINT_STAT_1("Data cache miss               ,", 6 );
        PRINT_STAT_1("WBB single request unavailable,", 7 );
        PRINT_STAT_1("WBB burst request unavailable ,", 8 );
        PRINT_STAT_1("WBB burst request almost full ,", 9 );
        PRINT_STAT_1("WBB burst request full        ,",10 );
        PRINT_STAT_1("CPU data bus busy             ,",11 );
        PRINT_STAT_1("Instruction completed         ,",12 );
        PRINT_STAT_1("Non-BDS instruction completed ,",13 );
        PRINT_STAT_1("COP1 instruction completed    ,",14 );
        PRINT_STAT_1("Store completed               ,",15 );

        #undef PRINT_STAT_0
        #undef PRINT_STAT_1

    }
    
private:
    enum PCOUNT0_EVENT    // Performance Counter 0 Events
    {
        PC0_RESERVED            =(0 ),
        PC0_CPU_CYCLE           =(1 ), // Processor cycle
        PC0_SINGLE_ISSUE        =(2 ), // Single instructions issue
        PC0_BRANCH_ISSUED       =(3 ), // Branch issued
        PC0_BTAC_MISS           =(4 ), // BTAC miss
        PC0_ITLB_MISS           =(5 ), // ITLB miss
        PC0_ICACHE_MISS         =(6 ), // Instruction cache miss
        PC0_DTLB_ACCESSED       =(7 ), // DTLB accessed
        PC0_NONBLOCK_LOAD       =(8 ), // Non-blocking load
        PC0_WBB_SINGLE_REQ      =(9 ), // WBB single request
        PC0_WBB_BURST_REQ       =(10), // WBB burst request
        PC0_ADDR_BUS_BUSY       =(11), // CPU address bus busy
        PC0_INST_COMP           =(12), // Instruction completed
        PC0_NON_BDS_COMP        =(13), // Non-BDS instruction completed
        PC0_COP2_COMP           =(14), // COP2 instruction completed
        PC0_LOAD_COMP           =(15), // Load completed
        PC0_NO_EVENT            =(16)  // No event
    };

    enum PCOUNT1_EVENT    // Performance Counter 1 Events
    {
        PC1_LOW_BRANCH_ISSUED   =(0 ), // Low-order branch issued
        PC1_CPU_CYCLE           =(1 ), // Processor cycle
        PC1_DUAL_ISSUE          =(2 ), // Dual instructions issue
        PC1_BRANCH_MISS_PREDICT =(3 ), // Branch miss-predicted
        PC1_TLB_MISS            =(4 ), // TLB miss
        PC1_DTLB_MISS           =(5 ), // DTLB miss
        PC1_DCACHE_MISS         =(6 ), // Data cache miss
        PC1_WBB_SINGLE_UNAVAIL  =(7 ), // WBB single request unavailable
        PC1_WBB_BURST_UNAVAIL   =(8 ), // WBB burst request unavailable
        PC1_WBB_BURST_ALMOST    =(9 ), // WBB burst request almost full
        PC1_WBB_BURST_FULL      =(10), // WBB burst request full
        PC1_DATA_BUS_BUSY       =(11), // CPU data bus busy
        PC1_INST_COMP           =(12), // Instruction completed
        PC1_NON_BDS_COMP        =(13), // Non-BDS instruction completed
        PC1_COP1_COMP           =(14), // COP1 instruction completed
        PC1_STORE_COMP          =(15), // Store completed
        PC1_NO_EVENT            =(16)  // No event
    };

    struct count_t
    {
        unsigned int min,max,tot,cur,num;
    };

    // nov6/2001 amb - see p82 EE Core User's Manual 4.0
    struct pccr_t 
    {
        unsigned pad0:1;   // unused
        unsigned cl0:4;    // events in which mode (eg user/kernel/super/exception) 
        unsigned event0:5; // event to count in counter 0 (see PCOUNT0_EVENT)
        unsigned pad1:1;   // unused
        unsigned cl1:4;    // events in which mode (eg user/kernel/super/exception)  
        unsigned event1:5; // event to count in counter 1 (see PCOUNT1_EVENT)
        unsigned pad2:11;  // unused 
        unsigned cte:1;    // counter enable
    };

    char desc[32];

    pccr_t pccr; // 16 events

    count_t pc0[PC0_NO_EVENT];
    count_t pc1[PC0_NO_EVENT];
    unsigned int   count;
};

#endif // PS2PERF_HPP
