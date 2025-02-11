 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 6

//int sum; //data shared by threads

int amount=0; //account balance
pthread_mutex_t mutex;

void *deposit(void *param)
{
    int i,upper =atoi(param);
    
    pthread_mutex_lock(&mutex);
    amount+=upper;
    pthread_mutex_unlock(&mutex);

    printf("Deposite amount%d\n",amount);
    pthread_exit(0);
    
}

void *withdraw(void *param)
{
    int i,upper =atoi(param);
    
    pthread_mutex_lock(&mutex);
    amount-=upper;
    pthread_mutex_unlock(&mutex);
    
    printf("Withdraw amount%d\n",amount);
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
    
    // creating threads for deposit() <3 deposit calls>
    for(int i=0;i<SIZE/2;i++){
        if(pthread_create(&tid[i], &attr, deposit, argv[1])!=0)
            printf("Error in creating mutex for deposit function\n"); 
    }
    
    // creating thread for withdraw() <3 wtihdraw calls>
    for(int i=SIZE/2;i<SIZE;i++){
        if(pthread_create(&tid[i], &attr, withdraw, argv[2])!=0)
            printf("Error in creating mutex for withdraw function\n");
    }

    //wait for thread to exit
    for(int i=0; i<SIZE; i++){
       if(pthread_join(tid[i], NULL)!=0)
           printf("Error in waiting for threads to exit\n");
    }
    
    
    pthread_mutex_destroy(&mutex);
    printf("Final amount = %d\n", amount);
}




