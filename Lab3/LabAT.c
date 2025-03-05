#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 10
#define OFFSET_MASK 4095
#define PAGES 8
#define OFFSET_BITS 12
#define PAGE_SIZE 4096


int main() {
    
    char buff[BUFFER_SIZE];
    FILE *fptr = fopen("labaddr.txt", "r");
    int page_table[PAGES] = {6,4,3,7,0,1,2,5};
    
    //Read from labaddr.txt till you read end of file.
    while(fgets(buff, BUFFER_SIZE, fptr) != NULL){
        // Virtual_Addr
        int va =atoi(buff);
        int page_num = va >> OFFSET_BITS;
        int page_offset = va & OFFSET_MASK;
        int frame_number = page_table[page_num];
        int pa = (frame_number<<OFFSET_BITS)|page_offset;
        printf("Virtual addr is %d: Page# = %d & Offset = %d. Physical add= %d\n",va,page_num,page_offset,pa);
        
    }
    fclose(fptr);
    return 0;
}