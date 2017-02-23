#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAXSize 1024 
#define HISSize 10

char** ParseCommandLine(char* InputCommand);
char** ParsePipeCommand(char* InputCommand);

int main(){
	int i = 0; //index
	int j = 0; //index
	int k; //index in history

	//internal command variables
	int current = 0;// 'current' mark the index of history array
	int count = 0;
	char** args;//the array for all the tokens in the command line
	char directory[100];//store the pwd directories into the string
	char InputCommand[MAXSize]; 
	char* hist_arr[HISSize]; // the array to store history
	int error = 0;//determine whether the input command exists

	//pipe variables
	pid_t pid[128];
	int pipe_count = 0;// the number of the piped commands
	int pipe_i = 0;//index for the pipes array
	char** pipes;//the array for all piped commands
	int dup2_count = 0;
	int status;

	for(i = 0; i < HISSize; i++)// initialize the history array with NULL
			hist_arr[i] = NULL;

	for(;;){//ifinite loop
		int internal = 0;//decide whether the command is internal or external

		printf("CMPT300@my_shell:~$ ");//title for each command line

		fgets(InputCommand, sizeof(InputCommand), stdin); // store the command line from user to the array
		
		//If the internal command line is "exit", the shell will exit
		if(strcmp(InputCommand, "exit\n") == 0 || feof(stdin)){
			printf("Wow! my_shell exit!\n\n");
			exit(1);
		}

		free(hist_arr[current]); // free the history array

		hist_arr[current] = strdup(InputCommand);
		current = (current +1) % HISSize;//get new entry of the position in history array
		
		int fd[2 * pipe_count];// connect all processes

		//refresh variables
		pipe_count = 0;
		pipe_i = 0;
		error = 0;

		//if the command is piped command
		// count the commands in the piped command
		while(InputCommand[pipe_i] != '\n'){
			if(InputCommand[pipe_i] == '|')
				pipe_count++;
			pipe_i++;
		}
		
		//split the command line into each pipe
		pipes = ParsePipeCommand(InputCommand);
		for(i = 0; i < pipe_count; i++){
			if(pipe(fd + i * 2) < 0){
				printf("***ERROR: pipe creat failed!\n");
				exit(1);
			}
		}

		//refresh the pipe_i
		pipe_i = 0;
		dup2_count = 0;
		j = 0;
		i = 0;
		while(pipe_i <= pipe_count){
			strtok(pipes[pipe_i], "\n");

			args = ParseCommandLine(pipes[pipe_i]); //split the command line into each token

			char* cd_command = pipes[pipe_i];
			char* command = pipes[pipe_i]; // 'command' store the command without \n 
			
			if(strcmp(command, "\n") == 0) internal = 1; //new command line

			if(strcmp(cd_command, "cd") == 0){
				args[1] = strtok(args[1], "\n");
				if(chdir(args[1]) == -1){
					printf("***ERROR: you can not find the file or directory.\n");
					free(hist_arr[current]);
					hist_arr[current] = strdup(pipes[pipe_i]);
					internal = 1;			
				}
				internal = 1;
				break;
			}
			else if(strcmp(command, "pwd") == 0){// internal pwd command
				getcwd(directory, sizeof(directory));
				if(directory != NULL)
					printf("Directory:  %s\n", directory);
				else
					printf("***ERROR: you can not find the directory!\n");
				internal = 1;
				break;
			}
			else if(strcmp(command, "history") == 0){ // internal history command
				k = current;
				int his_count = 1;
				printf("\n");
				do{
					if(hist_arr[k] != NULL && strcmp(hist_arr[k], "\n") != 0)
						printf("%4d %s", his_count++, hist_arr[k]);
					k = (k+1) % HISSize;
				}while (k != current);
				printf("\n");
				internal = 1;
				break;
			}

			pid[pipe_i] = fork();

			if(pid[pipe_i] < 0){
				printf("***ERROR: forking child process failed.\n");
				exit(1);
			}

			if(pid[pipe_i] == 0){//child process
				if(internal == 0 && pipe_count == 0){// execute the external command
					if(execvp(args[0], args) < 0){
						error = 1;
						perror("***ERROR: command failed");
						break;
					}
					exit(1);
				}
				if(pipe_count > 0){// execute the pipes
					if(pipe_i+1 <= pipe_count){
						if(dup2(fd[dup2_count+1], STDOUT_FILENO) < 0){
							perror("***ERROR: dup2 failed");
							break;
						}
					}
					if(dup2_count != 0){
						if(dup2(fd[dup2_count-2], STDIN_FILENO) < 0){
							perror("***ERROR: dup2 failed");
							break;
						}
					}

					for(j = 0; j < 2*pipe_count; j++) close(fd[j]); // close pipe

					if(execvp(args[0], args) < 0){ // execute pipe commands
						perror("***ERROR: command failed");
						break;
					}		
				}
			}
			else{
				if(internal == 1) wait(NULL);
			}
			dup2_count = dup2_count + 2;
			pipe_i++;
		}
		if(internal == 0 && error != 1){
			for(j = 0; j < pipe_count*2; j++)  close(fd[j]);
			for(j = 0; j < pipe_count+1; j++)  while(wait(&status) != pid[j]);
		}
	}
	return 0;
}

char** ParseCommandLine(char* InputCommand){
 	char** arr_tokens = malloc(MAXSize * sizeof(char*)); // allocate the space for each tokens in the array
 	char* token; //each token in each command line
 	token = strtok(InputCommand, " ");
 	int index = 0; //index in arr_tokens array

 	while(token != NULL){
 		arr_tokens[index++] = token;//store each token in array
 		token = strtok(NULL, " ");
 	}
 	arr_tokens[index] = NULL;
 	return arr_tokens;
}

char** ParsePipeCommand(char* InputCommand){
	char** arr_pipe = malloc(MAXSize * sizeof(char*));// allocate the space for each tokens in the array
	char* token;//each pipe in each piped command
	token = strtok(InputCommand, "|");
	int index = 0;//index in arr_pipe array

	while(token != NULL){
		arr_pipe[index++] = token;//store each token in array
		token = strtok(NULL, "|");
	}
	arr_pipe[index] = NULL;
	return arr_pipe;
}

