#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{
	int data_processed;
	int file_pipes[2];
	const char some_data[] = "1233";
            pid_t fork_result;


	if (pipe(file_pipes)==0)
	{
		fork_result  = fork();

		if (fork_result ==(pid_t)-1)
		{
			fprintf(stderr,"Fork failed");
			exit(EXIT_FAILURE);
		}

		if (fork_result == (pid_t)0)
		{
			close(0); //close the stdin
			dup(file_pipes[0]);//pip read node ==> stdin
			close(file_pipes[0]);
			close(file_pipes[1]);

			execlp("od","od","-c",(char*)0);
		}
		else
		{
			close(file_pipes[0]); //close pip read note
			data_processed = write(file_pipes[1],some_data,strlen(some_data));
			close(file_pipes[1]);
			printf("%d -Wrote %d bytes \n",getpid(),data_processed);
		}
		
	}
	exit(EXIT_SUCCESS);
}
