// Michael Harris
// EEL4768 - Summer 2019
// Virtual Memory Simulator v.1.2
// Simulates Translation Lookaside Buffer, Page Table, Frame Table

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define VPAGES 1024 // number of virtual pages
#define TLB_SIZE 8 // TLB size
#define FRAMES 256 // number of frames

// some custom functions
void print_results();
unsigned int rand_addy(int mode);

// TLB related functions
int TLB_lookup(unsigned int vpn);
int get_available_TLB_entry();
unsigned int select_TLB_shootdown_candidate();
void TLB_shootdown(int index);
int cache_translation_in_TLB (unsigned int vpn);

// PT related functions
int get_available_frame();
int search_PageTable_by_VDR(int V, int D, int R);
unsigned int select_page_eviction_candidate();
int page_evict(int vpn);
int cache_page_in_RAM(unsigned int vpn, int read_write);

// set/get related accessor functions
void reset_reference_bits();
void memory_access(unsigned int address, int read_write);

// variables to represent data structures, declaring as global to initialize all values to 0
unsigned int PageTable[VPAGES][4];
unsigned int TLB [TLB_SIZE][5];
unsigned int FrameTable[FRAMES];

// variables to represent statistics
int _TLBhit=0, _TLBmiss=0, _TLBwrite=0, _PThit=0, _PTfault=0, _PTwrite=0, _TLBshootdown=0, _PTeviction=0, _HDread=0, _HDwrite=0;



