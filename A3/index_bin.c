#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define INT_SIZE 16
#define INT_COUNT 16
#define MEMORY_SIZE INT_SIZE*INT_COUNT
int intArray[MEMORY_SIZE];
signed char *mmapfptr;

int main(){
  
  // Load the page from the backing store into the frame
    char buffer[256];
    int page_num = 0;
//    FILE *backing_store = fopen("BACKING_STORE_1.bin", "rb");
  //  fseek(backing_store, page_num * 256, SEEK_SET);
    //fread(buffer, sizeof(char), 256, backing_store);
    //fclose(backing_store);

    int mmapfile_fd = open("BACKING_STORE_1.bin", O_RDONLY);
	//last arg is offset
    mmapfptr = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);

printf("s");    
    for (int i=0; i<MEMORY_SIZE; i++){
//	memcpy(intArray+i, mmapfptr+4*i,INT_SIZE);
	intArray[i ] = mmapfptr[i]; 
//	printf("buffer %x\n", mmapfptr[]);

  }

    munmap(mmapfptr, MEMORY_SIZE);
    for(int i =0; i <MEMORY_SIZE; i++){
	printf("buffer %x\n", intArray[i]);
}

//    for (int i=0; i<256; i++)
//	    printf("buffer %x\n", buffer[i]);

return 0;
}
