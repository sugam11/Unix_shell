#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

void pr_exit (int status);

int main(){
    char command[10][10],c;
    char command_arg[10][100];
    int command_counter = 0;
    pid_t pid[50];
    pid_t id;
    int process_no = 0,status;
    const char delim[] = " \t\r\n\v\f";
    while(1){
        scanf("%s",command[command_counter]);
        c = getc(stdin);
        
        gets(command_arg[command_counter]);
        printf("%s\n",command_arg[command_counter]);
        //Command tokenization begins
        
//        char *command_tokens[100];
//        int i = 0;
//        command_tokens[i]  = strtok(command,delim);
//        while(command_tokens[i] != NULL){
//            //printf("%s\n",command_tokens[i]);
//            i++;
//            command_tokens[i]=strtok(NULL,delim);
//            
//        }
        //Command tokenization ends
        
        if ((pid[process_no] = fork ()) < 0)
            perror ("fork error");
        else if (pid[process_no] == 0)
        {
            printf ("Executing %s with argument %s \n", command[command_counter],command_arg[command_counter]);
            execvp (command[command_counter],&command_arg[command_counter]);
            perror ("error in execlp");
        }
        else if (pid[process_no] > 0)
        {
            wait (&status);
            pr_exit (status);
        }

    }
}


void pr_exit (int status)
{
    if (WIFEXITED (status))
        printf ("normal termination, exit status = %d\n", WEXITSTATUS (status));
    else if (WIFSIGNALED (status))
        printf ("abnormal termination, signal number = %d \n", WTERMSIG (status));
    else if (WIFSTOPPED (status))
        printf ("child stopped, signal number = %d\n", WSTOPSIG (status));
}
