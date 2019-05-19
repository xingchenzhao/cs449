//
//  myshell.c
//  proj4
//
//  Created by 赵星辰 on 4/4/19.
//  Xingchen Zhao, XIZ168
//
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void get_line(char* buffer, int size) {// This function is to get user's input.
    fgets(buffer, size, stdin);
    int len = strlen(buffer);
    if(len != 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
}

void myExit(int exit_Code){//This function is to exit program with an exit code.
    exit(exit_Code);
}

char** myStrtok(char *string_to_parse,char **tokens){// This function is to tokenize the input.
    const char* delim = " ";
    char* token = strtok(string_to_parse, delim);
    int i ;
    for(i = 0; token != NULL; i++){
        tokens[i] = token;
        token = strtok(NULL, delim);
    }
    tokens[i] = NULL;
    
    return tokens;
}

void cd_Directory(char* directory_str){// This is function is to change directory.
    if(chdir(directory_str)!=0){
	perror("Error ");
    }
    return;
}

void IO(char** tokens){// This function is to handle redirection.
    int i;
    FILE* file;
    int countOfInput = 0;
    int countOfOutput = 0;
    for(i = 0; tokens[i]!=NULL; i++){
        if(strcmp(tokens[i], ">")==0){
            countOfOutput++;
        }else if(strcmp(tokens[i], "<")==0){
            countOfInput++;
        }
    }
    
    if((countOfInput > 1)||(countOfOutput>1)){
        printf("You cannot try to redirect stdin or stdout more than once!\n");
        exit(errno);
    }
    
    int isFileNull = 0;
    
    for(i = 0; tokens[i]!=NULL; i++){
        if(strcmp(tokens[i], ">")==0){
            file=freopen(tokens[i+1],"w",stdout);
            tokens[i]=NULL;
            if(file == NULL)isFileNull = 1;
        }else if(strcmp(tokens[i], "<")==0){
            file=freopen(tokens[i+1],"r",stdin);
            tokens[i]=NULL;
            if(file == NULL)isFileNull = 1;
        }
    }
    
    if(isFileNull == 1){
        perror("Error ");
        exit(errno);
    }
    
}

void regularPrograms(char** tokens){// This function is to run regular programs.
    if(fork()==0){
        IO(tokens);
        signal(SIGINT, SIG_DFL);
        execvp(tokens[0], tokens);
        perror("Error");
        exit(errno);
    }else{
        int status;
        int childpid = waitpid(-1, &status, 0);
        
        if(childpid == -1){
            perror("waitpid: ");
            exit(EXIT_FAILURE);
        }else if(WIFEXITED(status)){
            if(WEXITSTATUS(status) == 0){
                printf("----------------------\n");
                printf("Exited successfully\n");
            }else{
                printf("----------------------\n");
                printf("Program exited with error code %d\n", WEXITSTATUS(status));
            }
        }else if(WIFSIGNALED(status)){
            printf("----------------------\n");
            printf("Terminated due to signal: %s\n", strsignal(SIGINT));
        }else{
            printf("----------------------\n");
            printf("Terminated some other way!\n");
        }
    }
}

int main(int argc, const char * argv[]) {//main function
    // insert code here...
    
    char input[512];
    int isContinue = 1;
    signal(SIGINT,SIG_IGN);
    
    while (isContinue == 1) {
        printf("myshell> ");
        get_line(input, sizeof(input));
        if((strlen(input)== 0) || (strlen(input)>512)){
            continue;
        }
        
        int len = (int)strlen(input)/2+1;
        char* myTokens[len];
        char** tokens = myStrtok(input,myTokens);
        if(tokens[0] == NULL) continue;
        if(strcmp(tokens[0], "exit") ==0){
            if(tokens[1]!= NULL){
                myExit(atoi(tokens[1]));
            }else{
                myExit(0);
            }
        }else if(strcmp(tokens[0], "cd") == 0){
            if(tokens[1]!= NULL){
                cd_Directory(tokens[1]);
            }
        }else if(strcmp(tokens[0], "pwd")==0){
            char s[100];
            printf("%s\n",getcwd(s, 100));
        }else{
            regularPrograms(tokens);
        }
    }
    
    
    return 0;
}
