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
		printf("The Group ID of child is %d.\n",getpgid(0));
		printf("The Session ID of child is %d.\n",getsid(0));
		sleep(10);
		setsid(); //子进程非组长进程，故其成为新会话首进程，且成为组长进程。该进程id即为会话进程
		printf("Changed:\n");
		printf("The child process PID is %d.\n",getpid());
		printf("The Group ID of child is %d.\n",getpgid(0));
		printf("The Session ID of child is %d.\n",getsid(0));
	    sleep(20);	
		exit(0);
	}

	return 0;
}
