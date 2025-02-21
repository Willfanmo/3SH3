#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

int num_students = 0;
pthread_mutex_t mutex;
sem_t help;
sem_t line;
int wait_list [3];

void help_student(int param){

	printf("Student %d got help", param);
	// sleep for time
}

void student_line(int param){

	printf("Student %d lined up", param):
}

void *student(int param){

	/*wait semaphore for  line up
	add student number to list
	if (student in list){
	wait semaphore for help
	give  semaphore for list up
	help student func

	give help semaphore	
	}
	else {wait}
*/

}

int main(int argc, char *argv[]){
	
	num_students = atoi(argv[1]);
	printf("%d", num_students);	
}