int main() {
    /// ***** changing "RUN_CASE" below 1-8 will run each test scenario *****
    int loop, loop2, RUN_CASE = 1;
    unsigned int addy;

    // seed random number generator
    srand(time(NULL));
    printf("-- Virtual Memory Simulation --\n");

    //reset_reference_bits(TLB, TLB_SIZE, PageTable, VPAGES);

    /// below, each case study is reproduced from the specification document, and 8 as a custom study.

    if (RUN_CASE == 1) {
        memory_access(0b110011, 1); /*0, 51*/ printf("T:  1,\tAddy: w\t\t51\t\tVPN:0\n");
        memory_access(0b101110110, 0); /*0, 374*/ printf("T:  2,\tAddy: r\t\t374\t\tVPN:0\n");
        memory_access(0b1101001110, 1); /*0, 846*/ printf("T:  3,\tAddy: w\t\t846\t\tVPN:0\n");
        memory_access(0b11011011, 1); /*0, 219*/ printf("T:  4,\tAddy: w\t\t219\t\tVPN:0\n");
        memory_access(0b110110001, 1); /*0,433*/ printf("T:  5,\tAddy: w\t\t433\t\tVPN:0\n");
    }

    else if (RUN_CASE == 2) {
        memory_access(0b10101110, 1); /*174, 178406*/ printf("T:  1,\tAddy: w\t\t178406\t\tVPN:174\n");
        memory_access(0b1101100000100000110, 0); /*432, 442630*/ printf("T:  2,\tAddy: r\t\t442630\t\tVPN:432\n");
        memory_access(0b1111011111111111000, 1); /*495, 507896*/ printf("T:  3,\tAddy: w\t\t507896\t\tVPN:495\n");
        memory_access(0b110011110101111010, 0); /*207, 212346*/ printf("T:  4,\tAddy: r\t\t212346\t\tVPN:207\n");
        memory_access(0b1101011011010001011, 0); /*429 ,439947*/ printf("T:  5,\tAddy: r\t\t439947\t\tVPN:429\n");
    }

    else if (RUN_CASE == 3) {
        memory_access(0b0000001101001101, 0); /*0, 845*/ printf("T:  1,\tAddy: r\t\t845\t\tVPN:0\n");
        memory_access(0b0000011101000001, 1); /*1, 1857*/ printf("T:  2,\tAddy: w\t\t1857\t\tVPN:1\n");
        memory_access(0b0000100000110111, 1); /*2, 2103*/ printf("T:  3,\tAddy: w\t\t2103\t\tVPN:2\n");
        memory_access(0b0000111111110001, 1); /*3, 4081*/ printf("T:  4,\tAddy: w\t\t4081\t\tVPN:3\n");
        memory_access(0b0001001111001100, 1); /*4 ,5068*/ printf("T:  5,\tAddy: w\t\t5068\t\tVPN:4\n");
        memory_access(0b0001010110011111, 1); /*5, 5535*/ printf("T:  6,\tAddy: w\t\t5535\t\tVPN:5\n");
        memory_access(0b0001100011001001, 1); /*6, 6345*/ printf("T:  7,\tAddy: w\t\t6345\t\tVPN:6\n");
        memory_access(0b0001110010001000, 0); /*7, 7304*/ printf("T:  8,\tAddy: r\t\t7304\t\tVPN:7\n");
        memory_access(0b0010001000000000, 1); /*8, 8704*/ printf("T:  9,\tAddy: w\t\t8704\t\tVPN:8\n");
        memory_access(0b0010010001100011, 1); /*9 ,9315*/ printf("T:  10,\tAddy: w\t\t9315\t\tVPN:9\n");
        memory_access(0b0010100010000110, 0); /*10, 10374*/ printf("T:  11,\tAddy: r\t\t10374\t\tVPN:10\n");
        memory_access(0b0010111110000000, 0); /*11, 12160*/ printf("T:  12,\tAddy: r\t\t12160\t\tVPN:11\n");
        memory_access(0b0011001010100001, 1); /*12, 12961*/ printf("T:  13,\tAddy: w\t\t12961\t\tVPN:12\n");
        memory_access(0b0011010011110001, 0); /*13, 13553*/ printf("T:  14,\tAddy: r\t\t13553\t\tVPN:13\n");
        memory_access(0b0011100001000101, 1); /*14 ,14405*/ printf("T:  15,\tAddy: w\t\t14405\t\tVPN:14\n");
        memory_access(0b0011110011011010, 0); /*15, 15578*/ printf("T:  16,\tAddy: r\t\t15578\t\tVPN:15\n");
        memory_access(0b0100001001000000, 1); /*16, 16960*/ printf("T:  17,\tAddy: w\t\t16960\t\tVPN:16\n");
        memory_access(0b0100011010010011, 0); /*17, 18067*/ printf("T:  18,\tAddy: r\t\t18067\t\tVPN:17\n");
        memory_access(0b0100100100101110, 1); /*18, 18734*/ printf("T:  19,\tAddy: w\t\t18734\t\tVPN:18\n");
        memory_access(0b0100111111110110, 0); /*19 ,20470*/ printf("T:  20,\tAddy: r\t\t20470\t\tVPN:19\n");
    }

    else if (RUN_CASE == 4) {

        for (loop=0; loop<500; loop++) {
            memory_access(rand_addy(1), rand() % 2 );
        }
    }

    else if (RUN_CASE == 5) {

        for (loop=0; loop<500; loop++) {
            memory_access(rand_addy(1), 1);
        }
    }

    else if (RUN_CASE == 6) {
        addy = 0;
        for (loop=0; loop<25; loop++) {
            for (loop2=0; loop2<40; loop2++) {
                memory_access(addy | rand_addy(3), rand() % 2 );
            }
            addy = ((addy >> 10) + 1) << 10;
        }
    }

    else if (RUN_CASE == 7) {
        addy = 0;
        for (loop=0; loop<300; loop++) {
            for (loop2=0; loop2<10; loop2++) {
                memory_access(addy | rand_addy(3), rand() % 2 );
            }
            addy = ((addy >> 10) + 1) << 10;
        }
    }

    else if (RUN_CASE == 8) {
    addy = 0;
        for (loop=0; loop<5000; loop++) {
            memory_access(rand_addy(2) | rand_addy(3), rand() % 2 );
        }
    }
    else { printf("\nValid test cases are currently 1-8, change in main()\n"); }

    print_results();
    return 0;
}

void print_results() {
    printf("\n-- Simulation Results: --\n");
    printf("TLB hits: %d\t\t", _TLBhit);
    printf("TLB misses: %d\t\t", _TLBmiss);
    printf("TLB hit rate: %.2lf%%\n", (double) (_TLBhit*100.0 / ( _TLBhit + _TLBmiss)) );
    printf("TLB shootdowns: %d\t", _TLBshootdown);
    printf("TLB writes: %d\n\n", _TLBwrite);

    printf("Pg Table accesses: %d\n", (_PThit + _PTfault));
    printf("Pg Table hits: %d\t", _PThit );
    printf("Pg Table faults: %d\t", _PTfault);
    printf("Pg Table hit rate: %.2lf%%\n", (double) (_PThit*100.0 / (_PThit + _PTfault)) );
    printf("Pg Table evictions: %d\t", _PTeviction);
    printf("Pg Table writes: %d\n\n", _PTwrite);

    printf("Hard disk reads: %d\t", _HDread);
    printf("Hard disk writes: %d\n\n", _HDwrite);

}

