## 三  信号
###3.1. 信号简介
信号是进程之间互相传递消息的一种方法，信号全称为软中断信号，也可称为软中断，它是实现IPC的方法之一。

信号是UNIX和Linux系统响应某些条件而产生的一个事件。进程之间可以互相通过系统调用kill发送软中断信号。内核也可以因为内部事件而给进程发送信号，通知进程发生了某个事件。
  
**注:信号只是简单用来通知某进程发生了什么事件，并不给该进程传递任何数据。** 
 
信号是由于某些错误条件而生成的，如内存段冲突、浮点处理错误或非法指令等。它们由shell和终端处理器生成来引起中断，可以作为在进程间传递消息或修改行为的一种方式，明确地由一个进程发送给另一个进程。无论何种情况，它们的编程接口都是相同的。

**信号可以生成、捕获、响应或忽略。** 用术语生成(raise)表示一个信号的产生,用术语捕获(catch)表示接收到一个信号。接收到该信号的进程会采取相应地处理，具体处理方法可以分为三种：  

* 第一种方法： 类似中断的处理程序，对于需要处理的信号，进程可以指定处理函数，由该函数来处理。
* 第二种方法： 忽略某个信号，对该信号不做任何处理，就像未发生过一样。
* 第三种方法： 对该信号的处理保留系统的默认值，这是缺省操作。对大部分的信号的缺省操作是使得进程终止。进程通过系统调用signal   
              来指定进程对某个信号的处理行为。   

信号的名称在头文件signal.h中定义。它们以SIG开头，下图是系统支持的所有信号种类：
  ![](Screenshots/signal01.png?raw=true)     
**Ctrl+C组合键:** 默认情况下向前台进程发送SIGINT信号，这将引起该进程的终止，除非它事先安排了捕获这个信号。  
**kill命令 :** 如果想发送一个信号给进程，而该进程并不是当前的前台进程，可以使用kill命令。该命令需要有一个可选的信号代码或信号名称和一个需要接收信号的目标进程PID。
    
		kill -HUP 512 向一个运行在另一个终端上的PID为512的进程发送“挂断”信号。
**xkill命令：** 可以向运行着某一命令的所有进程发送信号。  

		xkillall -HUP inetd 通知inete程序重新读取它的配置选项。
**Ctrl + \ 组合键：**默认情况下会产生SIGQUIT信号。

**注:系统对信号的响应视具体实现情况而定。如果进程接收到这些信号中的一个，但事先没有安排捕获它，进程将会终止。通常系统将生成核心转储文件core,并将其放在当前目录下。core文件是进程在内存中的映像，它对程序的调试很有用。**


###3.2 singal 相关函数
函数原型：  
 
	#include <signal.h> 
	void (*signal (int sig, void (*func) (int)))(int); 
    
    #include <sys/types.h>
    #include <signal.h>
    int kill(pid_t pid,int sig);

    #include <unistd.h>
    unsigned int alram(unsinged int seconds);

    #include <unistd.h>
    int pause(void);
