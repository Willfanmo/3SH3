/*
Compilation:
    gcc -pthread A2.c -o A2
Execution:
    ./A2 n
    where n is the number of student threads
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

/*
 * semaphore is like the number of resource that is availible in the system 
 * if s = 0, meaning no resource is available 
 * wait(s): if s>0 then grab it and decrement s by 1
 * signal(s): increment s by 1 
 */

sem_t sem_ta;
sem_t sem_s;
pthread_mutex_t mutex;
int student_num;
int chair_occ = 0;
int curr_student = 0;

//the ta can only help so many students before lab session is over
//program will end after helping 10 students
int end_labsession = 10;
int lab_running= 1;

void* ta(void *param){
    while(end_labsession >0)
    {
        
        // if sem ==0, student wait
        sem_wait(&sem_s); // TA is available, procecced to the next instruction
        
        if(pthread_mutex_lock(&mutex)!=0)
            printf("Error in mutex_lock");
        
	// track number of free seats
        chair_occ--;
	if (curr_student >0){
		printf("Ta helped student %d previously\n", curr_student);
	}
        printf("TA starts helping a student\n");
        // Assume the time for helping each of the student is from 1-3 sec
        sleep(rand()%3+1);
	end_labsession--;
        
        if(pthread_mutex_unlock(&mutex)!=0)
            printf("Error in mutex_unlock");
        
        sem_post(&sem_ta);
        printf("TA is now available\n");
        
        if (chair_occ ==0){
            printf("TA is taking a nap\n");
            sleep(1);
        }
    }
    lab_running = 0;
    printf("ta is done for the day\n");
    
    for (int i = 0; i < student_num; i++) {
        sem_post(&sem_ta);
    }
    
    pthread_exit(0);
}

void* student(void *param){
    int id = *((int *)param);
    free(param);
    while(1){
        sleep(rand() % 3 + 1); // Programming time
        printf("Student %d is programming\n", id);
        
        if(pthread_mutex_lock(&mutex) != 0)
            printf("Error in mutex_lock\n");
        
        if (!lab_running ||end_labsession<0){
            pthread_mutex_unlock(&mutex);
            printf("Student %d leaves as the lab is over\n",id);
            break;
        }
        
        if(chair_occ < 3){
            chair_occ++;
            sem_post(&sem_s); // Signal TA
            printf("Student %d wants to ask a question\n", id);
            if(pthread_mutex_unlock(&mutex) != 0)
                printf("Error in mutex_unlock\n");
            
            if(!lab_running)
                break;
            sem_wait(&sem_ta); // Wait for TA to finish
	    curr_student = id;
        }
        else{
            printf("Student %d will come back later\n", id);
            if(pthread_mutex_unlock(&mutex) != 0)
                printf("Error in mutex_unlock\n");
        }
    }

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    student_num = atoi(argv[1]);
    printf("Number of student visiting the office hour is: %d\n",student_num);
    
    pthread_t s[student_num];
    pthread_t t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    // mutex initlization
    if (pthread_mutex_init(&mutex,NULL)!=0)
    {
        printf("Error in initializing mutex\n");
    }
    
    // Initilize the value of semaphore of ta
    sem_init(&sem_ta,0,0);
    // Initilize the value of semaphore of students
    sem_init(&sem_s,0,0);
    
    // Create thread for TA 
    if(pthread_create(&t,&attr,ta,NULL)!=0){
        printf("Error in creating thread for TA\n");
        exit(1);
    }
    
    // Create thread for student
    int i=0;
    for (int i = 0; i < student_num; i++) {
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            perror("malloc");
            exit(1);
        }
        *id = i + 1;
        if(pthread_create(&s[i], &attr, student, id) != 0) {
            printf("Error in creating thread for student %d\n", i+1);
            exit(1);
        }
    }
    
    pthread_join(t, NULL);
    for (int i = 0; i < student_num; i++) {
        pthread_join(s[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_ta);
    sem_destroy(&sem_s);
    printf("lab over\n");
    return 0;
}