//return an address ready for use (bit shifted into position)
//mode 1: return a random 20 bit number
//mode 2: return a random 10 bit number, shifted up 10, rest 0s
//mode 3: return a random 10 bit number, not shifted, rest 0s
unsigned int rand_addy(int mode) {
    unsigned int addy = 0, i = 0;
    if (mode == 1) {
        for (i=0; i<20; i++) {
            addy <<= 1;
            addy |= rand() & 1;
        }
        return addy;
    }
    else if (mode == 2) {
        for (i=0; i<10; i++) {
            addy <<= 1;
            addy |= rand() & 1;
        }
        return addy<<10;
    }
    else if (mode == 3) {
        for (i=0; i<10; i++) {
            addy <<= 1;
            addy |= rand() & 1;
        }
        return addy;
    }
    else return -1;
}

//******************************************************************************
//***********************  TLB  Functions  *************************************
//******************************************************************************

// Returns the TLB entry that corresponds to a virtual page
// Returns -1 if the page's translation is not in the TLB
int TLB_lookup(unsigned int vpn) {
    int i;

    for (i=0; i<TLB_SIZE; i++)
        if ((vpn == TLB[i][3]) && (TLB[i][0] == 1))
            return i;


    return -1;
}

// Returns the index of the first available TLB entry
// Returns -1 if all TLB entries are used
int get_available_TLB_entry() {
    int i;

    for (i=0; i<TLB_SIZE; i++)
        if (TLB[i][0] == 0)
            return i;


    return -1;
}

// Selects the TLB entry that will be evicted
// Pre-condition: All TLB entries are full
// Criteria: Select a random entry with ref.bit=0; if all ref.bit=1, select a random entry
unsigned int select_TLB_shootdown_candidate() {
    int i, flag=0, r = rand() % 8;

    if (TLB[r][2] == 0)
        return r;

    for (i=0; i<TLB_SIZE; i++) {
        if (TLB[i][2] != 1)
            flag++;
    }
    r = rand() % 8;
    if (flag == TLB_SIZE)
        return r;


    return -1;
}

// Perform a TLB shootdown (set V=0, copy D,R bits to the page table)
// Pre-condition: shootdown entry is currently valid
// Parameter index is the TLB entry to shoot down
void TLB_shootdown(int index) {
    PageTable[TLB[index][3]][1] = TLB[index][1];
    PageTable[TLB[index][3]][2] = TLB[index][2];
    PageTable[TLB[index][3]][0] = 1;

    TLB[index][0] = 0;
}

// Copies a translation from the Page Table to the TLB
// The first available TLB entry is used; otherwise, a TLB shootdown is performed
// It copies the D,R bits and the frame number from the page table
// Parameter: virtual page number
// Returns: (+1: shootdown performed)
int cache_translation_in_TLB(unsigned int vpn) {

    int shotdown = 0, temp = get_available_TLB_entry();

    if (temp == -1) {
        temp = select_TLB_shootdown_candidate();
        TLB_shootdown(temp);
        shotdown++;
    }


    TLB[temp][1] = PageTable[vpn][1];
    TLB[temp][2] = PageTable[vpn][2];
    TLB[temp][3] = vpn;
    TLB[temp][4] = PageTable[vpn][3];

    TLB[temp][0] = 1;


    return shotdown;
}




//******************************************************************************
//***********************  Frame Table Functions  *******************************
//******************************************************************************


// Returns the index of the first available frame (in frame table)
// Returns -1 if all frames are allocated
int get_available_frame() {
    int i;

    for (i=0; i<FRAMES; i++) {
        if (FrameTable[i] == 0)
            return i;
    }


    return -1;
}



//******************************************************************************
//***********************  Page Table Functions  *******************************
//******************************************************************************


// Search the PageTable for VDR values passed as parameters
// Return -1 if not found; otherwise, return the index of one such
// randomized entry (using rand function)
// Pre-condition: VDR are 0 or 1
int search_PageTable_by_VDR(int V, int D, int R) {
    int i;

    for (i=0; i<VPAGES; i++) {
        if ((PageTable[i][0] == V) && (PageTable[i][1] == D) && (PageTable[i][2] == R))
            return i;
    }


    return -1;
}