函数描述：
  
    signal函数：该函数带有sig和func两个参数，准备捕获或忽略的信号由参数sig给出，接收到指定的信号后将要调
			   用的函数由参数func(为void(*)(int)类型)给出。signal函数返回值为一个函数指针，该函数指针
               指向先前用来处理这个信号的函数，该函数(signal的返回值)同样是接受一个int参数并返回void。
			   信号处理函数必须有一个int类型的参数来表示接收信号的代码，并且返回值为void类型。
                   两种系统定义的特殊信号处理函数，可以用两个特殊值来代替：              
                             SIG_IGN 忽略信号
                             SIG_DFL 恢复信号默认处理行为

    kill函数：  把参数sig指定的信号发送给有参数pid给定的进程号所指定的进程。
               发送信号的进程必须拥有相应的权限，普通用户只能发送信号给属于自己的进程，超级用户可以发送信号给任何进程。
               函数调用失败时返回-1并设置errno变量。
                    失败的原因可能是：
                                 给定的信号无效（errno设置为EINVAL)
                                 发送进程权限不够(errno设置为EPERM）
                                 目标进程不存在(errno设置为ESRCH）

    alarm函数： 闹钟函数，用来在seconds秒之后安排发送一个SIGALRM信号。
               由于处理延迟和时间调度的不确定性，实际闹钟时间比预先安排的稍微拖后些。把参数设置为0将取消所有已设置
               的闹钟请求。如果在接收到SIGALRM信号之前再次调用alarm函数，则闹钟重新开始计时。每个进程只能有一个闹钟时间。
               函数的返回值是以前设置的闹钟时间的秒数，调用失败返回-1。

    pause函数： 把程序的执行挂起直到有一个信号出现为止。当程序接收到一个信号时，预设好的信号处理函数将开始运行，程序
               也就恢复正常的执行。使用pause函数可以使程序不必在一个循环中无休止地检测某个事件是否发生，防止出现忙等
               消耗CPU资源。
               
                                             
###3.3 使用实例
    ctrlc.c
	#include <signal.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	
	void ouch(int sig)
	{
		printf("OUCH!-I got signal %d\n",sig);
		//恢复信号SIGINT的默认处理行为
		(void) signal(SIGINT,SIG_DFL);
	}
	
	int main()
	{
		//自定义信号SIGINT的处理行为
		(void)signal(SIGINT,ouch);
	
		while(1)
		{
			printf("Hello world!\n");
			sleep(1);
		}
	}

    alarm.c
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

	通过fork调用启动新的进程，这个子进程休眠5秒后向其父进程发送一个SIGALRM信号。
    父进程在安排好捕获SIGALRM信号后暂停运行，直到接受到一个信号为止。
	这里并未在信号处理函数中直接调用printf，因为在信号处理函数中调用像printf这样的函数是不安全的。
    常用的做法是通过在该函数中设置标志，然后在main函数中检查该标志来完成消息的输出。


###3.4 sigaction 相关函数

函数原型：

    #include <signal.h>
    int sigaction(int sig,const struct sigaction *act,struct sigaction *oact);
    int sigaddset(sigset_t *set,int signo);
    int sigemptyset(sigset_t *set);
    int sigfillset(sigset_t *set);
    int sigdelset(sigset_t *set,int signo);
    int sigismember(sigset_t *set,int signo);
    int sigprocmask(int how,const sigset_t *set,sigest_t *oset);
    int sigpending(sigset_t *set);
    int sigsuspend(const sigset_t *sigmask);

函数描述：

    sigaction函数：作用是查询或设置信号处理方式。
                  sigaction 结构体定义在接收到参数sig指定的信号后应该采取的行动。该结构定义如下:
                      struct sigaction {
						    void (*sa_handler)(int);
							void (*sa_sigaction)(int, siginfo_t*, void*);
							sigset_t sa_mask;
							int sa_flags;
							void (*sa_restorer)(void);
					 }
				  成员含义:
                        sa_handler   是一个函数指针，主要是表示接收到信号时所要采取的行动。相当于signal函数的参数func,此字段的值可以是      
                                     SIG_DFL,SIG_IGN分别代表默认操作与内核将忽略进程的信号。
                                     这个函数只接受一个参数那就是信号代码。
						sa_sigaction 当SA_SIGINFO被设定在sa_flags中，那么则会使用sa_sigaction来指示信号处理函数，而非sa_handler。
					    sa_mask      设置掩码集，在调用sa_handler所指向的信号处理函数之前，该信号集将被加入到进程的信号屏蔽字中，
                                     在程序执行期间会阻挡掩码集中的信号。设置信号屏蔽字可以防止信号在它的处理函数还未运行结束时就别
                                     接收到信的信号打断。
						sa_flags     设置了一些标志： 
                                             SA_RESETHAND 当该函数处理完成之后，设定为系统默认的处理模式。
                                                          注：由sigaction函数设置的信号处理函数在默认情况下是不被重置的。
                                             SA_NODEFER   在处理函数中，如果再次到达此信号时，将不会阻塞。
                                                          默认情况下，同一信号两次到达时，如果此时处于信号处理	
							                              程序中，那么此信号将会阻塞。
											 SA_SIGINFO   使用sa_sigaction指示的函数。
					   sa_restorer 已经被废弃。
                 如果oact不是空指针，sigaction将把原先对该信号的动作写到它指向的位置。
                 如果act是空指针，则sigaction函数就不需要再做其他设置了，否则将在该参数中设置对指定信号的动作。
                 函数执行成功返回0，失败时返回-1。

    sigaddset函数  ： 向信号集set中添加给定的信号signo。

    sigdelset函数  ： 从信号集set中删除给定的信号signo。

    sigemptyset函数： 将信号集合set初始化为空。

    sigfillset函数 ： 将信号集set初始化为包含所有已定义的信号。
                     以上四个信号集处理函数在调用成功时返回0，失败时返回-1并设置errno。

    sigismeber函数 ： 判定给定的信号sino是否是信号集set的成员。
                     如果是就返回1，不是返回0，如果给出的信号无效，返回-1并设置errno为EINVAL。

    sigpromask函数 ： 根据how指定的方法修改进程的信号屏蔽字。新的信号屏蔽字由参数set指定，而原来的信号屏蔽字将保留到信号集oset中。
                        how 参数的取值如下:
                              SIG_BLOCK   把参数set中的信号添加到信号屏蔽字中
                              SIG_SETMASK 把信号屏蔽字设置为参数set中的信号
                              SIG_UNBLOCK 从信号屏蔽字中删除参数set中的信号
                     如果参数set是空指针，how的值就无意义，此时这个调用的唯一目的就是把当前信号屏蔽字的值保存到oset中。
                     函数在调用成功时返回0，如果how取值无效，它将返回-1并设置errno为EINVAL。

    sigpending函数 ： 将被阻塞的信号中停留在待处理状态的一组信号写到参数set指向的信号集中。
                     如果一个信号被进程阻塞，它就不会传递给进程，但会停留在待处理状态。程序可以通过调用该函数来查看它阻塞的信号中有哪些
                     正停留在待处理状态。
                     函数在调用成功时返回0，失败时返回-1并设置errno。

    sigsuspend函数 ： 将坚持的屏蔽字替换为由参数sigmask给出的信号集，然后挂起程序的执行。程序将在信号处理函数执行完毕后继续执行。
                     如果接收到的信号终止了程序，sigsuspend就不会返回；如果接收到的信号没有终止程序，sigsuspend就返回-1并将
                     errno设置为EINTR。
###3.4 sigaction函数使用

	include <signal.h>
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

	用sigaction代替signal来设置Ctrl+C组合键的信号处理函数ouch。首先设置一个sigaction结构，在
	该结构中包含信号处理函数、信号屏幕字和标志。在本例中，不需要设置任何标志，并通过新的函数
	sigemptyset来创建空的信号屏蔽字。
    
## 四  信号量
###4.1.信号量简介
信号量(semphore)用于管理对资源的访问，程序中存在着一部分临界代码，需要确保只有一个进程(或一个执行线程)可以进入这个临界代码并拥有对资源独占式的访问权。为了防止出现因多个程序同时访问一个共享资源而引发的问题，需要一种方法，它可以通过生成并使用令牌来授权，在任一时刻只能有一个执行进程的线程来访问代码临界区域（临界区域是指执行数据更新的代码需要独占式地执行）。  
             
荷兰计算机科学家Edsger Dijkstra提出的信号量的概念是在并发编程领域迈出的重要一步。 信号量是一个特殊的变量，它只取正整数值，只允许对它进行等待(wait)和发送信号(signal)这两种操作，最简单的信号只能取值0或1的变量，即二进制信号量。这也是信号最常见的一种形式。可以取多个正整数的信号被称为通用信号量。


在Linux编程中，“等待”和“发送信号”都已具有特殊的含义，所以这里将用原来定义的符号来标识这两种操作：
       
    P(信号量变量）：用于等待  
    V(信号量变量）：用于发送信号

PV 操作非常简单，假设有一个信号变量是sv,则这两个操作的定义如下：

    P(sv)  如果sv的值大于零，就给它减去1;如果它的值等于零，就挂起该进程的执行。
    V(sv)  如果其它进程因等待sv而被挂起，就让它恢复运行;如果没有进程因等待sv而被挂起，就给它加1;
	
    还可以这样看信号量，当临界区可用时，信号量变量sv的值为true，然后p(sv)操作将它的值减1,使它变
    为false以表示临界区域正在被使用。当进程离开临界区域时，使用V(sv)操作将它加1,使临界区再次变为可用。

**注:用一个普通变量进行类似的加减法是不行的，因为在C、 C++ 、C#或任何一个传统的编程语言中，都没有一个原子操作可以满足检测变量是否为true，如果是true再将该变量设置为false。这也是信号量操作如此特殊的原因。**

例如，两个进程共用信号sv，一旦其中一个进程执行了p(sv)操作，它将得到信号量，并可以进入临界区。而第二个进程将被阻止进入临界区域，因为它试图执行p(sv)操作时，它会被挂起等待第一个进程离开临界区域并执行V(sv)操作释放信号量。

	semaphore sv = 1
	loop forever {
	   P(sv)
	  critical code setcion;
	  V(sv)
	  noncritical code setcion;
	}



###4.2 信号量函数

函数原型：  

	#include <sys/sem.h>
	int semget(key_t key, int num_sems, int sem_flags);
	int semop(int sem_id, struct sembuf *sem_ops, size_t num_sem_ops);
    int semctl(int sem_id, int sem_num, int command, ...);

函数描述：

    semget函数：创建一个新信号量或取得一个已有的信号量的键。
               key_t key   :  一个用来允许不相关的进程访问相同信号量的整数值。程序对所有信号量的访问都是间接的，它先
                              提供一个键，再由系统生成一个相应的信号量标识符。只有semget函数才可直接使用信号量键，所有其
                              它的信号量函数都是使用由semget函数返回的信号量标识符。在创建新信号量时，需要给键提供一个
                              唯一的非零整数。
                              有一个特殊的信号量key值，IPC_PRIVATE(通常为0)，其作用是创建一个只有创建进程可以访问的
                              信号量。这个键值通常没有实际的用途。
               int num_sems:  所需信号量的数目，通常为1。
               int sem_flags: 一个标记集合，与open函数的标记十分类似。低九位是信号的权限，其作用与文件权限类似。另外，
                              这些标记可以与IPC_CREAT进行或操作来创建新的信号量。
							  注：设置IPC_CREAT标记并且指定一个已经存在的信号量键值并不是一个错误。如果不需要，
								  IPC_CREAT标记只是被简单的忽略。
								  可以使用IPC_CREAT与IPC_EXCL的组合来保证可以获得一个新的唯一的信号量。
							      如果这个信号量已经存在，则会返回一个错误。        
    		  函数调用成功时返回一个正整数，即信号量标识符。失败时返回-1。

    semop函数：用于改变信号量的值。
              int msqid             ：信号量描述符，由 semget 函数调用创建。
              struct sembuf *sem_ops：指向结构体数组的指针，每个数组元素包括的成员如下:
                                        struct sembuf {
						                    short sem_num;
									        short sem_op;
									        short sem_flg;
					                     } 
                                         sem_num: 信号量编号，除非需要使用一组信号量，否则它的值一般取0.
                                          sem_op: 信号量在一次操作中需要改变的值，可以使用一个非1的数值来改变信号量的值。通常只会
                                                  用到两个值:
                                           				一个是-1，即P操作，它等待信号量变为可用。
						        		   				一个是+1，即V操作，它发送信号表示信号量现在已可用。
                                        sem_flg： 通常设置为SEM_UNDO。这会使得操作系统跟踪当前进程对信号量所做的改变，而且如果进程
                                      			  在没有释放这个信号量的情况下终止，操作系统将自动释放该进程持有的信号量。
                                       			  注：将sem_flg设置为SEM_UNDO是一个好习惯，除非有特殊的要求，需要不同的行为。
                                       			      如果决定使用一个非SEM_UNDO的值，保存一致性就十分重要，否则就会变得十分迷惑，搞不
                                       			      清楚当进程退出时，内核是否会尝试清理信号量。
                size_t num_sem_ops：要操作的sembuf结构体的数目。恒大与或等于1。
						            semop调用的一切动作都是一次性完成的，这是为了避免出现因使用多个信号量而可能发生的竞
                                    争现象。
    			函数调用成功返回0，失败返回-1，具体错误类型可具体查看errno。

    semctl函数：对信号量信息进行直接控制。
               int msgid   ： 信号量描述符，由 semget 函数调用创建。
               sem_num     ： 信号量编号。当使用信号量数组时会用到这个参数。通常取值为0，表示这是第一个也是唯一的一个信号量。
               int command ： 将要采取的动作。
                              如果还有第四个参数，它将会是一个union semun结构体，根据X/OPEN规范，它至少包含以下几个成员：
							     union semun {
								     int val;
								     struct semid_ds *buf;
								     unsigned short *array;
								 }
                              注:虽然X/Open规范中确认说semun联合结构必须由程序员自己定义。但大多数Linux版本会在某头文件(通常为sem.h)中定义
                                 semun联合。如果确实需要定义自己的联合，可以查看semctl手册页了解定义。如果有这样的情况，建议使用手册页中提
                                 供的定义，尽管这个定义与上面的有区别。
                              command经常使用下面两个值：
									SETVAL：用于初始化信号量为一个已知的值。所需要的值作为联合semun的val成员来传递。其作用是信号量第一次使用
                                            之前对它进行设置。
								  IPC_RMID：用于删除一个已经无需继续使用的信号量标识符。
              函数依据command参数会返回不同的值。对于SETVAL与IPC_RMID，如果成功则会返回0，否则会返回-1。
 
    
###4.3 使用实例
	semun.c
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <sys/sem.h>

	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	
	static int set_semvalue(void);
	static void del_semvalue(void);
	static int semaphore_p(void);
	static int semaphore_v(void);
	
	static int sem_id;
	
	int main(int argc,char **argv)
	{
		int i ;
		int pause_time;
		char op_char = 'O';
		srand((unsigned int)getpid());
	
	    //create semaphore
		sem_id = semget((key_t)1234,1,0666|IPC_CREAT);
	
		if (argc >1)
		{
			//set sempvalue 初始化信号量
			if (!set_semvalue())
			{
				fprintf(stderr,"Failed to initialize semaphore");
				exit(EXIT_FAILURE);
			}
			op_char = 'X';
			sleep(2);
		}
	
		for (i = 0; i < 10; ++i)
		{
			//设置信号量 等待进入
			if (!semaphore_p())
			{
				exit(EXIT_FAILURE);
			}
			printf("%c",op_char);
			fflush(stdout);
			pause_time = rand() % 3;
			sleep(pause_time);
			printf("%c",op_char);
			fflush(stdout);
	
			//退出临界区，信号量设置为可用
			if (!semaphore_v())
			{
				exit(EXIT_FAILURE);
			}
			pause_time = rand() % 3;
			sleep(pause_time);
		}
		printf("\n%d - finished \n",getpid());
		
		if (argc>1)
		{
			sleep(10);
			//删除信号量
			del_semvalue();
		}
		exit(EXIT_SUCCESS);
	}
	
	static int set_semvalue(void)
	{
		union semun sem_union;
		sem_union.val = 1;
		if (semctl(sem_id,0,SETVAL,sem_union) == -1)
		{
			return 0;
		}
		return 1;
	} 
	
	static void del_semvalue()
	{
		union semun sem_union;
		sem_union.val = 1;
		if (semctl(sem_id,0,IPC_RMID,sem_union) == -1)
			fprintf(stderr, "Failedto delete semaphore.\n");
	}
	
	static int semaphore_p()
	{
		struct sembuf sem_b;
	
		sem_b.sem_num = 0;
		sem_b.sem_op     = -1;
		sem_b.sem_flg  = SEM_UNDO;
	
		if (semop(sem_id,&sem_b,1)== -1)
		{
			fprintf(stderr,"sempore_p failed.");
			return 0;
		}
		return 1;
	}
	
	static int semaphore_v()
	{
		struct sembuf sem_b;
	
		sem_b.sem_num = 0;
		sem_b.sem_op     = 1;
		sem_b.sem_flg  = SEM_UNDO;
	
		if (semop(sem_id,&sem_b,1)== -1)
		{
			fprintf(stderr,"sempore_p failed.");
			return 0;
		}
		return 1;
	}

 可以使用`ipc -s` 命令来查看系统的消息队列：  
   ![](Screenshots/sem.png?raw=true)

###4.4 小结
所有的Linux信号量函数都是针对成组的信号量进行操作，而不是只针对一个二进制信号量。当在一个进程中需要锁定多个资源的复杂情况下，这种能够对一组信号量进行操作的能力是一个巨大的优势。