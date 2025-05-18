////////////////////////////////////////////////////////////////////////////////
// Main File: p3heap.c file is a shared library that does not
//				have a main function.
// This File: p3heap.c
// Other Files: this library is used by the test programs 
//					in the tests directory
// Semester: CS 354 Lecture 03, Fall 2024
// Grade Group: gg19 (See canvas.wisc.edu/groups for your gg#)
// Instructor: Hina Mahmood
//
// Author: Shivani Choudhary
// Email: choudhary26@wisc.edu
// CS Login: shivani
//
/////////////////////////// WORK LOG //////////////////////////////
// Document your work sessions on your copy http://tiny.cc/work-log
// Download and submit a pdf of your work log for each project.
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
// Persons: Identify persons by name, relationship to you, and email.
// Describe in detail the the ideas and help they provided.
//
// Online sources: avoid web searches to solve your problems, but if you do
// search, be sure to include Web URLs and description of
// of any information you find.
//
// AI chats: save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */




/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* alloc(int size) 
{
   	//check for invalid size     
    if(size < 1)
	{
		return NULL;
	}
	
	//check if heap is initialized
	if (heap_start == NULL)
	{
		 return NULL;
	}

	//double-word align size
	int total = size + sizeof(blockHeader);

	if(total % 8 != 0)
	{
		int padding = 8 - (total % 8);
		total += padding;
	}

    //current, the current block being checked
	blockHeader *current = heap_start;

    //runningBest, to keep track of current best fit block, where size of block >= size
	blockHeader *runningBest = NULL;

	//check if there is a block of size equal to the requested size; if so, set p-bit of next block and return
	while(current->size_status != 1) {
		//find the size of block
		int currentSize = current->size_status - current->size_status % 8;
 
		// check if block is free and has EXACT size
		if (current->size_status %2  == 0 && currentSize == total) 
		{
			//update a-bit
			current->size_status = total + (current->size_status % 8) + 1;
			
			//update next block's p bit 
			blockHeader *next = (blockHeader*)((void*)current + current->size_status - current->size_status%8);
		
			if (next->size_status != 1) 
            { 
				next->size_status = next->size_status + 2;
			}

			// return pointer to payload IFF address is a multiple of 8
			int addr = (int)(blockHeader*)((void*)current + sizeof(blockHeader));
			if (addr % 8 != 0) 
            {
				return NULL;
			} 
			return ((void*)current + sizeof(blockHeader));
		}
		
		//use size of the current block to jump to next block
		current = (blockHeader*)((void*)current + current->size_status - current->size_status%8);

	}

	//reset current for re-looping to find the best-fitting block
	current = heap_start;
	int bestSize = 0;
	
	//assuming there is no block equal to the requested size, look for the smallest block of size greater than the requested size
	while(current->size_status != 1) {
		//get size of block
         int currentSize = current->size_status - current->size_status % 8;
             
         //check that this block is 1) free and 2) bigger than the requested size
         if ((current->size_status % 2)  == 0 && currentSize >= total)
		 {
			//check if this block is less than the current optimal block, and assign this block to runningBest if so
			if (bestSize == 0 || currentSize < bestSize) 
            {
				runningBest = current;
				bestSize = runningBest -> size_status - runningBest->size_status % 8;
			}
		 }
	
		//use size of current block to jump to next block
		current = (blockHeader*)((void*)current + current->size_status - current->size_status%8);
	}

	//return NULL if no block of size ≥ to requested size found
	if (runningBest == NULL) 
	{
		return NULL;
	}

	//split block if best block's size exceeds requested size; split only if heap rules are followed
	int pBit = runningBest->size_status % 8;  
	bestSize = runningBest->size_status - pBit;
	int newSize = bestSize - total;
    int newAddr = (int)(blockHeader*)((void*)runningBest + total) + sizeof(blockHeader);

	if (newSize % 8 == 0 && newAddr % 8 == 0) {
		blockHeader *newBlock;
		newBlock = (blockHeader*)((void*)runningBest+total);

		//set p-bit of the new block to show previous block is allocated 
		newBlock->size_status = (bestSize + 2 - total);
		int newBlockSize = bestSize - total;
			
		//create footer for new block
		blockHeader *footer;
		footer = (blockHeader*)((char*)newBlock + newBlockSize - sizeof(blockHeader));
		footer->size_status = newBlockSize;

	}
	//block cannot be split or does not need splitting; set p-bit of next block
	else 
	{ 
		blockHeader *nextBlock = (blockHeader*)((void*)runningBest + 
		runningBest->size_status - runningBest->size_status%8);
		if (nextBlock->size_status != 1)
		{
			nextBlock->size_status = nextBlock->size_status + 2;
		}
	}
	
	//update a-bit of this block to show it is now allocated
	runningBest->size_status = total + pBit + 1;
	
	// return pointer to the payload of best block IFF multiple of 8
	int addr = (int)(blockHeader*)((void*)runningBest + sizeof(blockHeader));
	if (addr % 8 != 0) 
	{
		return NULL;
	}

	return ((void*)runningBest + sizeof(blockHeader));
} 

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int free_block(void *ptr) 
{ 
    //checks for if this block can be freed  

	if(ptr == NULL)
	{
		return -1;
	}

	int blockInt = (int)ptr;
	if(blockInt % 8 != 0)
    {
        return -1;
	
    }

	if((blockHeader*)ptr < heap_start || blockInt > alloc_size)
    {
        return -1;
    }

	blockHeader *block = (blockHeader*)(ptr-4);
	if(block->size_status % 2 == 0)
	{
		return -1;
	}

	int size = block->size_status -block->size_status%8;

    //find p-bit
	int pBit = (block->size_status%8) -1;
	
	//update a-bit to show this block is now free
	block->size_status = block->size_status - 1;      

	//try to coalesce with next block, assuming it's free
        //1) check a-bit of next block. if free:
        //2) add size of next block to the size of this block
        //3) update header with new size and p-bit
	blockHeader* nextBlock = (blockHeader*)((void*)block + size);
	if (nextBlock->size_status%2 == 0 ) 
    {
		int nextSize = nextBlock->size_status - (nextBlock->size_status % 8);
		size = size + nextSize;
		block->size_status = size+pBit; 
	} 

	//try to coalesce with previous block
        //1) find footer
        //2) use footer to find size and find header using size
        //3) use p-bit to see if previous block is free
        //4) coalesce if it is
	if (pBit == 0) 
    {
		//find the footer of the previous block for size information, use to coalesce
		blockHeader* prevFooter = (blockHeader*)((void*)block - sizeof(blockHeader));

		blockHeader* prev = (blockHeader*)((void*)block - (prevFooter->size_status));
		
		int newSize = size + (prev->size_status - (prev->size_status % 8));
		
		block = prev;
	
		//update size to include previous block
		block->size_status = newSize;

		//update p-bit
		block->size_status = block->size_status + (prev->size_status % 8);
	}

	//update size of coalesced block
	size = block->size_status - block->size_status%8;

	//update p-bit of next block to show this block is free
	blockHeader *next = (blockHeader*)((void*)block + size);
	int aBit = next->size_status % 2; 
	int nextPBit;
	// if a bit is 0, p bit = bits
	if (aBit == 0) 
    {
		nextPBit = (next->size_status % 8) % 2;
	} 
    else 
    {
		nextPBit = (next->size_status % 8) - aBit;
	}

	if (nextPBit != 0 && next->size_status != 1)
    {
		next->size_status = next->size_status - 2;
	}	

	//add footer to newly coalesced free block
	blockHeader* footer = (blockHeader*)((char*)next - sizeof(blockHeader));
	footer->size_status = size;
    return 0;
}


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S. 
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout, 
            "********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");
    fprintf(stdout, 
            "********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
            "********************************************************************************\n");
    fflush(stdout);

    return;  
} 



//		p3Heap.c              (FA24)                     
                                       
