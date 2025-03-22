#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
/*
2^16
logical address space = 16

page size = 256B
Page offset = 8
*/

#define BUFFER_SIZE 10
#define OFFSET_MASK 255 //changed
#define PAGES 256 
#define TLB_SIZE 16
#define PHYS_MEM 128

#define OFFSET_BITS 8 //changed
#define PAGE_SIZE 256 // changed

#define INT_SIZE 4
#define INT_COUNT 10
#define MEMORY_SIZE INT_COUNT*INT_SIZE

typedef struct {
	int page_num;
	int frame_num;
} TLBentry;

int page_table[PAGES];
int physical_memory[PHYS_MEM];

TLBentry TLB[TLB_SIZE];

int intArray[MEMORY_SIZE];

signed char *mmapfptr;

//int page_num = logical_address >> 8; // Get page number
//int offset = logical_address & 0xFF; // Get offset within the page


int search_TLB(int page_num) {
    for (int i = 0; i < 16; i++) {
        if (TLB[i].page_num == page_num) {
            return TLB[i].frame_num;
        }
    }
    return -1;  // TLB miss
}

void TLB_Add(int page_num, int frame_num) {
    static int TLB_index = 0; // Keep track of the oldest entry (FIFO)
    TLB[TLB_index].page_num = page_num;
    TLB[TLB_index].frame_num = frame_num;
    TLB_index = (TLB_index + 1) % 16; // Circular buffer
}

int translate_address(int logical_address) {
    int page_num = logical_address >> 8;
    int offset = logical_address & 0xFF;

    // Try to find the page in the TLB
    int frame_num = search_TLB(page_num);
    if (frame_num != -1) {
        return (frame_num << 8) | offset;  // Return physical address
    }

    // TLB miss, check the page table
    frame_num = page_table[page_num];
    if (frame_num != -1) {
        return (frame_num << 8) | offset;  // Return physical address
    }

    // Page fault, handle page fault (load from backing store)
    handle_page_fault(page_num);
    frame_num = page_table[page_num];
    return (frame_num << 8) | offset;  // Return physical address
}


void handle_page_fault(int page_num) {
    static int physical_mem_index = 0;  // FIFO replacement policy

    // Find an empty frame or replace the oldest one
    int frame_num = -1;
    for (int i = 0; i < 128; i++) {
        if (physical_memory[i] == -1) {
            frame_num = i;
            break;
        }
    }

    if (frame_num == -1) {
        frame_num = physical_mem_index;
        physical_mem_index = (physical_mem_index + 1) % 128; // FIFO policy
    }

    // Load the page from the backing store into the frame
    char buffer[256];
    FILE *backing_store = fopen("BACKING_STORE.bin", "rb");
    fseek(backing_store, page_num * 256, SEEK_SET);
    fread(buffer, sizeof(char), 256, backing_store);
    fclose(backing_store);

    memcpy(&physical_memory[frame_num], buffer, 256);

    // Update the page table and TLB
    page_table[page_num] = frame_num;
    TLB_Add(page_num, frame_num);  // Add the new mapping to the TLB
}


int main(){
    FILE *address_file = fopen("addresses_1.txt", "r");
    if (address_file == NULL) {
        perror("Failed to open address file");
        return 1;
    }

    int logical_address;
    while (fscanf(address_file, "%d", &logical_address) != EOF) {
        int physical_address = translate_address(logical_address);
        printf("Logical Address: %d -> Physical Address: %d\n", logical_address, physical_address);
    }

    fclose(address_file);
    return 0;

}
