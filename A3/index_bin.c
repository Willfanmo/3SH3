#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
  
  // Load the page from the backing store into the frame
    char buffer[256];
    int page_num = 0;
    FILE *backing_store = fopen("BACKING_STORE_1.bin", "rb");
    fseek(backing_store, page_num * 256, SEEK_SET);
    fread(buffer, sizeof(char), 256, backing_store);
    fclose(backing_store);

    for (int i=0; i<256; i++)
	    printf("buffer %x\n", buffer[i]);

return 0;
}
