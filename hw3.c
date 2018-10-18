#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void twoArgsHandler(char *args[], int i){
    int argsLength = sizeof(args) / sizeof(char*);
    char* firstArgs[i+1];
    char* secondArgs[argsLength - i];
    
    firstArgs[i] = "/0";
    for(int j = 0; j < i; j++){
        firstArgs[j] = args[j];
    }
    
    for(int k = i+1; k< argsLength; k++){
        secondArgs[k-(i+1)] = args[k];
    }
    //create new array
    
    execvp(firstArgs[0],firstArgs);
    execvp(secondArgs[0],secondArgs);
}

void redirectOutputHandler(char *args[], int i, int fd){
    args[i] = (char *) 0;
    fd = open(args[i + 1], O_RDWR|O_CREAT, S_IWUSR|S_IRGRP| S_IROTH);
    dup2(fd, 1);
    close(fd);
    execvp(args[0], args);

}

void redirectInputHandler(char *args[], int i, int fd){
    args[i] = (char *) 0;
    fd = open(args[i+1], O_RDONLY);
    dup2(fd, 0);
    close(fd);
    execvp(args[0], args);
}



void sigint_handler(int sig){
    write(1, "caught signal: sigint\nCS361 >", sizeof("caught signal: sigint\nCS361 >"));
    //exit(0);
    return;
}
void sigtstp_handler(int sig){
    write(1, "caught signal: sigstp\nCS361 >", sizeof("caught signal: sigstp\nCS361 >"));
    //exit(0);
    return;
}
void forkChild(char *args[], int n){
    int status;
    int fd;
    int pid = fork();
    char *word;
    int i;
    int flag = 0;
    if(pid == 0){
        for(i = 0; i < n; i++){
            if(strncmp(args[i], ";", 2) == 0){
                
                twoArgsHandler(args,i);
//                int argsLength = sizeof(args) / sizeof(char*);
//                char* firstArgs[i+1];
//                char* secondArgs[argsLength - i];
//
//                firstArgs[i] = "/0";
//                for(int j = 0; j < i; j++){
//                    firstArgs[j] = args[j];
//                }
//
//                for(int k = i+1; k< argsLength; k++){
//                    secondArgs[k-(i+1)] = args[k];
//                }
//                //create new array
//
//                execvp(firstArgs[0],firstArgs);
//                execvp(secondArgs[0],secondArgs);
                
                break;
            }else if(strncmp(args[i], ">", 1) == 0){
                flag = 1;
                redirectOutputHandler(args,i,fd);
//                args[i] = (char *) 0;
//                fd = open(args[i + 1], O_RDWR|O_CREAT, S_IWUSR|S_IRGRP| S_IROTH);
//                dup2(fd, 1);
//                close(fd);
//                execvp(args[0], args);
                break;
            }else if(strncmp(args[i], "<", 1) == 0){
                flag = 1;
                redirectInputHandler(args,i,fd);
//                args[i] = (char *) 0;
//                fd = open(args[i+1], O_RDONLY);
//                dup2(fd, 0);
//                close(fd);
//                execvp(args[0], args);
                break;
            }
        }
        
        exit(0);
    }else{
        wait(&status);
        printf("pid:%d", pid);
        printf(" status:%d\n", WEXITSTATUS(status));
        
        //printf("exit: %d\n", status);
        //printf("exit: %d\n", WEXITSTATUS(status));
    }
}
//void freeStuff(char **arr, int n){
//    int i;
//    for(i = 0; i < n; i++){
//        free(arr[i]);
//    }
//    free(arr);
//}
int main(int argc, char **argv) {
    while (1) {
        
        signal(SIGINT, sigint_handler);
        signal(SIGTSTP, sigtstp_handler);
        char line[500];
        char *args[20];
        printf("CS361 >"); //
        fgets(line, 500, stdin);
        char *word = strtok(line, " ");
        int i = 0;
        while(word){
            if(strncmp(word, "exit", 4) == 0 && i == 0){
                exit(0);
            }
            args[i] = (char *) malloc(sizeof(char* )* 100);
            strcpy(args[i], word);
            word = strtok(NULL, " ");
            ++i;
        }
        strtok(args[i-1], "\n");
        args[i] = (char *) malloc(sizeof(char* )* 100);
        args[i] = (char *)0;
        forkChild(args, i);
        //freeStuff(args, i);
    }
    return 0;
}
