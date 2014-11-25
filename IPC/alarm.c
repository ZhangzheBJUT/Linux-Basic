#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static int alarm_fired = 0;

void ding(int sig)
{
	alarm_fired = 1;
}

int main()
{
	pid_t pid;
	printf("alarm application starting\n");

	pid = fork();
	switch(pid)
	{
	case -1:
		perror("fork failed.");
		exit(1);
	case 0:
		sleep(5);
		kill(getppid(),SIGALRM);
		exit(0);
	}

	printf("Waiting for alarm to go off\n");
	(void) signal (SIGALRM,ding);
	pause();

	if (alarm_fired)
		printf("Ding\n");

	printf("done\n");
	exit(0);
}
/*
   通过fork调用启动新的进程，这个子进程休眠5秒后向其父进程发送一个SIGALRM
   信号。父进程在安排号捕获SIGALRM信号后暂停运行，直到接受到一个信号为止。
   这里我们并未在信号处理函数中直接调用printf，而是通过在该函数中设置标志，
   然后在main函数中检查该标志来完成消息的输出。
   
   */
