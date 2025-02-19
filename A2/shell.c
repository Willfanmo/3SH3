#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int main(void)
{
char cmd[MAX_LINE];


int should_run = 1;
pid_t id;


while(should_run){
	printf("osh>");
	fflush(stdout);

//	scanf("%41s", cmd);
	fgets(cmd, sizeof(cmd), stdin);
	printf("%s   %d   %c\n", cmd, strlen(cmd), cmd[strlen(cmd)]);

	if (cmd[strlen(cmd)-2]== '&'){
		id = fork();
		if (id ==0){
			//exs
			exit(0);
		}
	}

	else{
		id = fork();
		if (id > 0){

			wait(NULL);
		}
		else{
			printf("child \n");
			//exec
//			execv("", cmd);
//			exec(cmd);
			exit(0);
		}
	}

}

return 0;

}
