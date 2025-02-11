 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#define SIZE 10

//int sum; //data shared by threads

int amount=0; //account balance
pthread_mutex_t mutex;
sem_t amount_full;
sem_t amount_empty;


void *deposit(void *param)
{
    int i,upper =atoi(param);
    
    // error check for sem_wait
    if(sem_wait(&amount_empty)!=0)
        printf("Error in amount_empty");
    
    printf("Executing deposit function\n");
    if(pthread_mutex_lock(&mutex)!=0)
        printf("Error in mutex_lock");
    amount+=upper;
    if(pthread_mutex_unlock(&mutex)!=0)
        printf("Error in mutex_unlock");
    
    if(sem_post(&amount_full)!=0)
        printf("Error in sem_post");

    printf("Amount after deposit = %d\n",amount);
    pthread_exit(0);
    
}

void *withdraw(void *param)
{
    int i,upper =atoi(param);
    
    if(sem_wait(&amount_full)!=0)
        printf("Error in amount_full");
    
    printf("Executing withdraw function\n");
    if(pthread_mutex_lock(&mutex)!=0)
        printf("Error in mutex_lock");
    amount-=upper;
    if(pthread_mutex_unlock(&mutex)!=0)
        printf("Error in mutex_unlock");
    
    if(sem_post(&amount_empty)!=0)
        printf("Error in sem_post");
    
    printf("Amount after withdraw = %d\n",amount);
    
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_t tid[SIZE]; //thread identifier
    pthread_attr_t attr; // set of thread attributes
    
    //set default attributes of thread
    pthread_attr_init(&attr);
    
    // mutex initlization
    if (pthread_mutex_init(&mutex,NULL)!=0)
    {
        printf("Error in initializing mutex\n");
    }
    
    // amount_full initlization
    if (sem_init(&amount_full,0,0)!=0)
        printf("Error in initializing amount_full\n");
    
    //  amount_empty initlization
    if (sem_init(&amount_empty,0,4)!=0)
        printf("Error in initializing amount_empty\n");
    
    // creating threads for deposit() <7 deposit calls>
    for(int i=0;i<7;i++){
        if(pthread_create(&tid[i], &attr, deposit, argv[1])!=0)
            printf("Error in creating mutex for deposit function\n"); 
    }
    
    // creating thread for withdraw() <3 wtihdraw calls>
    for(int i=7;i<10;i++){
        if(pthread_create(&tid[i], &attr, withdraw, argv[1])!=0)
            printf("Error in creating mutex for withdraw function\n");
    }

    //wait for thread to exit
    for(int i=0; i<SIZE; i++){
       if(pthread_join(tid[i], NULL)!=0)
           printf("Error in waiting for threads to exit\n");
    }
    
    // Destroy the mutex and semaphore before exiting
    pthread_mutex_destroy(&mutex);
    sem_destroy(&amount_full);
    sem_destroy(&amount_empty);
    
    printf("Final amount = %d\n", amount);
}




