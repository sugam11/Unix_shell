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
    while(1){
        int j = 0;
        gets(command[command_counter]);
        //printf("%s\n%s\n",command[command_counter],copy);
        
//Detecting operators
        short op1,op2,op3,op4,op5,op6 = 0;                  // '>','>>','<','|','||','|||' operator
        for (j = 0; j < strlen(command[command_counter]); j++) {
            if(command[command_counter][j] == '<'){
                op3 = 1;
                command[command_counter][j] = ' ';
            }
            else if(command[command_counter][j] == '>'){
                op1 = 1;
                command[command_counter][j] = ' ';
                //printf("%d",flag);
                if(command[command_counter][j+1] != '\0' && command[command_counter][j+1] == '>'){
                    op2 = 1;
                    op1 = 0;
                    j++;
                    command[command_counter][j] = ' ';
                    //printf("%d",flag);
                }
            }
            else if(command[command_counter][j] == '|'){
                op4 = 1;
                command[command_counter][j] = ' ';
                //printf("%d",flag);
                if(command[command_counter][j+1] != '\0' && command[command_counter][j+1] == '|'){
                    // Add error detection here if two files separated by comma are not written
                    op5 = 1;
                    op4 = 0;
                    j++;
                    command[command_counter][j] = ' ';
                    //printf("%d",flag);
                }
                else if(command[command_counter][j+1] != '\0' && command[command_counter][j+1] == '|'){
                    op6 = 1;
                    op5 = 0;
                    j++;
                    command[command_counter][j] = ' ';
                    //printf("%d",flag);
                }
            }
        }
        
        //printf("%d %d %d %d %d %d ",op1,op2,op3,op4,op5,op6);
        
//Operator detection ends
        
//Command tokenization begins
        char *command_delim[100];
        int i = 0;
        char *command_tokens[100];
        command_tokens[i]  = strtok(command[command_counter],delim);
        while(command_tokens[i] != NULL){
            //printf("%s\n",command_tokens[i]);
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
