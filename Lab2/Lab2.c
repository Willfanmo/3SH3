#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum; //data shared by threads
void *runner(void *param); //thread call this function

int main(int argc, char *argv[])
{
    pthread_t tid; //thread identifier
    pthread_attr_t attr; // set of thread attributes

    //set default attributes of thread
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, runner, arv[1]);

    //wait for thread to exit
    pthread_join(tid,NuLL);

    printf("sum = %d\n", sum);
}

void *runner(void *param){
    int i, upper =atoi(param);
    sum = 0;
    for(i=1; i<= upper; i++)
        sum+=i;
    pthread_exit(0);
}