// Selects the virtual page that will be replaced
// Pre-condition: All the frames are allocated
// Criteria: Valid must be 1; choose in order as below
//     VDR.bits: 100   110   101   111
// Between pages with the same category, randomize (using rand)
unsigned int select_page_eviction_candidate() {
    int i, count = 0;
    int access[VPAGES];

    //clear the array for the first time. this will hold indexes, with count holding the size,
    //we will arrange all hits from each collection category, in order, and then randomize
    //amongst them if even a single hit is detected. otherwise, the next condition will run as above
    for (i=0; i<VPAGES;i++) {
        access[i] = 0;
    }

    //     VDR.bits: --->100   110   101   111
    for (i=0; i<VPAGES; i++) {
        if ((PageTable[i][0] == 1) && (PageTable[i][1] == 0) && (PageTable[i][2] == 0))
            access[count++] = i;
        //if any indexes stored, randomize amongst them and return
        if (count > 0) {
            return access[rand() % (count+1)];
        }
    }

    //     VDR.bits: 100   --->110   101   111
    for (i=0; i<VPAGES; i++) {
        if ((PageTable[i][0] == 1) && (PageTable[i][1] == 1) && (PageTable[i][2] == 0))
            access[count++] = i;

        if (count > 0) {
            return access[rand() % (count+1)];
        }
    }

    //     VDR.bits: 100   110   --->101   111
    for (i=0; i<VPAGES; i++) {
        if ((PageTable[i][0] == 1) && (PageTable[i][1] == 0) && (PageTable[i][2] == 1))
            access[count++] = i;

        if (count > 0) {
            return access[rand() % (count+1)];
        }
    }

    //     VDR.bits: 100   110   101   --->111
    for (i=0; i<VPAGES; i++) {
        if ((PageTable[i][0] == 1) && (PageTable[i][1] == 1) && (PageTable[i][2] == 1))
            access[count++] = i;


        return access[rand() % (count+1)];
    }
    //gcc likes to complain
    return 0;
}

// Evict a page from RAM to the hard disk
// If its translation is in the TLB, perform a TLB shootdown
// If the page is dirty, write it to hard disk
// Update the Frame Table and the page table
// Pre-condition: the page is currently allocated in the RAM
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed)
int page_evict(int vpn) {
    int shotdown = 0, hdwrite = 0, temp;

    temp = TLB_lookup(vpn);

    if (temp != -1) {
        TLB_shootdown(temp);
        shotdown++;
    }

    if (PageTable[vpn][1] != 0) {
        hdwrite++;
    }

    FrameTable[PageTable[vpn][3]] = 0;
    PageTable[vpn][0] = 0;



    return shotdown + (10*hdwrite);
}

// Copies a page from the hard disk to the RAM
// Pre-conditions: Page currently not in RAM; page's translation is not in the TLB
// Find a frame for the page; if all the frames are used, performa a page eviction
// Find a TLB entry for the page; if the TLB is full, perform a TLB shootdown
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed) (+100: page eviction performed)
// Parameter read_write: indicates read access or write access
int cache_page_in_RAM(unsigned int vpn, int read_write) {
    int retval = 0, FTidx, temp;

    FTidx = get_available_frame();

    if (FTidx == -1) {
        temp = select_page_eviction_candidate();
        FTidx = PageTable[temp][3];
        retval += page_evict(temp);
        retval += 100;
    }

    FrameTable[FTidx] = 1;

    PageTable[vpn][1] = read_write;
    PageTable[vpn][2] = 1;
    PageTable[vpn][3] = FTidx;
    PageTable[vpn][0] = 1;

    retval += cache_translation_in_TLB(vpn);

    return retval;
}







// Clears the reference bits of the TLB and the Page Table
void reset_reference_bits () {
    int i;

    for (i=0; i<TLB_SIZE; i++)
        TLB[i][2] = 0;

    for (i=0; i<VPAGES; i++)
        PageTable[i][2] = 0;
}

