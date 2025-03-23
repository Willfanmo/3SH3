#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 10
#define OFFSET_MASK 255 //changed
#define PAGES 256 
#define TLB_SIZE 16
#define PHYS_MEM 128
#define OFFSET_BITS 8 //changed
#define PAGE_SIZE 256 // c

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

/*
bool TLB_search(int page){

    for(int i=0; i<TLB_SIZE; i++){
	if(TLB[i]==page)
		return true;
    }

    return false;
}
*/
int addr_translation(unsigned short va){

    int pa;
    static int frame_num = 0;
    int offset = va & OFFSET_MASK;
    int page = va >> OFFSET_BITS;




    if(page_table[page] !=-1 || page_table[page] == offset){
	pa = page_table[page]*256+offset;
//printf("Frame %d  page_frame %d page %d  offset %d   ", frame_num, page_table[page], page, offset);
    }
    else{
   	pa = frame_num*256+offset;
 	page_table[page] = frame_num;
//printf("Frame %d  page_frame %d page %d  offset %d   ", frame_num, page_table[page], page, offset);


	if(++frame_num ==128){
		frame_num = 0;}
    }
   
return pa;
}

int main(){

    int va;
    int pa;
    char buffer[BUFFER_SIZE];
    
    //initialize empty page table
    for (int i=0; i<PAGES; i++){
	page_table[i] = -1;
    }

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
