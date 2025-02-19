#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int main(void)
{
char args[MAX_LINE/2+1];


int should_run = 1;
pid_t id;

while(should_run){
	printf("osh>");
	fflush(stdout);

//	scanf("%41s", args);
	fgets(args, sizeof(args), stdin);
	printf("%s   %d   %c\n", args, strlen(args), args[strlen(args)]);

	if (args[strlen(args)-2]== '&'){
		id = fork();
	}

	else{
		id = fork();
		if (id > 0){

			wait(NULL);
		}
		else{
			printf("child \n");
		}
	}

}

return 0;

}
