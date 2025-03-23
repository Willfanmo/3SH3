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
int physical_memory[PHYS_MEM][PAGE_SIZE];

TLBentry TLB[TLB_SIZE];

signed char *mmapfptr;
int total_addr = 0;
int total_fault = 0;
int total_hit = 0;

int addr_translation(unsigned short va){

int pa;

static int frame_num = 0;

int offset = va & OFFSET_MASK;

pa = frame_num*256+offset;

if(++frame_num ==128){
frame_num = 0;
}

return pa;


}

int main(){

    int va;
    int pa;
    char buffer[BUFFER_SIZE];
    

    FILE *fptr = fopen("addresses_1.txt", "r");
    if (fptr == NULL) {
        printf("Failed to open address file\n");
        return 1;
    }
    
    while(fgets(buffer, BUFFER_SIZE, fptr)!=NULL){
	va = atoi(buffer);
	pa = addr_translation(va);
	total_addr++;
	printf("Virtual address: %d Physical address = %d Value=0\n", va, pa);
}
    printf("Total addresses = %d\nPage_faults = %d\nTLB Hits = %d\n", total_addr, total_fault, total_hit);
    fclose(fptr);
    return 0;

}
