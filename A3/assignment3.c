#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

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

int main(){

return 0;
}
