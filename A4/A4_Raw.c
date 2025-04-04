#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_REQUESTS 20
#define DISK_SIZE 300   

// Put the values in an array in ascending orders 
void num_sort(int arr[], int n) {
    int i, j, num_min;
    for(i = 0; i < n - 1; i++){
        num_min = i;
        for(j = i + 1; j < n; j++){
            if(arr[j] < arr[num_min])
                num_min = j;
        }
        int temp = arr[i];
        arr[i] = arr[num_min];
        arr[num_min] = temp;
    }
}

int main(int argc, char *argv[]) {

    int init_pos = atoi(argv[1]);
    char *direction = argv[2];

    FILE *fp = fopen("request.bin", "rb");
    if(fp == NULL) {
        perror("Error opening request.bin");
        return 1;
    }
    int requests[DISK_REQUESTS];
    size_t num_read = fread(requests, sizeof(int), DISK_REQUESTS, fp);
    fclose(fp);
    if(num_read != DISK_REQUESTS) {
        fprintf(stderr, "Error: Could not read %d requests from file.\n", DISK_REQUESTS);
        return 1;
    }

    printf("Total requests = %d\n", DISK_REQUESTS);
    printf("init_pos Head Position: %d\n", init_pos);
    printf("Direction of Head: %s\n", direction);

    int total_movement = 0;
    int current_pos = init_pos;
    int i;

    /* FCFS */
    printf("FCFS DISK SCHEDULING ALGORITHM:\n");
    total_movement = 0;
    current_pos = init_pos;
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", requests[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
        total_movement += abs(current_pos - requests[i]);
        current_pos = requests[i];
    }
    printf("\nFCFS - Total head movements = %d\n", total_movement);

    /* SSTF ALGORITHM */
    printf("SSTF DISK SCHEDULING ALGORITHM \n");
    int sstf_order[DISK_REQUESTS];
    int serviced[DISK_REQUESTS] = {0};
    total_movement = 0;
    current_pos = init_pos;
    for(i = 0; i < DISK_REQUESTS; i++){
        int idx = -1, min_distance = 1e9;
        for(int j = 0; j < DISK_REQUESTS; j++){
            if(!serviced[j]){
                int distance = abs(current_pos - requests[j]);
                if(distance < min_distance) {
                    min_distance = distance;
                    idx = j;
                }
            }
        }
        serviced[idx] = 1;
        sstf_order[i] = requests[idx];
        total_movement += min_distance;
        current_pos = requests[idx];
    }
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", sstf_order[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
    }
    printf("\nSSTF - Total head movements = %d\n", total_movement);

    /* Prepare a sorted copy for the following algorithms */
    int sorted[DISK_REQUESTS];
    for(i = 0; i < DISK_REQUESTS; i++){
        sorted[i] = requests[i];
    }
    num_sort(sorted, DISK_REQUESTS);
    // Sorted array now holds the request values in increasing order.

    /* SCAN ALGORITHM */
    printf("SCAN DISK SCHEDULING ALGORITHM \n");
    total_movement = 0;
    current_pos = init_pos;
    int order_SCAN[DISK_REQUESTS], k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        // Partition: left group = all requests <= init_pos
        int left_count = 0;
        for(i = 0; i < DISK_REQUESTS; i++){
            if(sorted[i] <= init_pos)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
        // Go to cylinder 0 even if no request there.
        total_movement += current_pos; // current_pos to 0
        current_pos = 0;
        // Then service right group in ascending order.
        for(i = left_count; i < DISK_REQUESTS; i++){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
    } else { // direction RIGHT
        // Partition: right group = all requests >= init_pos.
        int idx = 0;
        while(idx < DISK_REQUESTS && sorted[idx] < init_pos)
            idx++;
        int right_start = idx; // right group starts here; left group count = idx.
        // Service right group in ascending order.
        for(i = right_start; i < DISK_REQUESTS; i++){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
        // Go to cylinder (DISK_SIZE - 1) i.e., 299.
        total_movement += (DISK_SIZE - 1 - current_pos);
        current_pos = DISK_SIZE - 1;
        // Then service left group in descending order.
        for(i = idx - 1; i >= 0; i--){
            order_SCAN[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
    }
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", order_SCAN[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
    }
    printf("\nSCAN - Total head movements = %d\n", total_movement);

    /* C-SCAN ALGORITHM */
    printf("C-SCAN DISK SCHEDULING ALGORITHM \n");
    total_movement = 0;
    current_pos = init_pos;
    int order_CSCAN[DISK_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        // For LEFT: service left group in descending order.
        int left_count = 0;
        for(i = 0; i < DISK_REQUESTS; i++){
            if(sorted[i] <= init_pos)
                left_count++;
        }
        for(i = left_count - 1; i >= 0; i--){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
        // Go from the last serviced request to cylinder 0.
        total_movement += current_pos;  // from current_pos to 0
        current_pos = 0;
        // Jump from 0 to the extreme right (DISK_SIZE-1)
        total_movement += (DISK_SIZE - 1);
        current_pos = DISK_SIZE - 1;
        // Service right group in descending order.
        for(i = DISK_REQUESTS - 1; i >= left_count; i--){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
    } else { // RIGHT
        // For RIGHT: service right group in ascending order.
        int idx = 0;
        while(idx < DISK_REQUESTS && sorted[idx] < init_pos)
            idx++;
        int right_start = idx;
        for(i = right_start; i < DISK_REQUESTS; i++){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
        // Go from the last serviced request to cylinder (DISK_SIZE-1)
        total_movement += (DISK_SIZE - 1 - current_pos);
        current_pos = DISK_SIZE - 1;
        // Jump from (DISK_SIZE-1) to 0.
        total_movement += (DISK_SIZE - 1);
        current_pos = 0;
        // Service left group in ascending order.
        for(i = 0; i < right_start; i++){
            order_CSCAN[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
    }
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", order_CSCAN[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
    }
    printf("\nC-SCAN - Total head movements = %d\n", total_movement);

    /* LOOK ALGORITHM */
    printf("LOOK DISK SCHEDULING ALGORITHM \n");
    total_movement = 0;
    current_pos = init_pos;
    int order_LOOK[DISK_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        int left_count = 0;
        for(i = 0; i < DISK_REQUESTS; i++){
            if(sorted[i] <= init_pos)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
        // If there is a right group, jump directly to its first request.
        if(left_count < DISK_REQUESTS) {
            order_LOOK[k++] = sorted[left_count];
            total_movement += abs(sorted[left_count] - current_pos);
            current_pos = sorted[left_count];
        }
        // Service the remainder of the right group in ascending order.
        for(i = left_count + 1; i < DISK_REQUESTS; i++){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
    } else { // RIGHT
        int idx = 0;
        while(idx < DISK_REQUESTS && sorted[idx] < init_pos)
            idx++;
        int right_start = idx;
        // Service right group in ascending order.
        for(i = right_start; i < DISK_REQUESTS; i++){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
        // If there is a left group, jump directly to its last request.
        if(right_start > 0) {
            order_LOOK[k++] = sorted[right_start - 1];
            total_movement += abs(current_pos - sorted[right_start - 1]);
            current_pos = sorted[right_start - 1];
        }
        // Service the remainder of the left group in descending order.
        for(i = right_start - 2; i >= 0; i--){
            order_LOOK[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
    }
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", order_LOOK[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
    }
    printf("\nLOOK - Total head movements = %d\n", total_movement);

    /* C-LOOK ALGORITHM */
    printf("C-LOOK DISK SCHEDULING ALGORITHM \n");
    total_movement = 0;
    current_pos = init_pos;
    int order_CLOOK[DISK_REQUESTS];
    k = 0;
    if(strcmp(direction, "LEFT") == 0) {
        int left_count = 0;
        for(i = 0; i < DISK_REQUESTS; i++){
            if(sorted[i] <= init_pos)
                left_count++;
        }
        // Service left group in descending order.
        for(i = left_count - 1; i >= 0; i--){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
        // Jump directly from the smallest request in left group to the largest in the right group.
        if(left_count < DISK_REQUESTS) {
            order_CLOOK[k++] = sorted[DISK_REQUESTS - 1];
            total_movement += abs(sorted[DISK_REQUESTS - 1] - current_pos);
            current_pos = sorted[DISK_REQUESTS - 1];
        }
        // Service the rest of the right group in descending order.
        for(i = DISK_REQUESTS - 2; i >= left_count; i--){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(current_pos - sorted[i]);
            current_pos = sorted[i];
        }
    } else { // RIGHT
        int idx = 0;
        while(idx < DISK_REQUESTS && sorted[idx] < init_pos)
            idx++;
        int right_start = idx;
        // Service right group in ascending order.
        for(i = right_start; i < DISK_REQUESTS; i++){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
        // Jump directly from the largest request in right group to the smallest in left group.
        if(right_start > 0) {
            order_CLOOK[k++] = sorted[0];
            total_movement += abs(current_pos - sorted[0]);
            current_pos = sorted[0];
        }
        // Service the remainder of the left group in ascending order.
        for(i = 1; i < right_start; i++){
            order_CLOOK[k++] = sorted[i];
            total_movement += abs(sorted[i] - current_pos);
            current_pos = sorted[i];
        }
    }
    for(i = 0; i < DISK_REQUESTS; i++){
        printf("%d", order_CLOOK[i]);
        if(i != DISK_REQUESTS - 1)
            printf(", ");
    }
    printf("\nC-LOOK - Total head movements = %d\n", total_movement);

    return 0;
}

