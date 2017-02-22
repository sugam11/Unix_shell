#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void pr_exit (int status,int process_id);

int main(){
    char command[10][10];
    char *copy;
    int command_counter = 0;
    pid_t pid[50];
    pid_t id;
    int process_no = 0,status;
    const char delim[] = " \t\r\n\v\f";
    const char redirect_delim[] = ">><";
    
    while(1){
        int j = 0;
        gets(command[command_counter]);
        //printf("%s\n%s\n",command[command_counter],copy);
        
//Detecting redirection operators
        int flag = 0;
        for(j = 0; j < strlen(command[command_counter]); j++){
            if(command[command_counter][j] == '<'){
                flag=1;
                printf("%d",flag);
                break;
            }
            else if(command[command_counter][j] == '>'){
                flag = 2;
                //printf("%d",flag);
                if(command[command_counter][j+1] != NULL && command[command_counter][j+1] == '>'){
                    flag = 3;
                    //printf("%d",flag);
                }
                break;
            }
        }
//Redirection operator detection ends
        
//Command tokenization begins
        char *command_delim[100];
        int i = 0;
        if(flag == 2 || flag == 3 || flag == 1){
            command_delim[i]  = strtok(command,redirect_delim);
            while(command_delim[i] != NULL){
                printf("%s\n",command_delim[i]);
                i++;
                command_delim[i]=strtok(NULL,delim);
            }
        }
        
        char *command_tokens[100];
        i = 0;
        if(flag == 2 || flag == 3 || flag == 1){
            command_tokens[i]  = strtok(command_delim[0],delim);
        }
        else{
            command_tokens[i]  = strtok(command,delim);
        }
        
        while(command_tokens[i] != NULL){
            i++;
            command_tokens[i]=strtok(NULL,delim);
            
        }
//Command tokenization ends
        int fd1;
        if(flag != 0){
            
            char file_name[100];
            i = 1;
            if(strcmp(command_delim[i],">") == 0){
                i++;
            }
            printf("I am :%s\n",command_delim[i]);
            strcpy(file_name,command_delim[i]);
            if(flag == 2){
                fd1 = open(file_name,O_CREAT|O_WRONLY,S_IRWXU);
                printf("FD is  :: %d\n",fd1);
            }
            else if(flag == 1){
                fd1= open(file_name,O_RDONLY);
            }
            else if(flag == 3){
                close(1);
                fd1 = open(file_name,O_APPEND|O_WRONLY|O_CREAT,S_IRWXU);
            }
        }


        
//Execvp call begins
        if ((pid[process_no] = fork ()) < 0)
            perror ("fork error");
        else if (pid[process_no] == 0)
        {
            if(flag != 0){
                if(flag == 2){
                    close(1);
                    dup(fd1);
                    printf("FD is  :: %d\n",fd1);
                }
                else if(flag == 1){
                    close(0);
                    dup(fd1);
                }
                else if(flag == 3){
                    printf("flag=3");
                    close(1);
                    dup(fd1);
                }
            }
            printf ("Executing %s\n", command_tokens[0]);
            execvp(command_tokens[0],command_tokens);
            perror ("error in execvp");
        }
        else if (pid[process_no] > 0)
        {
            id = wait (&status);
            pr_exit (status,id);
        }
//Execvp call ends

    }
}


void pr_exit (int status,int process_id)
{
    if (WIFEXITED (status))
        printf ("normal termination, process id = %d and exit status = %d\n",process_id, WEXITSTATUS (status));
    else if (WIFSIGNALED (status))
        printf ("abnormal termination, process id = %d and signal number = %d \n", process_id,WTERMSIG (status));
    else if (WIFSTOPPED (status))
        printf ("child stopped, process id = %d and signal number = %d\n",process_id, WSTOPSIG (status));
}
