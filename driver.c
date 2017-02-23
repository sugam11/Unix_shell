#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include"queue.h"

void pr_exit (int status,int process_id);
void child(char ** command_tokens);
void op_child (char * file_name, int op,char ** command);
void pipe_child(int p[], char ** command_tokens);
void sig_comm();
static void sig_quit(int signo);
static void sig_int(int signo);


int main(){
    
    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        printf("can't catch SIGQUIT");
    
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("can't catch SIGINT");
    
    
    
    
    Queue * Q;
    Q = createQueue();

    char *copy;
    int counter = 0;
    pid_t pid[50];
    pid_t id;
    int process_no = 0,status;
    const char delim[] = " \t\r\n\v\f";
    while(1){
        short flag = 0;
        sigset_t newmask, oldmask, pendmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGQUIT);
        sigaddset(&newmask, SIGINT);
        
        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
            printf("SIG_BLOCK error");
        
        char input[100];
        gets(input);
        Enqueue(Q,input);
        //printQ(Q);
        
        /* critical section starting*/
        
//Detecting operators
        char op[10][4];                 //Operator 2-d array in order of appearance
        char com[11][50];                   //Commands separated by their operators
        int i = 0,j = 0,k = 0,start = 0,end = 0;        //k is for com counter,j is for delim counter
        while(input[i] != '\0')
        {
            if(input[i] == '<' || input[i] == '>' || input[i] == '|' || input[i]==','){
                end = i;
                if(input[i+1] != '\0' && input[i+1] == '>'){
                    strcpy(op[j++],">>");
                    i++;
                }
                else if(input[i+1] != '\0' && input[i+1] == '|'){
                    if(input[i+2] != '\0' && input[i+2]=='|'){
                        strcpy(op[j++],"|||");
                        i += 2;
                    }
                    else{
                        strcpy(op[j++],"||");
                        i++;
                    }
                }
                else{
                    op[j++][0] = input[i];
                    op[j++][1] = '\0';
                }
        //Because strncpy wasnt working so manually copying all commands in com 2-d array
                int a,b;
                for (a = start, b = 0; a < end; a++,b++) {
                    com[k][b] = input[a];
                    //printf("inside=%c\n",com[k][b]);
                }
                k++;
                start = i + 1;
            }
            else
                flag=1;
            i++;
        }
    //The single command, last command to be copied too !!
        int a,b;
        //printf("%d %d %d\n",end,start,strlen(input));
        for (a = start, b = 0; a < strlen(input); a++,b++) {
            com[k][b] = input[a];
           // printf("This is it%c\n",com[k][b]);
        }
        
//        for(i = 0; i < j; i++){
//            printf("%s\n",op[i]);
//            printf("%s\n",com[i]);
//        }
        
//Operator detection ends
        int op_num = j,op_counter = 0,com_num = k,com_counter = 0;
        
        //      Command tokenization begins
        int token_no = 0;
        char *command_tokens[100];
        command_tokens[token_no]  = strtok(com[com_counter],delim);
        while(command_tokens[token_no] != NULL){
            //printf("this is : %s\n",command_tokens[token_no]);
            token_no++;
            command_tokens[token_no]=strtok(NULL,delim);
        }
        
        //      Command tokenization ends
        
        if(op_num > 0){
            short op1 = 0,op2 = 0,op3 = 0,pipe_count = 0;          //op1 -> |    op2 -> ||   op3-> |||
            
            while (op_counter < op_num) {
        //      Execv calling handler begins
                int p[2];
                pipe(p);
                if(op1 == 1 || op2 == 1 || op3 == 1){
                    if(op1 == 1){
                        op1 = 0;
                        prev_pipe_child(p,4,command_tokens,NULL);
                    }
                    else if(op2 == 1){
                        if(pipe_count == 1){
                            op2 = 0;
                        }
                        pipe_count--;
                        prev_pipe_child(p,4,co)
                    }
                }
                else{
                    if(strcmp(op[op_counter],">") == 0){
                        char *file_name;
                        file_name = strtok(com[com_counter + 1],delim);
                        op_child(file_name,1,command_tokens); // Execvp call
                    }
                    else if(strcmp(op[op_counter],">>") == 0){
                        char *file_name;
                        file_name = strtok(com[com_counter + 1],delim);
                        op_child(file_name,2,command_tokens);  //Execvp call
                    }
                    else if(strcmp(op[op_counter],"<") == 0){
                        char *file_name;
                        file_name = strtok(com[com_counter + 1],delim);
                        op_child(file_name,3,command_tokens);  //Execvp call
                    }
                    else if(strcmp(op[op_counter],"|") == 0){
                        op1 = 1;
                        pipe_child(p,command_tokens);
                    }
                    else if(strcmp(op[op_counter],"||") == 0){
                        op2 = 1;
                        pipe_count = 2;
                        pipe_child(p,command_tokens);
                    }
                    else if(strcmp(op[op_counter],"|||") == 0){
                        op3 = 1;
                        pipe_count = 3;
                        pipe_child(p,command_tokens);
                    }
                }
        //Execv calling handler ends
                
                op_counter++;
                com_counter++;
            }
            
        }
        else if(op_num == 0 && flag == 1){
           // printf("%s\n",com);
            child(command_tokens);
            
        }
        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
            printf("SIG_SETMASK error");
    }
}

