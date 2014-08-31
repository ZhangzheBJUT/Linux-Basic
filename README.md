### 一 进程概述
 一个进程都由另一个称之为父进程的进程启动，被父进程启动的进程叫做子进程。Linux系统启动时候，它将运行一个名为init的进程，该进程是系统运行的第一个进程，它的进程号为1，它负责管理其它进程，可以把它看做是操作系统进程管理器，它是其它所有进程的祖先进程。系统中的进程要么是由init进程启动，要么是由init进程启动的其他进程启动。


  使用`ps`命令输出中的`PPID`栏给出的是父进程的进程`ID`，它是启动这个进程的`PID`。如果原来父进程已经不存在了，该栏显示的就是init进程的`ID`。
   如下图所示:
   `ps -ef`
  ![](Screenshots/1.jpg?raw=true)
   

### 二 进程创建
###2.1.**system函数**  
   	表头文件: #include<stdlib.h>   
	函数原型: int system(const char * string);   
	函数说明: system()会调用fork()产生子进程，由子进程来调用/bin/sh-c string来执行参数
		     string字符串所代表的命令，此命令执行完后随即返回原调用的进程。在调用system()期间
             SIGCHLD信号会被暂时搁置，SIGINT和SIGQUIT 信号则会被忽略。   
	返回值 :=-1:出现错误   
			=0:调用成功但是没有出现子进程 
		    >0:成功退出的子进程的id 
   
    #include <stdlib.h>    
    #include <stdio.h>    
	int main()    
	{    
		printf("Running ps with system.\n");    
		system("ps ax &");    
		printf("Done.\n");    
		exit(0);    
	}
   
    
   system函数的作用是，运行以字符串参数的形式传递给它的命令,它会在一个进程的内部启动另一个进程，并等待命令的完成。命令的执行情况就如同在shell中执行如下命令:`sh -c string`

   system()会调用fork()产生子进程，由子进程来调用/bin/sh-c string来执行参数string字符串所代表的命令，此命令执行完后随即返回原调用的进程。

   **system局限性:**使用System函数远非启动其他进程的理想手段，因为它必须用一个shell来启动需要的程序。由于在启动之前需要先启动一个shell,而且对shell的安装情况及使用的环境的依赖也很大，所以使用system函数的效率不高。System很有用但是也有它的局限性，因为程序必须等待由System函数启动的进程结束之后才可能继续，因此我们不能立刻执行其他任务。          
###2.2. **exec系列函数**   
  > int execp(const char * file,const char * arg,……);  
	
 	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	int main()
	{
		printf("Running ps with execlp\n.");
		execlp("ps","ps","ax",0);
		printf("Done.\n");
		exit(0);
	}
exec系列函数有一组相关的函数组成，它们在进程的启动方式和程序参数的表达式上各有不同。exec函数可以把当前进程替换为一个新进程，新进程由path或file参数指定。可以使用exec函数将程序的执行从一个程序切换到另一个程序。  
***注：新进程的PID、PPID和nice值与原来的完全一样，exec只是用另一个新程序替换了当前进程的正文、数据、堆和栈段。*** 事实上，这里发生的一切其实就是， 当进程调用一种exec函数时，该进程完全由新进程代换，而新进程则从其main函数开始执行。因为调用exec并不创建新进程，所以前后的进程ID并未改变。

新进程继承了原进程的许多特性，在原进程中已经打开的文件描述符在新进程中仍将保持打开，除非它们的“执行时关闭标志”被置位。任何在原进程中已打开的目录流都将在新进程中被关闭。
一般情况下，exec函数是不会返回的，除非发生错误。出现错误时候，exec函数返回-1，并且会设置错变量errno.


###3.3. **fork函数**
	表头文件: <sys/types.h> 和 <unistd.h>  
	函数原型:pid_t  fork(void);  
	函数说明:创建一个子进程
    返回值:			
		如果成功创建一个子进程，对于父进程来说返回子进程ID  
		如果成功创建一个子进程，对于子进程来说返回值为0  
		如果为-1表示创建失败

