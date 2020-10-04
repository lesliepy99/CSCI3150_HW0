#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int shell_execute(char ** args, int argc)
{
	int child_pid, wait_return, status;
        

	if ( strcmp(args[0], "EXIT") == 0 )
		return -1; 
	
	if( (child_pid = fork()) < 0 ){
		printf("fork() error \n");
	}else if (child_pid == 0 ){
		int count=0;
        int position[2];
        position[0] = position[1] = -1;
        int order = 0;
        for(int i=0;i < argc;i++){
        	if(args[i]!=NULL){

      
        		if(strcmp(args[i],"|")==0){
        			count++;
        			if(order<2){
                        position[order] = i;
                        order++;
        			}
        		}
        	}
	}


	
    if(count==0){
	if ( execvp(args[0], args) < 0){ 
			printf("execvp() error \n");
			exit(-1);
		}
	}
	/*One |*/
	else if (count==1){
        int fd[2];
        int ret;
        char* argv[5];

        pipe(fd);
        
        if((ret=fork())>0){
        	
        	for(int i=0;i<position[0];i++){
        		
        		if(args[i]!=NULL)
        			
        		argv[i] = args[i];
        		
        	}
            argv[position[0]] = NULL;
        	
        	close(1);
         
        	dup(fd[1]); close(fd[0]); close(fd[1]);
            
        	execvp(argv[0],argv);
        }else if(ret==0){
        	
        	close(0);
        	dup(fd[0]);close(fd[0]); close(fd[1]);
            int j =0;
            for(int i=position[0]+1;i<argc;i++){
            	argv[j] = args[i];
            	j++;
            }
            argv[j]=NULL;
            execvp(argv[0],argv);
        }else{
        	printf("Error in fork()\n");
        	exit(-1);
        }
	}

	/*2 |*/
	else if(count==2){
		int fd1[2],fd2[2];
        int ret;
        char* argv[5];

        pipe(fd1);
        if((ret=fork())>0){
        	
        	for(int i=0;i<position[0];i++){
        		
        		if(args[i]!=NULL)
        			
        		argv[i] = args[i];
        		
        	}
            argv[position[0]] = NULL;
        	
        	close(1);
         
        	dup(fd1[1]); close(fd1[0]); close(fd1[1]);
            
        	execvp(argv[0],argv);
        }else if(ret==0){
        	int another_ret;
        	close(0);
        	dup(fd1[0]);close(fd1[0]); close(fd1[1]);

        	pipe(fd2);

        	if((another_ret=fork())>0){
        		close(1);
        		dup(fd2[1]);close(fd2[0]);close(fd2[1]);
                int j =0;
            for(int i=position[0]+1;i<position[1];i++){
            	argv[j] = args[i];
            	j++;
            }
            argv[j]=NULL;
            execvp(argv[0],argv);
        	}else if(another_ret==0){
                close(0);
                dup(fd2[0]);close(fd1[0]); close(fd1[1]);
                int j =0;
            for(int i=position[1]+1;i<argc;i++){
            	argv[j] = args[i];
            	j++;
            }
            argv[j]=NULL;
            execvp(argv[0],argv);
        	}else{
        		printf("Error in inner fork()\n");
        	exit(-1);
        	}

        }else{
        	printf("Error in outter fork()\n");
        	exit(-1);
        }

	}
	}else{
		if ( (wait_return = wait(&status) ) < 0 )
			printf("wait() error \n"); 
	}
			
	return 0;


}