void child(char ** command_tokens){
    pid_t id;
    int status;
    if ((id = fork ()) < 0)
        perror ("fork error");
    else if (id == 0)
    {
        printf ("Executing %s\n", command_tokens[0]);
        execvp(command_tokens[0],command_tokens);
        perror ("error in execvp");
    }
    else if (id > 0)
    {
        id = wait (&status);
        pr_exit (status,id);
    }

}
void prev_pipe_child(int p[],int op,char ** command_tokens,char *file_name){

    close(0);
    dup(p[0]);
    close(p[1]);
    if(op == 4){
        pipe_child(p,command_tokens);
    }
    else if(op == 1 || op == 2 || op == 3){
        op_child(file_name,op,command_tokens);
    }
    else{
        child(command_tokens);
    }
}
void pipe_child(int p[], char ** command_tokens){
    pid_t pid,id;
    int status;
    if ((pid = fork ()) < 0)
        perror ("fork error");
    else if (pid == 0)
    {
        printf ("Executing %s\n", command_tokens[0]);
        close(1);
        dup(p[1]);
        close(p[0]);
        execvp(command_tokens[0],command_tokens);
        perror ("error in execvp");
    }
    else if (pid > 0)
    {
        id = wait (&status);
        pr_exit (status,id);
    }
    
}

void op_child(char *file_name,int op, char ** command_tokens){
    pid_t pid,id;
    int status,fd1;
    if ((pid = fork ()) < 0)
        perror ("fork error");
    else if (pid == 0)
        {
            printf ("Executing %s\n", command_tokens[0]);
            if(op == 1){
            close(1);
            fd1 = open(file_name,O_CREAT|O_WRONLY,S_IRWXU);
            }
            else if (op == 2){
                close(1);
                fd1 = open(file_name,O_CREAT|O_APPEND|O_WRONLY,S_IRWXU);
            }
            else if (op == 3){
                close(0);
                fd1 = open(file_name,O_RDONLY);
            }
            execvp(command_tokens[0],command_tokens);
            perror ("error in execvp");
        }
    else if (pid > 0)
        {
            id = wait (&status);
            pr_exit (status,id);
        }

}
//Execvp call ends

void pr_exit (int status,int process_id)
{
    if (WIFEXITED (status))
        printf ("normal termination, process id = %d and exit status = %d\n",process_id, WEXITSTATUS (status));
    else if (WIFSIGNALED (status))
        printf ("abnormal termination, process id = %d and signal number = %d \n", process_id,WTERMSIG (status));
    else if (WIFSTOPPED (status))
        printf ("child stopped, process id = %d and signal number = %d\n",process_id, WSTOPSIG (status));
}

void sig_comm(){
    printf("Do you really want to quit?(Enter 'y' to exit or 'n' to continue\n");
    char c;
    scanf("%c",&c);
    if(c == 'y')
        exit(0);
    else if(c=='n')
        return;
}

static void sig_quit(int signo)
{
    sig_comm();
    return;
}

static void sig_int(int signo)
{
    //printQ(Q);
    return;
}