![](Screenshots/2.gif?raw=true)

	#include <sys/types.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <sys/wait.h>
	#include <stdlib.h>

	int main()
	{
		pid_t pid;
		char *message;
		int n;
		int exit_code;

		printf("fork program starting\n");
		pid  = fork();

		switch(pid)
		{
		case -1:
			perror("fork failed.");
			exit(1);
		case 0:
	 		message = "This is the child";
	 		n = 8;
	 		exit_code = 37;
	 		break;
	 	default:
	 		message = "This is the parent";
	 		n = 5;
	 		exit_code = 0;
	 		break;
	 	}

	 	for (;n>0;n--)
	 	{
	 		puts(message);
	 		sleep(1);
	 	}

     	if (pid != 0)
     	{	
       		int stat_val;
       		pid_t child_pid;

       		child_pid = wait(&stat_val);
       		printf("Child has finished:PID = %d\n",child_pid );

       		if (WIFEXITED(stat_val))
       		{
       			printf("Child exited with code %d\n",WEXITSTATUS(stat_val));
       		}
        	else
        	{
         		printf("Child terminated abnormally\n");
        	}
     	}

	 	exit(exit_code);
	}

在父进程中`fork`调用返回的是新子进程的PID,子进程将从`fork`调用处执行，就像原进程一样，不同之处在于，在子进程中`fork`调用返回的是0,父进程可以通过这一点来判断究竟谁是父进程，谁是子进程。   

***注：如果`fork`失败它将返回-1,失败通常是因为父进程所拥有的子进程数目超过了规定的限制(CHILD_MAX)，此时errno将被设为EAGNIN。如果是因为进程表里没有足够的空间用于创建新的表单或虚拟内存不足，errno变量将被设为ENOMEM。***

`fork`系统调用之后，父子进程将交替执行。如果父进程先退出，子进程还没退出---**孤儿进程**,那么子进程的父进程将变为init进程。    
***注：任何一个进程都必须有父进程***  
如果子进程先退出，父进程还没退出，那么子进程必须等到父进程捕获到了子进程的退出状态才真正结束，否则这个时候子进程就成为**僵尸进程**。子进程退出会发送SIGCHLD信号给父进程，父进程可以选择忽略或使用信号处理函数接收处理就可以避免僵尸进程。

**`fork` 函数得到的子进程从父进程继承了整个进程的地址空间，包括：进程上下文、进程堆栈、内存信息、打开的文件描述符、信号控制设置、进程优先级、进程组号、当前工作目录、根目录、资源限制、控制终端等、权限掩码`umask`等**。

### 三 僵尸和孤儿进程

在Unix/Linux中，子进程是通过父进程创建的，子进程再创建新的进程。子进程的结束和父进程的运行是一个异步过程,即父进程永远无法预测子进程到底什么时候结束。 当一个进程完成它的工作终止之后，它的父进程需要调用`wait()`或者`waitpid()`系统调用取得子进程的终止状态。

###3.1.**僵尸进程**
在Linux进程的状态中，***僵尸进程是非常特殊的一种，它已经放弃了几乎所有内存空间，没有任何可执行代码，也不能被调度，仅仅在进程列表中保留一个位置，记载保留一定的信息(包括进程号the process ID,退出状态the termination status of the process,运行时间the amount of CPU time taken by the process等), 直到父进程通过wait / waitpid来取时才释放，除此之外，僵尸进程不再占有任何内存空间。*** 它需要它的父进程来为它"收尸"，如果他的父进程没安装SIGCHLD信号处理函数调用`wait`或`waitpid()`等待子进程结束，又没有显式忽略该信号，那么它就一直保持僵尸状态，如果这时父进程结束了，那么`init`进程自动会接手这个子进程，为它"收尸"，它还是能被清除的。但是如果如果父进程是一个循环，不会结束，那么子进程就会一直保持僵尸状态，这就是为什么系统中有时会有很多的僵尸进程。**父进程死后僵尸进程成为"孤儿进程"**

