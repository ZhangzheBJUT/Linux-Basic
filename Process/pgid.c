#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc,char**argv)
{
	pid_t pid;
	
	pid = fork();
	if (pid < 0)
	{
		printf ("fork error");
	}
	else if (pid ==0) //子进程
	{
		printf("The child process PID is %d.\n",getpid());
		printf("The Group ID is %d.\n",getpgrp());
		printf("The Group ID is %d.\n",getpgid(0));
		printf("The Group ID is %d.\n",getpgid(getpid()));
		exit(0);
	}

	sleep(3);
   	printf("The Parent process PID is %d.\n",getpid());
	printf("The Group ID is %d.\n",getpgrp());
		
	return 0;
}
