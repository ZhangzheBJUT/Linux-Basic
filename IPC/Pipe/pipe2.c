#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{
	int data_processed;
	int file_pipes[2];

	const char some_data[] = "1233";
	char buffer[BUFSIZ+1];
    pid_t fork_result;


	memset(buffer,'\0',sizeof(buffer));

	if (pipe(file_pipes)==0)
	{
		fork_result  = fork();

		if (fork_result ==-1)
		{
			fprintf(stderr,"Fork failed");
			exit(EXIT_FAILURE);
		}
		
		if (fork_result ==0)
		{
			close(file_pipes[1]);
			sleep(2);
			data_processed = read(file_pipes[0],buffer,BUFSIZ);
			printf("Read %d bytes :%s \n",data_processed,buffer);
			

		}
		else
		{
			close(file_pipes[0]);
			data_processed = write(file_pipes[1],some_data,strlen(some_data));
			printf("Wrote %d bytes \n",data_processed);
		}
		
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
}
/*
程序首先用pipe调用创建一个管道，接着用fork调用创建一个新进程。如果fork调用成功，父进程就写数据到管道中，而子进程从管道中读取数据。
父子进程都在只调用了一次write或read之后就退出。
如果父进程在子进程之前退出，你就会在两部分输出之间看到shell提示符。
 */