**当一个子进程结束运行（一般是调用exit、运行时发生致命错误或收到终止信号所导致）时，并非马上就消失掉，而是留下一个称为僵尸进程(Zombie)的数据结构，等待父进程处理。子进程的退出状态（返回值）会回报给操作系统，系统则以SIGCHLD信号将子进程被结束的事件告知父进程，此时子进程的进程控制块（PCB）仍驻留在内存中。一般来说，收到SIGCHLD后，父进程会使用`wait`系统调用以取得子进程的退出状态，然后内核就可以从内存中释放已结束的子进程的PCB；而若父进程没有这么做的话，子进程的PCB就会一直驻留在内存中，成为僵尸进程。如果大量的产生僵死进程,将因为没有可用的进程号而导致系统不能产生新的进程。**
   

如果父进程能及时处理，可能用ps命令就来不及看到子进程的僵尸状态，但这并不等于子进程不经过僵尸状态。如果父进程在子进程结束之前退出，则子进程将由init接管。init将会以父进程的身份对僵尸状态的子进程进行处理。
	![](Screenshots/3.png?raw=true)


怎样来清除僵尸进程：   
    
	1. 改写父进程
	   在子进程死后要为它"收尸"。具体做法是接管SIGCHLD信号。子进程死后，会发送SIGCHLD信号给父进程，
	   父进程收到此信号后，执行waitpid(函数为子进程"收尸"。这是基于这样的原理：就算父进程没有调用
       wait，内核也会向它发送SIGCHLD消息，尽管对的默认处理是忽略，如果想响应这个消息，可以设置一个
       处理函数。

	2. 杀掉父进程
	   父进程死后，僵尸进程成为"孤儿进程"会自动过继给1号进程init， init始终会负责清理僵尸进程．它产生的所有
	   僵尸进程也跟着消失。
 

###3.2.**孤儿进程**  
   一个父进程退出，而它的一个或多个子进程还在运行，那些子进程将成为孤儿进程。孤儿进程将被init进程(进程号为1)所收养，并由init进程对它们完成状态收集工作。

###3.3 **两者的区别**
僵尸进程和孤儿进程的区别：
   
   * 僵尸进程：当前进程运行结束父进程未结束且父进程没有调用wait来清理子进程；有危害 
   * 孤儿进程：当前进程结束且父进程在它之前结束，这时子进程被“托孤”于init进程；没危害
   
### 四 守护进程

#### 4.1 **守护进程概述**
守护进程，也就是通常说的Daemon进程，是Linux中的后台服务进程,它是一个生存期较长的进程，通常独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。守护进程常常在系统引导装入时启动，在系统关闭时终止。Linux系统有很多守护进程，大多数服务都是通过守护进程实现的，同时，守护进程还能完成许多系统任务，例如，作业规划进程crond、打印进程lqd等（这里的结尾字母d就是Daemon的意思）。

守护进程脱离于终端是为了避免进程在执行过程中的信息在任何终端上显示并且进程也不会被任何终端所产生的终端信息所打断。为了增加灵活性，root可以选择系统开启的模式，这些模式叫做运行级别，每一种运行级别以一定的方式配置系统。

![](Screenshots/4.png?raw=true)  
TPGID一栏写着-1的都是没有控制终端的进程，也就是守护进程。在COMMAND一列用[]括起来的名字表示内核线程，这些线程在内核里创建，没有用户空间代码，因此没有程序文件名和命令行，通常采用以k开头的名字，表示Kernel。init是祖先进程，守护进程通常采用以d结尾的名字，表示Daemon。

***附:由于在Linux中，每一个系统与用户进行交流的界面称为终端，每一个从此终端开始运行的进程都会依附于这个终端，这个终端就称为这些进程的控制终端，当控制终端被关闭时，相应的进程都会自动关闭。但是守护进程却能够突破这种限制，它从被执行开始运转，直到整个系统关闭时才退出。如果想让某个进程不因为用户或终端或其他地变化而受到影响，那么就必须把这个进程变成一个守护进程。***

 
#### 4.2 **相关概念**
 * **进程组**  
 进程组就是一组进程的集合。这些进程并不是孤立的，他们彼此之间或者存在父子、兄弟关系，或者在功能上有相近的联系。  
 进程组ID也是一个进程的必备属性。每个进程组都有一个组长进程，其组长进程的进程号等于进程组ID。且该进程组ID不会因组长进程的退出而受到影响。

    **提供进程组就是为了方便对进程进行管理。**假设要完成一个任务，需要同时并发100个进程。当用户处于某种原因要终止这个任务时，要是没有进程组，就需要手动的一个个去杀死这100个进程，并且必须要严格按照进程间父子兄弟关系顺序，否则会扰乱进程树。有了进程组，就可以将这100个进程设置为一个进程组，它们共有1个组号（pgrp），并且有选取一个进程作为组长（通常是“辈分”最高的那个，该进程的ID也就作为进程组的ID）。现在就可以通过杀死整个进程组，来关闭这100个进程，并且是严格有序的。

 ***注：进程必定属于一个进程组，也只能属于一个进程组。  一个进程组中可以包含多个进程。 进程组的生                 命周期从被创建开始，到其内所有进程终止或离开该组。  
 组长进程可以创建一个进程组，创建该进程组中的进程，只要进程组中有一个进程存在，进程组就存在，与组长进程是否终止无关。  
 进程组生存期: 进程组创建到最后一个进程离开(终止或转移到另一个进程组)***

 内核中，`getpgrp()`系统调用用来获取当前进程所在进程组号；`setpgid(int pid, int pgid)`调用用来设置置顶进程`pid`的进程组号为`pgid`。
		
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

 * **会话期**  

   会话期是一个或多个进程组的集合。通常，一个会话开始于用户登录，终止于用户退出，在此期间该用户运行的所有进程都属于这个会话期。  
   由于Linux是多用户多任务的分时系统，所以必须要支持多个用户同时使用一个操作系统。当一个用户登录一次系统就形成一次会话。每个会话都有一个会话首领（leader），即创建会话的进程。一个会话可包含多个进程组，但只能有一个前台进程组。
   setsid()调用能创建一个会话,调用setsid 之后，该进程成为新会话的leader。  
   ***注:只有当前进程不是进程组的组长时，才能创建一个新的会话。***  
   	
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

  setsid函数用于创建一个新的会话，并担任该会话组的组长。  
   >调用setsid有下面的3个作用：   
   > 1.让进程摆脱原会话的控制  
   > 2.让进程摆脱原进程组的控制  
   > 3.让进程摆脱原控制终端的控制

#### 4.3 **守护进程的创建**
![](Screenshots/5.png?raw=true)

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <sys/types.h>
	#include <fcntl.h>
	
	
	int main(int argc,char**argv)
	{
		pid_t pid;
		int i,fd;
	
		char*buf = "This is a daemon program.\n";
		pid = fork();
		if (pid < 0)
		{
			printf("fork error!");
			exit(-1);
		}
		else if (pid > 0)
		{
			exit(0); //1.父进程退出
		}
	
		setsid();    //2.子进程中创建新会话
		chdir("/");  //3.设置工作目录为根目录
		umask(0);    //4.设置文件权限掩码
	
		for (i=0;i<getdtablesize();i++)  //5.getdtablesize返回子进程文件描述符表项数
			close(i);                    //关闭这些不将用到的文件描述符
	
		while(1)
		{
			fd = open("/tmp/daemon.log",O_CREAT|O_WRONLY|O_APPEND,0600);
			if(fd < 0)
			{
				printf("Open file error!\n");
				exit(1);
			}
	
			write(fd,buf,strlen(buf)+1);
			close(fd);
			sleep(10);
			printf("Never output!\n");
		}
		return 0;
	}

 **说明:** 
 
 **1. 创建子进程 父进程退出**  
   这是编写守护进程的第一步。由于守护进程是脱离控制终端的，因此，完成第一步后就会在Shell终端里造成一程序已经运行完毕的假象。之后的所有工作都在子进程中完成，而用户在Shell终端里则可以执行其他命令，从而在形式上做到了与控制终端的脱离。  
   在Linux中父进程先于子进程退出会造成子进程成为孤儿进程，而每当系统发现一个孤儿进程是，就会自动由1号进程（init）托管它，这样原先的子进程就会变成init进程的子进程。 
 
 **2. 创建新的会话**   
   **建立新回话是创建守护进程关键的一步。** 创建守护进程的第一步调用了`fork`函数来创建子进程，再将父进程退出。由于在调用了`fork`函数时，子进程全盘拷贝了父进程的会话期、进程组、控制终端等，虽然父进程退出了，但会话期、进程组、控制终端等并没有改变，因此，还不是真正意义上的独立开来，***setsid函数能够使进程完全独立出来，从而摆脱其它进程的控制。***
 **3. 设置权限掩码** 
   使用`fork`创建的子进程继承了父进程的当前工作目录。由于在进程运行中，当前目录所在的文件系统是不能卸载的，这对以后的使用会造成诸多的麻烦（比如系统由于某种原因要进入但用户模式）。因此，通常的做法是让"/"守护进程的当前工作目录，这样就可以避免上述的问题，当然，如有特殊需要，也可以把当前工作目录换成其他的路径，如/tmp。改变工作目录的常见函数式 `chdir`。

 **4. 设置文件权限掩码**   
   使用`fork`函数新建的子进程继承了父进程的文件权限掩码，这就给该子进程使用文件带来了诸多的麻烦。因此，把文件权限掩码设置为0，可以大大增强该守护进程的灵活性。设置文件权限掩码的函数是umask。在这里，通常的使用方法为`umask(0)`。  

 **5. 关闭文件描述符**  
   用`fork`函数新建的子进程会从父进程那里继承一些已经打开了的文件。这些被打开的文件可能永远不会被守护进程读写，但它们一样消耗系统资源，而且可能导致所在的文件系统无法卸下

### 使用`fork`函数创建的子进程继承了父进程的许多属性，创建守护进程的过程就是消除继承来的属性的过程,是子进程摆脱与父进程的联系过程。
 



### 补充:   
* **SIGCHILD信号**  
在一个进程终止或者停止时，将SIGCHLD信号发送给其父进程。按系统默认将忽略此信号。如果父进程希望被告知其子系统的这种状态，则应捕捉此信号。信号的捕捉函数中通常调用wait函数以取得进程ID和其终止状态。
	
	SIGCHLD属于unix以及类unix系统的一种信号，产生原因siginfo_t代码值:

		1 子进程已终止 CLD_EXITED
		2 子进程异常终止（无core） CLD_KILLED
		3 子进程异常终止（有core） CLD_DUMPED
		4 被跟踪子进程以陷入 CLD_TRAPPED
		5 子进程已停止 CLD_STOPED
		6 停止的子进程已经继续 CLD_CONTINUED
* **unmask**  
  Linux提供了一个方便的工具umask，可以用来设置文件的默认掩码。它是一个由3个八进制数字组成的值。每个数字都是八进制1、2、4的OR操作结果。这三个数字分别对应着用户(USER)、组(group)和其他用户(other)d的访问权限。  

   ![](Screenshots/6.png?raw=true)  
  ***注：文件的默认权限就是umask设置的掩码和系统中默认的最大权限码做异或运算，其结果就是文件的默认权限。open或creat调用给出的mode值将与当时的用户掩码的反值做AND操作。例如，如果用户掩码是001，且指定了S_IXOTH模式标志，那么其他用户将对创建的文件不会拥有执行权限。*** 

   ![](Screenshots/7.png?raw=true)

* **getdtablesize()**

		Get the size of the file descriptor table
		
		Synopsis:
		#include <unistd.h>
		int  getdtablesize( void );
		
        Library:
		libc
		Use the -l c option to qcc to link against this library. This library is usually included automatically.
		
		Description:
		Each process has a fixed size descriptor table, which is guaranteed to 
        have at least 20 slots. The entries in the descriptor table are numbered
        with small integers starting at 0. The getdtablesize() returns the size of this table.
		This function is equivalent to getrlimit() with the RLIMIT_NOFILE option.
		
		Returns:
		The size of the file descriptor table.
   