#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char  *user;

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}
void c_handler(int signum) 
{
    // printf("\n");
    return;
}

void change_dir(char *argset[]){
    
    if(argset[1][0]=='~'){
        char  my_add[MAX_INPUT_SIZE];
	    user= (char *)malloc(sizeof(char)*100);
        strcpy(my_add,"/home/");
        user=getenv("USER");
        strcat(my_add, user);
        strcat(my_add, argset[1]+1);
        printf("%s\n", my_add);
        if(!(chdir(my_add)==0)){
            perror(my_add);
        }
    }
    else{
        if(!(chdir(argset[1])==0)){
            perror(argset[1]);
        }
    }
    
}
void make_dir(char **argset){
    for(int i=1;argset[i]!=NULL;i++){
        if (mkdir(argset[i], 0777) == -1)
            perror("Error in creating new directory ");
    }
}
void remove_dir(char **argset){
    for(int i=1;argset[i]!=NULL;i++){
        if (rmdir(argset[i]) != 0)
            perror("Error in removing directory ");
    }
}

void rm_file(char **argset){
    for(int i=1;argset[i]!=NULL;i++){
        if (unlink(argset[i]) != 0)
            perror("Error in removing file ");
    }
}
void call_ls(char **argset, char *line){
    int status;
    int check_ls;
    pid_t ls_id = fork();

    if(ls_id==0)
    {
        check_ls = execvp(*argset,argset);
        
        if(check_ls < 0)
            perror("Not a valid command");
        exit(1);
    }
    else
    {
        while (wait(&status) != ls_id);
    }
}

void run_cmds(char **tokens, char *line){
    // if()
    char * pch;
    pch = strstr (line," > ");
    char * pch1;
    pch1 = strstr (line," < ");
    char * pch2;
    pch2 = strstr (line," | ");
    char * pch3;
    pch3 = strstr (line," ;; ");
    // Normal Echo 
    if(pch3!=NULL){
        int start = 0;
        char  my_cmd[MAX_INPUT_SIZE];
        char  **my_cmd_tok;
        int flag = 0;
        memset(my_cmd, 0, strlen(my_cmd));
        flag = strlen(my_cmd);
        while(tokens[start]!=NULL){
            if(!strcmp(tokens[start],";;")){
                my_cmd[strlen(my_cmd)] = '\n';
                my_cmd_tok = tokenize(my_cmd);
                printf("%s", my_cmd);
                run_cmds(my_cmd_tok, my_cmd);
                memset(my_cmd, 0, strlen(my_cmd));
            }
            else{
                flag = strlen(my_cmd);
                if(flag){
                    strcat(my_cmd," ");
                    strcat(my_cmd,tokens[start]);
                }
                else{
                    strcpy(my_cmd,tokens[start]);
                }
            }
            start +=1;  
        }
        my_cmd[strlen(my_cmd)] = '\n';
        my_cmd_tok = tokenize(my_cmd);
        run_cmds(my_cmd_tok, my_cmd);
        memset(my_cmd, 0, strlen(my_cmd));
    }
    else{
        if(tokens[0]!=NULL){
            if(!strcmp(tokens[0],"cd")){
                change_dir(tokens);

            }
            else if(!strcmp(tokens[0],"quit")){
                exit(0);
            }
            else{
                int status;
                int check_ls;
                pid_t ls_id = fork();

                if(ls_id==0)
                {
                    if(pch!=NULL){
                        int ii;
                        for(ii=0; tokens[ii]!=NULL; ii++){
                            if(!strcmp(tokens[ii],">")){
                                break;
                            }
                            
                        }
                        int fd1 = creat(tokens[ii+1],0644);
                        if(fd1<0)
                        {
                            perror("Error!! Could not open file");
                            exit(0);
                        }
                        else
                        {
                            dup2(fd1,STDOUT_FILENO); 
                            close(fd1);
                        }
                        int jj;
                        for(jj = ii+2;tokens[jj]!=NULL;jj++)
                        {
                            strcpy(tokens[jj-2],tokens[jj]);
                        }
                        tokens[jj-2] = '\0';
                        for(ii=0; tokens[ii]!=NULL; ii++){
                            printf("%s ", tokens[ii]);
                        }   
                         
                    }
                    if(pch1!=NULL){
                        int ii;
                        for(ii=0; tokens[ii]!=NULL; ii++){
                            if(!strcmp(tokens[ii],"<")){
                                break;
                            }
                        }
                        int fd1 = creat(tokens[ii+1],O_RDONLY);
                        if(fd1<0)
                        {
                            perror("Error!! Could not open file");
                            exit(0);
                        }
                        else
                        {
                            dup2(fd1,STDIN_FILENO); 
                            close(fd1);
                        }
                        int jj;
                        for(jj = ii+2;tokens[jj]!=NULL;jj++)
                        {
                            strcpy(tokens[jj-2],tokens[jj]);
                        }
                        tokens[jj-2] = '\0';
                    }
                    if(pch2!=NULL){
                        int ii;
                        char  left_cmd[MAX_INPUT_SIZE], right_cmd[MAX_INPUT_SIZE];
                        ii = strcspn(line, "|");
                        strncpy ( left_cmd, line,  ii);
                        left_cmd[strlen(left_cmd)] = '\n';
                        strcpy ( right_cmd, line+ii+2);
                        char  **left_token, **right_token;
                        if(left_cmd==NULL || right_cmd==NULL ){
                            printf("invalid cmd\n");
                        }
                        else{
                            left_token = tokenize(left_cmd);
                            right_token = tokenize(right_cmd);
                            int f[2];
                            int check_pipe = pipe(f);
                            if(check_pipe==-1){
                                perror("Pipe failed\n");
                            }
                            pid_t pipe_left, pipe_right;
                            if((pipe_left = fork())<0)
                            {
                                perror("Error!! Fork failed");
                                exit(1);
                            }
                            else if(pipe_left==0)
                            {
                                close(f[0]);  // close reading end of first pipe
                                dup2(f[1], STDOUT_FILENO);
                                close(f[1]);
                                run_cmds(left_token, left_cmd);
                                exit(1);
                            }
                            //Fork for right command execution
                            if((pipe_right = fork())<0)
                            {
                                perror("Error!! Fork failed");
                                exit(1);        
                            }
                            else if(pipe_right==0)
                            {
                                close(f[1]);  // close writing end of first pipe
                                dup2(f[0], STDIN_FILENO);
                                close(f[0]);
                                run_cmds(right_token, right_cmd);
                                exit(1);
                            }
                            close(f[0]);
                            close(f[1]);

                            // Wait for two childs to exit
                            wait(NULL);
                            wait(NULL);

                        }
                        
                        exit(1);
                    }

                    check_ls = execvp(*tokens,tokens);
                    
                    if(check_ls < 0)
                        perror("Not a valid command");
                    exit(1);
                }
                else
                {
                    while (wait(&status) != ls_id);
                }
            }
            
        }

    }
    
    

}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;
    char *wdir = (char *)malloc(sizeof(char)*1000);
    char *for_txt = (char *)malloc(sizeof(char)*1000);
    char *hdir = (char *)malloc(sizeof(char)*1000);
    char *test_dir = (char *)malloc(sizeof(char)*1000);
    getcwd(wdir,1000);
    getcwd(hdir,1000);
    signal(SIGINT, c_handler);
    while (1) {           
       
        printf("Hello>");     
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
   
        //do whatever you want with the commands, here we just print them
        run_cmds(tokens, line);
        
        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
