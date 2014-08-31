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
		exit(-1);
	}
	else if (pid ==0) //子进程
	{
		printf("The child process PID is %d.\n",getpid());
		printf("The Group ID is %d.\n",getpgid(0));
		sleep(5);
		printf("The Group ID of child changed to  %d.\n",getpgid(0));
		exit(0);
	}

	sleep(1);
	setpgid(pid,pid);//改变子进程的组id为子进程本身
	sleep(5);

   	printf("The Parent process PID is %d.\n",getpid());
   	printf("The Parent of parent process PID is %d.\n",getppid());
	printf("The Group ID of parent is %d.\n",getpgid(0));
	setpgid(getpid(),getppid()); //改变父进程的组id为父进程的父进程
	printf("The Group ID of parent  changed to  %d.\n",getpgid(0));
		
	return 0;
}
