#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_REQUESTS 20
#define DISK_SIZE 300   // cylinders 0 to 299

// Function prototype for sorting (selection sort)
void selection_sort(int arr[], int n);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <initial head position> <LEFT/RIGHT>\n", argv[0]);
        return 1;
    }
    int initial = atoi(argv[1]);
    char *direction = argv[2];
    if(strcmp(direction, "LEFT") != 0 && strcmp(direction, "RIGHT") != 0) {
        fprintf(stderr, "Error: Direction must be LEFT or RIGHT\n");
        return 1;
    }

    // Open the binary file "request.bin" for reading
    FILE *fp = fopen("request.bin", "rb");
    if(fp == NULL) {
        perror("Error opening request.bin");
        return 1;
    }
    int requests[NUM_REQUESTS];
    size_t num_read = fread(requests, sizeof(int), NUM_REQUESTS, fp);
    fclose(fp);
    if(num_read != NUM_REQUESTS) {
        fprintf(stderr, "Error: Could not read %d requests from file.\n", NUM_REQUESTS);
        return 1;
    }

    // Print header info
    printf("Total requests = %d\n", NUM_REQUESTS);
    printf("Initial Head Position: %d\n", initial);
    printf("Direction of Head: %s\n\n", direction);

    int total_movement = 0, current = initial;
    int i;

    /* FCFS ALGORITHM */
    printf("FCFS DISK SCHEDULING ALGORITHM:\n\n");
    total_movement = 0;
    current = initial;
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", requests[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
        total_movement += abs(current - requests[i]);
        current = requests[i];
    }
    printf("\n\nFCFS - Total head movements = %d\n\n", total_movement);

    /* SSTF ALGORITHM */
    printf("SSTF DISK SCHEDULING ALGORITHM \n\n");
    int sstf_order[NUM_REQUESTS];
    int serviced[NUM_REQUESTS] = {0};
    total_movement = 0;
    current = initial;
    for(i = 0; i < NUM_REQUESTS; i++){
        int idx = -1, min_distance = 1e9;
        for(int j = 0; j < NUM_REQUESTS; j++){
            if(!serviced[j]){
                int distance = abs(current - requests[j]);
                if(distance < min_distance) {
                    min_distance = distance;
                    idx = j;
                }
            }
        }
        serviced[idx] = 1;
        sstf_order[i] = requests[idx];
        total_movement += min_distance;
        current = requests[idx];
    }
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", sstf_order[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
    }
    printf("\n\nSSTF - Total head movements = %d\n\n", total_movement);

    /* Prepare a sorted copy for the following algorithms */
    int sorted[NUM_REQUESTS];
    for(i = 0; i < NUM_REQUESTS; i++){
        sorted[i] = requests[i];
    }
    selection_sort(sorted, NUM_REQUESTS);
    // Sorted array now holds the request values in increasing order.

    /* SCAN ALGORITHM */
    printf("SCAN DISK SCHEDULING ALGORITHM \n\n");
    total_movement = 0;
    current = initial;
    int order_SCAN[NUM_REQUESTS], k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        // Partition: left group = all requests <= initial
        int left_count = 0;
        for(i = 0; i < NUM_REQUESTS; i++){
            if(sorted[i] <= initial)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
        // Go to cylinder 0 even if no request there.
        total_movement += current; // current to 0
        current = 0;
        // Then service right group in ascending order.
        for(i = left_count; i < NUM_REQUESTS; i++){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
    } else { // direction RIGHT
        // Partition: right group = all requests >= initial.
        int idx = 0;
        while(idx < NUM_REQUESTS && sorted[idx] < initial)
            idx++;
        int right_start = idx; // right group starts here; left group count = idx.
        // Service right group in ascending order.
        for(i = right_start; i < NUM_REQUESTS; i++){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
        // Go to cylinder (DISK_SIZE - 1) i.e., 299.
        total_movement += (DISK_SIZE - 1 - current);
        current = DISK_SIZE - 1;
        // Then service left group in descending order.
        for(i = idx - 1; i >= 0; i--){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
    }
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", order_SCAN[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
    }
    printf("\n\nSCAN - Total head movements = %d\n\n", total_movement);

    /* C-SCAN ALGORITHM */
    printf("C-SCAN DISK SCHEDULING ALGORITHM \n\n");
    total_movement = 0;
    current = initial;
    int order_CSCAN[NUM_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        // For LEFT: service left group in descending order.
        int left_count = 0;
        for(i = 0; i < NUM_REQUESTS; i++){
            if(sorted[i] <= initial)
                left_count++;
        }
        for(i = left_count - 1; i >= 0; i--){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
        // Go from the last serviced request to cylinder 0.
        total_movement += current;  // from current to 0
        current = 0;
        // Jump from 0 to the extreme right (DISK_SIZE-1)
        total_movement += (DISK_SIZE - 1);
        current = DISK_SIZE - 1;
        // Service right group in descending order.
        for(i = NUM_REQUESTS - 1; i >= left_count; i--){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
    } else { // RIGHT
        // For RIGHT: service right group in ascending order.
        int idx = 0;
        while(idx < NUM_REQUESTS && sorted[idx] < initial)
            idx++;
        int right_start = idx;
        for(i = right_start; i < NUM_REQUESTS; i++){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
        // Go from the last serviced request to cylinder (DISK_SIZE-1)
        total_movement += (DISK_SIZE - 1 - current);
        current = DISK_SIZE - 1;
        // Jump from (DISK_SIZE-1) to 0.
        total_movement += (DISK_SIZE - 1);
        current = 0;
        // Service left group in ascending order.
        for(i = 0; i < right_start; i++){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
    }
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", order_CSCAN[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
    }
    printf("\n\nC-SCAN - Total head movements = %d\n\n", total_movement);

    /* LOOK ALGORITHM */
    printf("LOOK DISK SCHEDULING ALGORITHM \n\n");
    total_movement = 0;
    current = initial;
    int order_LOOK[NUM_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        int left_count = 0;
        for(i = 0; i < NUM_REQUESTS; i++){
            if(sorted[i] <= initial)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
        // If there is a right group, jump directly to its first request.
        if(left_count < NUM_REQUESTS) {
            order_LOOK[k++] = sorted[left_count];
            total_movement += abs(sorted[left_count] - current);
            current = sorted[left_count];
        }
        // Service the remainder of the right group in ascending order.
        for(i = left_count + 1; i < NUM_REQUESTS; i++){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
    } else { // RIGHT
        int idx = 0;
        while(idx < NUM_REQUESTS && sorted[idx] < initial)
            idx++;
        int right_start = idx;
        // Service right group in ascending order.
        for(i = right_start; i < NUM_REQUESTS; i++){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
        // If there is a left group, jump directly to its last request.
        if(right_start > 0) {
            order_LOOK[k++] = sorted[right_start - 1];
            total_movement += abs(current - sorted[right_start - 1]);
            current = sorted[right_start - 1];
        }
        // Service the remainder of the left group in descending order.
        for(i = right_start - 2; i >= 0; i--){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
    }
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", order_LOOK[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
    }
    printf("\n\nLOOK - Total head movements = %d\n\n", total_movement);

    /* C-LOOK ALGORITHM */
    printf("C-LOOK DISK SCHEDULING ALGORITHM \n\n");
    total_movement = 0;
    current = initial;
    int order_CLOOK[NUM_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        int left_count = 0;
        for(i = 0; i < NUM_REQUESTS; i++){
            if(sorted[i] <= initial)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
        // Jump directly from the smallest request in left group to the largest in the right group.
        if(left_count < NUM_REQUESTS) {
            order_CLOOK[k++] = sorted[NUM_REQUESTS - 1];
            total_movement += abs(sorted[NUM_REQUESTS - 1] - current);
            current = sorted[NUM_REQUESTS - 1];
        }
        // Service the rest of the right group in descending order.
        for(i = NUM_REQUESTS - 2; i >= left_count; i--){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(current - sorted[i]);
            current = sorted[i];
        }
    } else { // RIGHT
        int idx = 0;
        while(idx < NUM_REQUESTS && sorted[idx] < initial)
            idx++;
        int right_start = idx;
        // Service right group in ascending order.
        for(i = right_start; i < NUM_REQUESTS; i++){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
        // Jump directly from the largest request in right group to the smallest in left group.
        if(right_start > 0) {
            order_CLOOK[k++] = sorted[0];
            total_movement += abs(current - sorted[0]);
            current = sorted[0];
        }
        // Service the remainder of the left group in ascending order.
        for(i = 1; i < right_start; i++){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current);
            current = sorted[i];
        }
    }
    for(i = 0; i < NUM_REQUESTS; i++){
        printf("%d", order_CLOOK[i]);
        if(i != NUM_REQUESTS - 1)
            printf(", ");
    }
    printf("\n\nC-LOOK - Total head movements = %d\n\n", total_movement);

    return 0;
}

// A simple selection sort to sort the array in increasing order.
void selection_sort(int arr[], int n) {
    int i, j, min_idx;
    for(i = 0; i < n - 1; i++){
        min_idx = i;
        for(j = i + 1; j < n; j++){
            if(arr[j] < arr[min_idx])
                min_idx = j;
        }
        int temp = arr[i];
        arr[i] = arr[min_idx];
        arr[min_idx] = temp;
    }
}
