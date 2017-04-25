#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include"queue.h"

int count=0;
char buf[1000];

void pr_exit (int status,int process_id);
int child (int op1,char * file_name1, int op2, char *file_name2, int op3, char ** command_tokens,int p[],int q[],int r[]);
void sig_comm();
static void sig_quit(int signo);
static void sig_int(int signo);

Queue * Q;

int main(){
    
    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        printf("can't catch SIGQUIT");
    
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("can't catch SIGINT");
    
    Q = createQueue();
    
    char *copy;
    int counter = 0;
    pid_t pid[50];
    pid_t id;
    int process_no = 0,status;
    const char delim[] = " \t\r\n\v\f";
    
    while(1){
        int children = 0;
/*        sigset_t newmask, oldmask, pendmask;*/
/*        sigemptyset(&newmask);*/
/*        sigaddset(&newmask, SIGQUIT);*/
/*        sigaddset(&newmask, SIGINT);*/
/*        */
/*        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)*/
/*            printf("SIG_BLOCK error");*/
        int n=0;
        char input[100];
       // printf("\nbefore gets\n");
        fgets(input,100,stdin);
        input[strlen(input)-1] = '\0';
        //Enqueue(Q,input);
        int p[2],q[2],r[2],s[2];
        pipe(p);
        pipe(q);
        pipe(r);
        pipe(s);
        //printQ(Q);

        /* critical section starting*/
        
//Detecting operators
        char op[10][4];
        memset(op,0,sizeof(op));                //Operator 2-d array in order of appearance
        char com[11][50];    
        memset(com,0,sizeof(com));               //Commands separated by their operators
        int i = 0,j = 0,k = 0,start = 0,end = 0;        //k is for com counter,j is for delim counter
        while(input[i] != '\0' && input[i]!='\n')
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
                	//printf("input=%c\n",input[i]);
                	//printf("%s\n",op);
                    op[j++][0] = input[i];
                    //printf("%s\n",op);
                    op[j][1] = '\0';
                    //printf("%s\n",op);
                }
        //Because strncpy wasnt working so manually copying all commands in com 2-d array
                int a,b;
                for (a = start, b = 0; a < end; a++,b++) {
                    com[k][b] = input[a];
                    //printf("%c",com[k][b]);
                }
               // printf("\n%s\n",com[k]);
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
        
    /*    for(i = 0; i < j; i++){
            printf("op[%d] = %s\n",i,op[i]);
            printf("com[%d] =%s\n",i,com[i]);
        }*/
        
//Operator detection ends
        int op_num = j,op_counter = 0,com_num = k,op_pipe=0,op3=0,pipe_flag=0,char_read=0,char_write,id;//op3->'take input from pipe'
        i=0;
        //      Command tokenization begins
        int token_no = 0;
        char *command_tokens[100];
        memset(command_tokens,0,sizeof(command_tokens));
        command_tokens[token_no]  = strtok(com[0],delim);
       // printf("this is : %s\n",command_tokens[token_no]);
        while(command_tokens[token_no] != NULL){
           //  printf("this is : %s\n",command_tokens[token_no]);
             token_no++;
             command_tokens[token_no]=strtok(NULL,delim);
        }
//      Command tokenization ends
        int flag=0;
        count =0;
        memset(buf,0,sizeof(buf));
        
	while(op_pipe!=-1){
		close(p[0]);
		dup2(q[0],p[0]);
		close(p[1]);
		dup2(q[1],p[1]);
		pipe(q);
		pipe(r);
		if(flag==1){
			op3=1;
			i = op_pipe+1;
			id=fork();
			//printf("\n\nid=%d\n",id);
			if(id==0){
				if(count>1){
					char_read = read(p[0],buf,1000);
            		buf[char_read+1]='\0';
            	}
            	//printf("buf inside if -> %s",buf);
            	char_write = write(r[1],buf,char_read);
            	char_write = write(s[1],buf,char_read);
            	close(r[1]);
            	close(s[1]);
            	//printf("write concluded\n,char_write = %d\n",char_write);
            	exit(0);
            }
            else if(id>0){
            	id = wait (&status);
            	//printf("parent process\n");
            	
            	//printf("buf in parent -> %s",buf);
            	if(count!=1){
            		char_read = read(s[0],buf,1000);
            		//printf("Inside if\n\n\n");
            		close(r[1]);
            		close(p[0]);
					dup2(r[0],p[0]);
					close(p[1]);
					dup2(r[1],p[1]);
					
				}
				count=0;
				//printf("parent executed");
            }
		}
		
        //printf("%d",op_num);
        op_pipe=-1;
        if(op_num==0)
        	children = child(0,NULL,0,NULL,op3,command_tokens,p,q,r);
        else{
        	int j=0,op1=0,op2=0,op1i=0,op2i=0,op_comma=0;			//op1-> '<',op2-> 1-'>',2-'>>',3->'|'
        	while(i<op_num){
        		if(op[i][j]=='<'){
        			op1=1;
        			op1i=i;
        		}
        		else if(op[i][j]=='>'){
        			if(op[i][j+1]=='>')
        				op2=2;
        			else
        				op2=1;
        			op2i=i;
        		}
        		else if(op[i][j]=='|'){
        			op2=3;
        			pipe_flag=1;
        			op_pipe = i;
        			if(op[i][j+1]=='|'){
        				if(op[i][j+2]=='|')
        					count = 3;
        				else
        					count=2;
        			}
        			else
        				count=1;
        			break;
        		}
        		else if(op[i][j]==','){
        			op3=1;
        			op_pipe=i;
        			//printf("\n*****************op_pipe=%d**********\n",op_pipe);
        			break;
        		}
        			
        		i++;
        	}
        	char *file_name1,*file_name2;
        	if(op1==1)
        		file_name1 = strtok(com[op1i+1],delim);
        	else
        		file_name1=NULL;
        		
        	if(op2==1||op2==2)
        		file_name2 = strtok(com[op2i+1],delim);
        	else
        		file_name2 = NULL;
        	
        	//printf("op1=%d,op2=%d,op3=%d,op_pipe=%d,file1=%s,file2=%s\n,count=%d",op1,op2,op3,op_pipe,file_name1,file_name2,count);	
        	children = child(op1,file_name1,op2,file_name2,op3,command_tokens,p,q,r);
        	}      
        //printf("\n\nstr=%s:end of str\n\n",input);
        
/*        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)*/
/*            printf("SIG_SETMASK error");*/
			//      Command tokenization begins
        token_no = 0;
        memset(command_tokens,0,sizeof(command_tokens));
        command_tokens[token_no]  = strtok(com[op_pipe+1],delim);
       // printf("this is : %s\n",command_tokens[token_no]);
        while(command_tokens[token_no] != NULL){
             //printf("this is : %s\n",command_tokens[token_no]);
             token_no++;
             command_tokens[token_no]=strtok(NULL,delim);
        }
//      Command tokenization ends
		flag=1;
	}
	//printf("aaja\n");	
	//printQ(Q);
	//printf("1\n");	
	Enqueue(Q,input,children);
	//printQ(Q);
	//printf("2\n");
    }
}


