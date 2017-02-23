/*     short op1,op2,op3,op4,op5,op6 = 0;                  // '>','>>','<','|','||','|||' operator
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
