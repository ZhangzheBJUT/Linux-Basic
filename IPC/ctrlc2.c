#include <signal.h>
#include <stdio.h>
#include <unistd.h>


void ouch(int sig)
{
	printf("OUCH! - I got signal %d\n",sig);
}

int main()
{
	struct sigaction act;

	act.sa_handler = ouch;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGINT,&act,0);
	while(1)
	{
		printf("Hello world!\n");
		sleep(1);
	}
}
/*
   用sigaction代替signal来设置Ctrl+C组合键的信号处理函数ouch。首先设置一个sigaction结构，在
   该结构中包含信号处理函数、信号屏幕字和标志。在本例中，不需要设置任何标志，并通过新的函数
   sigemptyset来创建空的信号屏蔽字。
*/
