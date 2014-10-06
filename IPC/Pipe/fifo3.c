#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define FIFO_NAME  "/tmp/my_fifo"
#define BUFFE_SIZE PIPE_BUF
#define TEN_MEG (1024*1024*10)

int main(int argc,char* argv[])
{
	int pipe_fd;
	int res;
	int open_mode = O_WRONLY;
	int bytes_send = 0;
	char buffer[BUFFE_SIZE+1];

	if (access(FIFO_NAME,F_OK) == -1)
	{
		res = mkfifo(FIFO_NAME,0777);

		if (res !=0)
		{
			fprintf(stderr,"Could not create fifo %s\n",FIFO_NAME);
			exit(EXIT_FAILURE);
		}
	}


	printf("Process %d opening FIFO\n",getpid());
	pipe_fd  = open(FIFO_NAME,open_mode);
	printf("Process %d result %d\n",getpid(),pipe_fd);
	if (pipe_fd != -1)
	{
		while(bytes_send < TEN_MEG)
		{
			res = write(pipe_fd,buffer,BUFFE_SIZE);
			if (res != -1)
			{
				fprintf(stderr,"write error on pipe\n");
				exit(EXIT_FAILURE);
			}
			bytes_send += res;
		}
		(void)close(pipe_fd);
	}
	else
	{
		exit(EXIT_FAILURE);
	}
	printf("Process %d finished\n",getpid());
	exit(EXIT_SUCCESS);
}