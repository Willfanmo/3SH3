#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MAX_LINE 80



char **parse_cmd(char *cmd){

char **args = malloc (MAX_LINE*sizeof(char *));
char *token;
int i =0;

if (cmd[strlen(cmd)-2] == '&')
cmd = strtok(cmd, " &");

token = strtok(cmd, " ");
while (token!=NULL){
//	printf("%s \n", token);
	args[i] = token;
	i++;
	token = strtok(NULL, " ");
}

args[i-1] = strtok(args[i-1], "\n");
args[i]=NULL;
return args;

}

int main(void)
{
char cmd[MAX_LINE];
char **args;

int should_run = 1;
pid_t id;


while(should_run){
	printf("osh>");
	fflush(stdout);

//	scanf("%41s", cmd);
	fgets(cmd, sizeof(cmd), stdin);
//	printf("%s   %d   %c\n", cmd, strlen(cmd), cmd[strlen(cmd)]);
//	printf("check \n %s, %s111\n", args[0], args[1]);
//	args = parse_cmd(cmd);

	if (cmd[strlen(cmd)-2]== '&'){
		args = parse_cmd(cmd);
		id = fork();
		if (id ==0){
			//exs
			printf("\n\n\nconcurrent\n\n\n");
			execvp(args[0], args);
			exit(0);
		}
	}

	else{
		args = parse_cmd(cmd);
		id = fork();
		if (id > 0){

			wait(NULL);
		}
		else{
			printf("\n\n\nchild \n\n\n");
			//exec
//			execv("", cmd);
//			exec(cmd);
			execvp(args[0], args);
			exit(0);
		}
	}

}

return 0;

}
