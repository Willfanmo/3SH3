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
#define PAGES 16 //TLB 16 i think this is right

#define OFFSET_BITS 8 //changed
#define PAGE_SIZE 256 // changed

#define INT_SIZE 4
#define INT_COUNT 10
#define MEMORY_SIZE INT_COUNT*INT_SIZE

int intArray[MEMORY_SIZE];

signed char *mmapfptr;

int main(){

return 0;
}
