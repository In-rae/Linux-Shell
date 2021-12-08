/********
* Project 03: Shell
* COSC 208, Introduction to Computer Systems, Fall 2021
********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "shell> "
#define MAX_BACKGROUND 50

/*
 * Break input string into an array of strings.
 * @param input the string to tokenize
 * @param delimiters the characters that delimite tokens
 * @return the array of strings with the last element of the array set to NULL
 */
char** tokenize(const char *input, const char *delimiters) {        
    char *token = NULL;

    // make a copy of the input string, because strtok
    // likes to mangle strings.  
    char *input_copy = strdup(input);

    // find out exactly how many tokens we have
    int count = 0;
    for (token = strtok(input_copy, delimiters); token; 
            token = strtok(NULL, delimiters)) {
        count++ ;
    }
    free(input_copy);

    input_copy = strdup(input);

    // allocate the array of char *'s, with one additional
    char **array = (char **)malloc(sizeof(char *)*(count+1));
    int i = 0;
    for (token = strtok(input_copy, delimiters); token;
            token = strtok(NULL, delimiters)) {
        array[i] = strdup(token);
        i++;
    }
    array[i] = NULL;
    free(input_copy);
    return array;
}

/*
 * Free all memory used to store an array of tokens.
 * @param tokens the array of tokens to free
 */
void free_tokens(char **tokens) {        
    int i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); // free each string
        i++;
    }
    free(tokens); // then free the array
}

int runProcess(char *path, char ** args, int background){
    int pid = fork();
    if(pid == 0){
        execv(path, args);
        printf("Invalid input\n");
        exit(0);
    } else {
        if(background == 0){
            waitpid(pid, NULL, 0);
        } 
    }
    return pid;
}

int main(int argc, char **argv) {        
    // main loop for the shell
    printf("%s", PROMPT);
    fflush(stdout);  // Display the prompt immediately
    char buffer[1024];
    int pid_max = 10;
    int *pids = malloc(sizeof(int) * pid_max); //keep track of size, use realloc
    int pidCount = 0;
    while (fgets(buffer, 1024, stdin) != NULL) {
        char **command = tokenize(buffer, " \t\n");
        int i = 0;
        char *args[1024];
        char *path;
        int nonBuiltIn = 0;        
        while(command[i] != NULL){//iterate through every string in the input
            if(strcmp(command[0], "exit") == 0){//free memory an make exit system call.
                free_tokens(command);
                free(pids);
                exit(0);
            }
            if(strcmp(command[0], "jobs") == 0){//iterate through pid list, returning appropiate message depending on if its running. Once it finishes, set that index to 0 so it will be skipped over.
                for(int j = 0; j<pidCount; j++){
                    //printf("%d pid\n",pids[i]);
                    if( pids[j] == 0){
                        continue;
                    }
                    int status = waitpid(pids[j], NULL, WNOHANG);                    
                    if( status == 0){
                        printf("%d Running\n", pids[j]);
                    } else if( status > 0){
                        printf("%d Done\n", pids[j]);
                        pids[j] = 0;
                    }
                }
                break;
            } else { //if its not a built in command, add the path and every argument to args. 
                nonBuiltIn = 1;
                if(i == 0){
                    path = command[0];
                } else {
                    args[i] = command[i];
                }
            }
            i++;
        }
        args[0] = path;

        //handle non built in arguments
        if(nonBuiltIn == 1){
            if(strcmp(args[i-1], "&") == 0){//if & is at the end, call runProcess with background flag and add the pid to background pid list
                args[i-1] = NULL;
                int pid = runProcess(path, args, 1);
                if (pid > 0){
                    pids[pidCount] = pid;
                    pidCount += 1;
                }
            } else { //runprocess without background flag
                args[i] = NULL;
                runProcess(path, args, 0);
            }
            
            if (pidCount == pid_max) { //realloc if exceed max pid count
                pid_max *=2;
                pids = realloc(pids ,sizeof(int) * pid_max);
            }
        }
        
        
        // TODO: run commands
        free_tokens(command);
        printf("%s", PROMPT);
        fflush(stdout);  // Display the prompt immediately
    }
    free(pids);
    return EXIT_SUCCESS;
}