// Simulates a memory access; updates all the data and statistics
// recreation of flowchart
void memory_access(unsigned int address, int read_write) {
    unsigned int vpage = address >> 10;
    unsigned int ppage = address << 22;
    ppage = ppage >> 22;

    int TLB_idx = TLB_lookup(vpage);
    int temp2;

    //TLB hit
    if (TLB_idx > -1) {
        _TLBhit++;
        //update D,R bits
        TLB[TLB_idx][2] = 1;
        if (read_write)
            TLB[TLB_idx][1] = 1;
    }
    //TLB miss
    else {
        _TLBmiss++;

        //PT hit, copy to TLB
        if (PageTable[vpage][0] == 1) {
            _PThit++;

            //TLB full?
            if (get_available_TLB_entry() == -1) {
                _TLBshootdown++;

                //grab a shootdown candidate
                temp2 = select_TLB_shootdown_candidate();

                //before evicting from TLB, copy D, R bits to PT
                PageTable[TLB[temp2][3]][1] = TLB[temp2][1];
                PageTable[TLB[temp2][3]][2] = TLB[temp2][2];

                TLB_shootdown(temp2);

                TLB_idx = temp2;
            }
            //if the tlb isn't full, grab the index of a free position
            else {
                TLB_idx = get_available_TLB_entry();
            }

            //copy translation to TLB. update D, R bits in TLB
            if (read_write)
                TLB[TLB_idx][1] = 1;
            TLB[TLB_idx][2] = 1;
            TLB[TLB_idx][3] = vpage;
            TLB[TLB_idx][4] = ppage;
            TLB[TLB_idx][0] = 1;
            _TLBwrite++;

        }
        //PT fault
        else {
            _PTfault++;
            _HDread++; ///dubious

            //available frame?
            if (get_available_frame() != -1) {
                //copy page to a frame (allocate frame). update FT, PT
                //
                temp2 = get_available_frame();
                FrameTable[temp2] = 1;

                if (read_write)
                    PageTable[vpage][1] = 1;
                PageTable[vpage][2] = 1;
                PageTable[vpage][3] = temp2;
                PageTable[vpage][0] = 1;
                _PTwrite++;




                /// (after above portion,...) ->
                //TLB full?
                if (get_available_TLB_entry() == -1) {
                    _TLBshootdown++;

                    //grab a shootdown candidate
                    temp2 = select_TLB_shootdown_candidate();

                    //before evicting from TLB, copy D, R bits to PT
                    PageTable[TLB[temp2][3]][1] = TLB[temp2][1];
                    PageTable[TLB[temp2][3]][2] = TLB[temp2][2];

                    TLB_shootdown(temp2);

                    TLB_idx = temp2;
                }
                //if the tlb isn't full, grab the index of a free position
                else {
                    TLB_idx = get_available_TLB_entry();
                }

                //copy translation to TLB. update D, R bits in TLB
                if (read_write)
                    TLB[TLB_idx][1] = 1;
                TLB[TLB_idx][2] = 1;
                TLB[TLB_idx][3] = vpage;
                TLB[TLB_idx][4] = ppage;
                TLB[TLB_idx][0] = 1;
                _TLBwrite++;
                ////
            }
            //no available frame, evict a page
            else {
                //evict a page
                //
                temp2 = select_page_eviction_candidate();
                page_evict(temp2);
                _PTeviction++;
                _HDwrite++;


                //translation in TLB?
                if (TLB_lookup(vpage) != -1) {
                    //evict from TLB
                    TLB_shootdown(TLB_lookup(vpage));
                    _TLBshootdown++;
                }
                //copy page to a frame (allocate frame). update FT, PT
                //
                temp2 = get_available_frame();
                FrameTable[temp2] = 1;

                if (read_write)
                    PageTable[vpage][1] = 1;
                PageTable[vpage][2] = 1;
                PageTable[vpage][3] = temp2;
                PageTable[vpage][0] = 1;
                _PTwrite++;

                ///after above..
                //TLB full?
                if (get_available_TLB_entry() == -1) {
                    _TLBshootdown++;

                    //grab a shootdown candidate
                    temp2 = select_TLB_shootdown_candidate();

                    //before evicting from TLB, copy D, R bits to PT
                    PageTable[TLB[temp2][3]][1] = TLB[temp2][1];
                    PageTable[TLB[temp2][3]][2] = TLB[temp2][2];

                    TLB_shootdown(temp2);

                    TLB_idx = temp2;
                }
                //if the tlb isn't full, grab the index of a free position
                else {
                    TLB_idx = get_available_TLB_entry();
                }

                //copy translation to TLB. update D, R bits in TLB
                if (read_write)
                    TLB[TLB_idx][1] = 1;
                TLB[TLB_idx][2] = 1;
                TLB[TLB_idx][3] = vpage;
                TLB[TLB_idx][4] = ppage;
                TLB[TLB_idx][0] = 1;
                _TLBwrite++;
                ////
            }

        }
    }


}
