#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void pr_exit (int status,int process_id);
void child (char * file_name, int op,char ** command);
int main(){
    
    char *copy;
    int counter = 0;
    pid_t pid[50];
    pid_t id;
    int process_no = 0,status;
    const char delim[] = " \t\r\n\v\f";
    while(1){
        char input[100];
        gets(input);
        
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
                    //printf("%c\n",com[k][b]);
                }
                k++;
                start = i + 1;
            }
            i++;
        }
    //The single command, last command to be copied too !!
        int a,b;
        //printf("%d %d %d\n",end,start,strlen(input));
        for (a = start, b = 0; a < strlen(input); a++,b++) {
            com[k][b] = input[a];
            //printf("This is it%c\n",com[k][b]);
        }
        
//        for(i = 0; i < j; i++){
//            printf("%s\n",op[i]);
//            printf("%s\n",com[i]);
//        }
        
//Operator detection ends
        int op_num = j,op_counter = 0,com_num = k,com_counter;
        
        if(op_num > 0){
            while (op_counter < op_num) {
                //      Command tokenization begins
                int token_no = 0;
                char *command_tokens[100];
                command_tokens[token_no]  = strtok(com[com_counter],delim);
                while(command_tokens[token_no] != NULL){
                    //printf("%s\n",command_tokens[i]);
                    token_no++;
                    command_tokens[token_no]=strtok(NULL,delim);
                }
                
                //      Command tokenization ends

                char *file_name;
                file_name = strtok(com[com_counter + 1],delim);
                if(strcmp(op[op_counter],">") == 0){
                    child(file_name,1,command_tokens); // Execvp call
                }
                else if(strcmp(op[op_counter],">>") == 0){
                    child(file_name,2,command_tokens);  //Execvp call
                }
                else if(strcmp(op[op_counter],"<") == 0){
                    child(file_name,3,command_tokens);  //Execvp call
                }
                else if(strcmp(op[op_counter],"|") == 0){
                    
                }
                op_counter++;
            }
            
        }
        
//Execvp call begins


    }
}

void child(char *file_name,int op, char ** command_tokens){
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
