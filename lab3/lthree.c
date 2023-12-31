#include <stdio.h>
#include <stdlib.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 256
#define ADDRESS_SPACE_SIZE 65536

// Physical memory array
char physicalMemory[ADDRESS_SPACE_SIZE];

// Page table array
char *pageTable[PAGE_TABLE_SIZE];

// Ints to track number of total page faults and total address resolutions
int pageFaults = 0, addressResolutions = 0;

// Array to track free frames
int freeFrames[NUM_FRAMES];

// Structure for a TLB entry
typedef struct
{
    int virtualPageNum;
    char *physicalFrameAddr;
} tlbEntry;

// Structure for the TLB
typedef struct
{
    tlbEntry lookasideBuffer[TLB_SIZE]; // Array of TLB entries
    int currentIndex;                   // Current index for circular buffer
    int miss;
    int hit;
} translationLookasideBuffer;

// Declaration of global variable for TLB struct
translationLookasideBuffer tlb;

/**
 * Handle a page fault.
 * Increments global variable pageFaults upon successfull fetch from backing store.
 *
 * @param pageNumber The page number that caused the fault.
 * @return Pointer to the start of the frame in physical memory.
 */
char *handlePageFault(int pageNumber)
{
    int frameIndex;
    for (frameIndex = 0; frameIndex < NUM_FRAMES; frameIndex++)
    {
        if (freeFrames[frameIndex] == 0)
        {
            freeFrames[frameIndex] = 1;
            break;
        }
    }

    FILE *file = fopen("lab3_data/BACKING_STORE.bin", "rb");
    if (file == NULL)
    {
        perror("Error opening BACKING_STORE.bin, make sure that the backing store bin file is placed correctly in the file structure i should be lab3_data/BACKING_STORE.bin\n");
        exit(1);
    }

    fseek(file, pageNumber * FRAME_SIZE, SEEK_SET);
    fread(&physicalMemory[frameIndex * FRAME_SIZE], sizeof(char), FRAME_SIZE, file);
    fclose(file);
    pageFaults++;

    return &physicalMemory[frameIndex * FRAME_SIZE];
}

/**
 * Checks the TLB for stored pages matching the page number.
 * Records a hit if a matching tlb entry is found and a miss if not.
 *
 * @param pageNumber The page number of a page
 * @return  Pointer to the start of the frame in physical memory.
 */
char *lookAside(int pageNumber)
{
    for (size_t i = 0; i < TLB_SIZE; i++)
    {
        if (tlb.lookasideBuffer[i].virtualPageNum == pageNumber)
        {
            tlb.hit++;
            return tlb.lookasideBuffer[i].physicalFrameAddr;
        }
    }
    tlb.miss++;
    return NULL;
}
/**
 * Updates the TLB using FIFO principal with a circular array structure
 *
 * @param vpn -virtual page number
 * @param pfn -physical page number
 */
void updateTLB(int vpn, char *pfn)
{
    int i = (tlb.currentIndex + 1) % TLB_SIZE;
    tlb.lookasideBuffer[i].physicalFrameAddr = pfn;
    tlb.lookasideBuffer[i].virtualPageNum = vpn;
    tlb.currentIndex = i;
}

/**
 * Initializes the TLB
 *
 */
void initializeTLB()
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb.lookasideBuffer[i].virtualPageNum = -1; // Indicate an invalid or empty entry
        tlb.lookasideBuffer[i].physicalFrameAddr = NULL;
    }
    tlb.currentIndex = 0;
    tlb.miss = 0;
    tlb.hit = 0;
}

/**
 * Get the page from the TLB, page table or handle page fault if not present in either.
 * If page was not found in TLB, update the TLB with the page-frame mapping.
 * Increments global variable addressResolutions each time it is called.
 *
 * @param pageNumber The page number to retrieve.
 * @return Pointer to the requested page in memory.
 */
char *getPage(int pageNumber)
{
    char *page = NULL;
    addressResolutions++;
    page = lookAside(pageNumber);
    if (page != NULL)
        return page;
    page = pageTable[pageNumber];
    if (page == NULL)
    {
        page = handlePageFault(pageNumber);
        pageTable[pageNumber] = page;
    }
    updateTLB(pageNumber, page);
    return page;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("USAGE: input the address file to read from.\n");
        return -1;
    }

    initializeTLB();
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error opening address file");
        return 1;
    }
    int pageNumber, offset, logicalAddress, physicalAddress, value;
    char *frame;

    while (fscanf(file, "%d", &logicalAddress) == 1)
    {
        offset = logicalAddress & 0xFF;
        pageNumber = (logicalAddress >> 8) & 0xFF;
        frame = getPage(pageNumber);
        physicalAddress = frame - physicalMemory + offset;
        value = physicalMemory[physicalAddress];
        printf("Virtual Address: %d, Physical Address: %d, Value: %d\n", logicalAddress, physicalAddress, value);
    }
    double pageFaultRate = (double)pageFaults / (double)addressResolutions * 100.0;
    double tlbHitRate = (double)tlb.hit / (double)addressResolutions * 100.0;
    printf("Page-fault rate: %.2f%%\n", pageFaultRate);
    printf("TLB hit rate: %.2f%%\n", tlbHitRate);

    fclose(file);
    return 0;
}