int child(int op1,char *file_name1,int op2, char *file_name2, int op3, char ** command_tokens,int p[],int q[],int r[]){
    pid_t pid,id;
    int status,fd1;

if(op2 == 1||op2==2){
				printf("Fd of the file : 1, Remapped Fd : 1\n");
				
            }
            else if(op2==3){
				printf("Output pipe Fd : %d, Remapped Fd : 1\n", q[1]);
            }
            
            if (op1 == 1){
				printf("Fd of the file : 0, Remapped Fd : 0\n");
            }
            
            if (op3==1){
            	printf("Input pipe Fd : %d, Remapped Fd : 0\n", p[0]);
            }
	
    if ((pid = fork ()) < 0)
{perror ("fork error");
return 1;}
        //perror ("fork error");
    else if (pid == 0)
        {
           // printf ("Executing %s\n", command_tokens[0]);
            if(op2 == 1){
				//printf("Fd of the file : %d, Remapped Fd : %d\n",pid,pid);
            	close(1);
            	fd1 = open(file_name2,O_CREAT|O_WRONLY,S_IRWXU);
				
            }
            else if (op2 == 2){
                close(1);
                fd1 = open(file_name2,O_CREAT|O_APPEND|O_WRONLY,S_IRWXU);
				//printf("Fd of the file : %d, Remapped Fd : %d\n",fd1,fd1);
            }
            else if(op2==3){
            	close(1);
            	dup(q[1]);
            	if(op3!=1)
            		close(p[0]);
            }
            
            if (op1 == 1){
                close(0);
                fd1 = open(file_name1,O_RDONLY);
				//printf("Fd of the file : %d, Remapped Fd : %d\n",fd1,fd1);
            }
            
            if (op3==1){
            	//printf("op3 entered\n");
            	close(0);
            	dup(p[0]);
            	//printf("op3 loop concluded\n");
            	
            }
            //printf("before execvp\n");
           // printf("command tokens = %s\n",command_tokens[0]);
            execvp(command_tokens[0],command_tokens);
            perror ("error in execvp");
			return 1;
        }
    else if (pid > 0)
        {
            id = wait (&status);
            
            if(op2==3){
            	close(q[1]);
            }
                        
          /*  if (op3==1){
            	close(p[0]);
            }*/
			pr_exit (status,id);
			if (WIFEXITED (status))
        		return 0;
    		else
            	return 1;
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
    printf("\nDo you really want to quit?(Enter 'y' to exit or 'n' to continue)\n");
    char c;
    scanf("%c",&c);
    if(c == 'y'||c=='Y')
        exit(0);
    else if(c=='n'|| c=='N')
        return;
}

static void sig_quit(int signo)
{
    sig_comm();
    return;
}

static void sig_int(int signo)
{
	printQ(Q);
}
