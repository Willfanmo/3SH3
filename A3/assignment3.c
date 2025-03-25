/*
Compilation
	gcc assignment3.c -o A3
Execution
	./A3
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//define constants
#define PAGE_SIZE 256
#define NUM_PAGES 256      // 2^16 / 256 
#define NUM_FRAMES 128     // 2^15 / 256
#define TLB_SIZE 16
#define OFFSET_MASK 255
#define OFFSET_BITS 8
#define BUFFER_SIZE 32

typedef struct {
    int page;
    int frame;
} TLBEntry;

TLBEntry tlb[TLB_SIZE];
int tlb_index = 0;                                      // Index_num for tlb initialized to 0
int page_table[NUM_PAGES];                              // Initialize the page_table  
signed char physical_memory[NUM_FRAMES * PAGE_SIZE];
int next_frame = 0;                                     // Next available frame (or FIFO replacement index)

// Translates a logical address to a physical address, updating TLB hits and page fault counters.
int translate_address(int logical_address, char *backing_store, int *tlb_hits, int *page_faults) {
    int offset = logical_address & OFFSET_MASK;         // Lower 8 bits as offset
    int page = logical_address >> OFFSET_BITS;     // Upper 8 bits as page number
    int frame = search_TLB(page);

    if (frame != -1) {
        (*tlb_hits)++;
    }
    else {
        frame = page_table[page];
        if (frame == -1) {
            (*page_faults)++;
            frame = handle_page_fault(page, backing_store);
        }
        TLB_Add(page, frame);
    }
    return frame * PAGE_SIZE + offset;
}

// Searches the frame_num for a given page_num, returns -1 if not found
int search_TLB(int page_num) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == page_num) {
            return tlb[i].frame;
        }
    }
    return -1;
}

// Adds an entry to the TLB
void TLB_Add(int page_num, int frame_num) {
    tlb[tlb_index].page = page_num;
    tlb[tlb_index].frame = frame_num;
    tlb_index = (tlb_index + 1) % TLB_SIZE;
}

// Updates the frame_num for a given page_num
void TLB_Update(int page_num, int new_frame) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == page_num) {
            tlb[i].frame = new_frame;
            break;
        }
    }
}

int handle_page_fault(int page, char *backing_store) {
    int frame = next_frame;
    signed char *dest_ptr = physical_memory + frame * PAGE_SIZE;  
    char *src_ptr = backing_store + page * PAGE_SIZE;             
    memcpy(dest_ptr, src_ptr, PAGE_SIZE);                         
    for (int i = 0; i < NUM_PAGES; i++) {
        if (page_table[i] == frame) {
            page_table[i] = -1;
            break;
        }
    }

    page_table[page] = frame;
    TLB_Update(page, frame);
    next_frame = (next_frame + 1) % NUM_FRAMES;

    return frame;
}


int main() {
    int tlb_hits = 0;
    int page_faults = 0;
    int total_addresses = 0;

    // Initialize tables to empty, denoted by -1
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i] = -1;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
        
    // Open BACKING_STORE.bin and map it to memory
    int fd = open("BACKING_STORE_1.bin", O_RDONLY);
    if (fd < 0) {
        perror("Error opening BACKING_STORE_1.bin");
        exit(EXIT_FAILURE);
    }

    char *backing_store = mmap(0, NUM_PAGES * PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);

    if (backing_store == MAP_FAILED) {
        perror("Error with mmap");
        exit(EXIT_FAILURE);
    }
    
    FILE *fptr = fopen("addresses_1.txt", "r");
    char buff[BUFFER_SIZE];
   
    while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
        int va = atoi(buff); 
        int page_num = va >> OFFSET_BITS; 
        int page_offset = va & OFFSET_MASK; 
        int pa = translate_address(va, backing_store, &tlb_hits, &page_faults); 
        signed char value = physical_memory[pa];
        printf("Virtual address: %d Physical address = %d Value= %d\n", va, pa, value);
        total_addresses++;
    }
    
    printf("Total addresses = %d\n", total_addresses);
    printf("Page_faults = %d\n", page_faults);
    printf("TLB Hits = %d\n", tlb_hits);
    
    fclose(fptr);
    munmap(backing_store, NUM_PAGES * PAGE_SIZE);
    close(fd);
    
    return 0;
}
