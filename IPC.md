## 一 IPC 概述

进程间通信就是在不同进程之间传播或交换信息，那么不同进程之间存在着什么双方都可以访问的介质呢？进程的用户空间是互相独立的，一般而言是不能互相访问的，唯一的例外是共享内存区。但是，系统空间却是“公共场所”，所以内核显然可以提供这样的条件，如下图所示。除此以外，那就是双方都可以访问的外设了。在这个意义上，两个进程当然也可以通过磁盘上的普通文件交换信息，或者通过“注册表”或其它数据库中的某些表项和记录交换信息。  
  ![](Screenshots/IPC00.png?raw=true)

linux下的进程通信手段基本上是从Unix平台上的进程通信手段继承而来的。而对Unix发展做出重大贡献的两大主力AT&T的贝尔实验室及BSD（加州大学伯克利分校的伯克利软件发布中心）。他们在进程间通信方面的侧重点有所不同，前者对Unix早期的进程间通信手段进行了系统的改进和扩充，形成了 `system V IPC`，通信进程局限在单个计算机内；后者则跳过了该限制，形成了基于套接口 `socket`的进程间通信机制。  
Linux则把两者继承了下来，如下图示：  
     ![](Screenshots/IPC03.png?raw=true) 

	Unix  IPC包括： 管道、FIFO、信号；System V IPC包括：System V消息队列、System V信号灯、System V共享内存区；
    Posix IPC包括： Posix消息队列、Posix信号灯、Posix共享内存区。
    有两点需要简单说明一下：
        1.由于Unix版本的多样性，电子电气工程协会（IEEE）开发了一个独立的Unix标准，这个新的ANSI Unix标准被称为计算
          机环境的可移植性操作系统界面（PSOIX）。现有大部分Unix和流行版本都是遵循POSIX标准的，而Linux从一开始就遵循POSIX标准；
        2.BSD并不是没有涉足单机内的进程间通信（socket本身就可以用于单机内的进程间通信）。事实上，很多Unix版本的单
          机IPC留有BSD的痕迹，如4.4BSD支持的匿名内存映射、4.3+BSD对可靠信号语义的实现等等。

上图给出了linux 所支持的各种IPC手段，为了避免概念上的混淆，在尽可能少提及Unix的各个版本的情况下，所有问题的讨论最终都会归结到Linux环境下的进程间通信上来。并且，对于Linux所支持通信手段的不同实现版本（如对于共享内存来说，有Posix共享内存区以及System V共享内存区两个实现版本），将主要介绍Posix API。  

**linux下进程间通信的几种主要手段简介：**
    
 *  管道（Pipe）及命名管道（named pipe）：管道可用于具有亲缘关系进程间的通信，命名管道克服了管道没有名字的限制，因此，除具有管道所具有的功能外，它还允许无亲缘关系进程间的通信；
 *  信号（Signal）：信号是比较复杂的通信方式，用于通知接受进程有某种事件发生，除了用于进程间通信外，进程还可以发送信号给进程本身；linux除了支持Unix早期信号语义函数sigal外，还支持语义符合Posix.1标准的信号函数sigaction（实际上，该函数是基于BSD的，BSD为了实现可靠信号机制，又能够统一对外接口，用sigaction函数重新实现了signal函数）；
 *  消息队列：消息队列是消息的链接表，包括Posix消息队列和system V消息队列。有足够权限的进程可以向队列中添加消息，被赋予读权限的进程则可以读走队列中的消息。消息队列克服了信号承载信息量少，管道只能承载无格式字节流以及缓冲区大小受限等缺点。
 *  共享内存：使得多个进程可以访问同一块内存空间，是最快的可用IPC形式。是针对其它通信机制运行效率较低而设计的。往往与其它通信机制，如信号量结合使用，来达到进程间的同步及互斥。
 *  信号量（semaphore）：主要作为进程间以及同一进程不同线程之间的同步手段。
 *  套接字（Socket）：更为一般的进程间通信机制，可用于不同机器之间的进程间通信。起初是由Unix系统的BSD分支开发出来的，但现在一般可以移植到其它类Unix系统上，Linux和System V的变种都支持套接字。


## 二  管道
当从一个进程连接数据流到另一个进程时，使用术语管道(pipe)。通常是把一个进程的输出通过管道连接到另一个进程的输入。  
对于shell命令来说，命令的连接是通过管道操作符来完成的，如下所示：  
>       cmd1 | cmd2    
>
>		  shell负责安排两个命令的标准输入和标准输出
>       cmd1的标准输入来自终端键盘  
>       cmd1的标准输出传递给cmd2,作为它的标准输入  
>       cmd2的标准输出连接到终端屏幕

  ![](Screenshots/IPC01.png?raw=true)  
**shell所做的工作实际上是对标准输入和标准输出流进行重新连接，使数据流从键盘输入通过两个命令最终输出到屏幕上。**

###2.1.poen与pclose函数
函数原型:       
  
	#include <stdio.h>  
	FILE *popen(const char*command,const char *open_mode);  
	int pclose(FILE *stream_to_close);

函数描述:

	popen      函数允许一个程序将另一个程序作为新进程来启动，并可以传递数据给它或者通过它来接收数据。
    command    字符串是要运行的程序名和相应的参数，这个命令被送到 /bin/sh 以 -c 参数执行, 即由 shell来执行。
    open_mode  必须为"r"或者"w"，二者只能选择一个，函数的返回值FILE*文件流指针，通过常用的stdio库函数
               (如fread)来读取被调用程序的输出。如果open_mode是"w"，调用程序就可以用fwrite调用向被调用程序发送
               数据，而被调用程序可以在自己的标准输入上读取数据。

	补:/bin/sh -c   
        	Read commands from the command_string operand instead of from the standard input. 
 			Special  parameter 0 will be  set from the command_name operand and the positional 
			parameters ($1, $2, etc.) set from the remaining argument operands.



读取外部程序的输出：

		#include <unistd.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h>
		
		int main()
		{
			FILE *read_fp;
			char buffer[BUFSIZ+1];
			int chars_read;
		
			memset(buffer,'\0',sizeof(buffer));
			read_fp = popen("uname -a","r");
			if (read_fp !=NULL)
			{
				chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);
				if (chars_read>0)
				{
					printf("output was:-\n%s\n",buffer);
				}
		
				pclose(read_fp);
				exit(EXIT_SUCCESS);
			}
			exit(EXIT_FAILURE);
		}

将输出发送到外部程序:   
    
		#include <unistd.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h>
		
		int main()
		{
			FILE *write_fp;
			char buffer[BUFSIZ+1];
			
			sprintf(buffer,"Once upon a time ,thera was ...\n");
			write_fp = popen("od -c","w");
		
			if (write_fp != NULL)   
			{
				fwrite(buffer,sizeof(char),strlen(buffer),write_fp);
				pclose(write_fp);
				exit(EXIT_SUCCESS);
			}
			exit(EXIT_FAILURE);
		}
**注意:popen()函数的返回值是一个普通的标准I/O流,且它只能用pclose()函数来关闭,而不是fclose()。**

**popen函数运行一个程序时，它首先启动shell,即系统中的sh命令，然后将command字符串作为一个参数传递给它，由shell来负责分析命令字符串，它允许我们通过popen启动非常复杂的shell命令。使用shell的一个不太好的影响是，针对每个popen的调用，不仅要启动一个被请求的程序，还要启动一个shell，即每个popen调用将多启动两个进程，从节省资源的角度来看，popen函数的调用成本略高，而且对目标命令的调用比正常方式要慢一些。
pclose调用只在popen启动的进程结束后才返回。如果调用pclose时它仍在运行，pclose调用将等待该进程的结束**

###2.2.pipe函数

底层pip函数，通过这个函数在两个程序之间传递数据不需要启动一个shell来解释请求命令，它同时还提供了对读写数据的更多控制。

函数原型:  

		#include <unistd.h>  
		int pipe(int file_descriptor[2]);

函数描述:

	pipe 函数参数是一个由两个整数类型的文件描述符组成的数组的指针。该函数在数组中填上两个新的文件描述符后返回0,如果
    失败则返回-1,并设置error来表明识失败的原因。  
	 常见的错误：
          EMFILE:进程使用的文件描述符过多
     	  ENFILE:系统的文件表已满
     	  EFAULT:文件描述符无效
    两个返回的文件描述符以一种特殊的方式连接起来，写到file_descriptor[1]的所有数据都可以从file_descriptor[0]读出来。
    数据基于先进先出的原则进行处理，意味着如果你把1,2,3写到file_descriptor[1],从file_descriptior[0]读取到的数据也是1,2,3。

**注意:调用pipe函数时在内核中开辟一块缓冲区（称为管道）用于通信，它有一个读端一个写端，然后通过 `file_descriptor` 参数传出给用户程序两个文件描述符，file_descriptor[0]指向管道的读端，`file_descriptor[1]` 指向管道的写端。在用户程序看起来管道就像一个打开的文件，通过read(`file_descriptor[0]`)或者write(`file_descriptor[1]`)来向这个文件读写数据，其实是在读写内核缓冲区。两个文件描述符被强制规定 `file_descriptor[0]` 只能指向管道的读端，如果进行写操作就会出现错误；同理 `file_descriptor[1]`只能指向管道的写端，如果进行读操作就会出现错误。pipe使用的是文件描述符而不是文件流，所以必须使用底层的read和write调用来访问数据，而不是用文件流函数fread和fwrite。**

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
		
			memset(buffer,'\0',sizeof(buffer));
		
			if (pipe(file_pipes)==0)
			{
				data_processed = write(file_pipes[1],some_data,strlen(some_data));
				printf("Wrote %d bytes \n",data_processed);
		
				data_processed = read(file_pipes[0],buffer,BUFSIZ);
				printf("Read %d bytes \n",data_processed);
				exit(EXIT_SUCCESS);
			}
			exit(EXIT_FAILURE);
		}


**程序说明:** 这个程序用数组 `file_pipes` 中的两个文描述符创建一个管道，然后它用文件描述符 `file_pipes[1]` 向管道中写数据
，再从 `file_pipes[0]` 读回数据。  

**管道的真正优势体现在:两个进程之间传递数据。** 当程序用fork调用创建新进程时，原先打开的文件描述符仍将保持打开状态。如果在原先的进程中创建一个管道，然后再调用fork创建新进程，即可以通过管道在两个进程之间传递数据，如下图所示:
   ![](Screenshots/IPC02.png?raw=true)
  
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

**程序说明:** 程序首先用pipe函数创建一个管道，接着用fork调用创建一个新进程。如果fork调用成功，父进程首先关闭读操作符，然后写数据到管道中，而子进程首先关闭写操作符，然后从管道中读取数据。父子进程都在只调用了一次write或read之后就退出。其原理如下图所示:    
![](Screenshots/IPC04.png?raw=true)

 系统维护一个文件的文件描述符表的计数，父子进程都各自有指向相同文件的文件描述符，当关闭一个文件描述符时，相应计数减一，当这个计数减到0时，文件就被关闭，因此虽然父进程关闭了其文件描述符 `file_pipes[0]`，但是这个文件的文件描述符计数还没等于0，所以子进程还可以读取。也可以这么理解，父进程和子进程都有各自的文件描述符，因此虽然父进程中关闭了`file_pipes[0]`，但是对子进程中的 `file_pipes[0]` 没有影响。  

**注:1.文件表中的每一项都会维护一个引用计数，标识该表项被多少个文件描述符(fd)引用，在引用计数为0的时候，表项才会被删除。所以调用close(fd)关闭子进程的文件描述符，只会减少引用计数，但是不会使文件表项被清除，所以父进程依旧可以访问。  
2.当没有数据可读时，read调用通常会阻塞，即它将暂停进程来等待直到有数据到达为止。如果管道的另一端已被关闭，也就是说没有进程打开这个管道并向它写数据了，此时read调用将会被阻塞。注意，这与读取一个无效的文件描述符不同，read把无效的文件描述符看做一个错误并返回-1.**

在pipe使用中，也可以在子进程中运行一个与其父进程完全不同的另一个程序，而不是仅仅运行一个相同的程序。这个可由exec调用来实现。在上面的例子中，因为子进程本身有file_pipes数据的一个副本，所以这并不成为问题。但经过exec调用后，原来的进程已经被新的子进程替换了。为解决这个问题，可以将文件描述符（实际上是一个数字)作为一个参数传递给exec启动程序。详细实现见:[pipe3.c](https://github.com/ZhangzheBJUT/linux/blob/master/IPC/Pipe/pipe3.c "pipe3.c") 和 [pipe4.c](https://github.com/ZhangzheBJUT/linux/blob/master/IPC/Pipe/pipe4.c "pipe4.c")   

 

###2.3.命名管道FIFO
无名管道只能用在父子进程之间，这些程序由一个共同的祖先进程启动。但如果想在不同进程之间交换数据，这就不太方便。而这可以使用FIFO文件来完成在不相关的进程之间交换数据，它通常叫做命名管道（named pipe)。  
命名管道是一种特殊类型的文件，它在文件系统中以文件名的形式存在，但它的行为却和已经看到过的没有名字的管道类似。

使用下面两个函数可以创建一个FIFO文件:    

	int mkfifo(const char*filename,mode_t mode)
	int mknod(const char* filename,mode_t mode | S_IFIFO,(dev_t)0);

命名管道的一个非常有用特点是：由于它们出现在文件系统中，所以它们可以像平常的文件名一样在命令中使用，使用FIFO只是为了单向传递数据。

使用方法如下:  

	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/stat.h>

	int main()
	{
		int res = mk  fifo("/tmp/my_fifo",0777);
		if (res == 0)
		{
			printf("FIFO created.\n");
		}
		exit(EXIT_SUCCESS);
	}

查看运行结果:
  ![](Screenshots/IPC05.png?raw=true)  
注:输出结果中的第一个字符为p，表示这是一个管道文件。不用的用户掩码最后得到的文件访问权限是不同的(我的用户掩码为0002）。

与通过pipe调用创建管道不同，FIFO是以命名文件的形式存在，而不是打开的文件描述符，所以对它进行读写操作必须先打开它。FIFO也用open和close函数打开和关闭。
对FIFO文件来说，传递给open调用的是FIFO的路径名，而不是一个正常的文件。

打开一个FIFO文件的方法如下:  

	open(const char*path,O_RDONLY);
        在这种情况下，open调用将阻塞，除非有一个进程以写的方式打开同一个FIFO,否则它不会返回。
	open(const char* path,O_RDONLY|O_NONBLOCK)
        在这种情况下，即使没有其它进程以写方式打开FIFO,open调用也将成功并立刻返回。
	open(const char *path,O_WRONLY)
        在这种情况下，open调用将会阻塞，直到有一个进程以读方式打开一个FIFO为止。
	open(const char*path,O_WRONLY|O_NONBLOCK)
        这个函数调用总是立刻返回，但如果没有一个进程以读方式打开FIFO文件，open调用将返回一个错误并且FIFO也不会被打开。
        如果确实有一个进程以读方式打开FIFO文件，那么我们就可以通过它返回的文件描述符对这个FIFO文件进行读写操作。

注意:  

* 使用open打开FIFO文件程序不能以O_RDWR模式打开FIFO文件进行读写操作。这样做的后果是未明确定义。如果确实需要在程序之间双向传递数据，最好使用一对FIFO。
* `O_NONBLOCK` 分别搭配` O_RDONLY` 和 `O_WRONLY`在效果上是不同的，如果没有进程以读方式打开管道，非阻塞写方式的open调用将失败，但非阻塞读方式的open调用总是成功。close调用的行为并不受 `O_NONBLOCK`标志的影响。 

		#include <unistd.h>
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>
		#include <fcntl.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		
		#define FIFO_NAME  "/tmp/my_fifo"
		
		int main(int argc,char* argv[])
		{
			int res;
			int open_mode = 0;
			int i;
		
			if (argc <2)
			{
				fprintf(stderr, "Usage:%s <some combination of \
					O_RDONLY O_WRONLY O_NONBLOCK>\n",*argv );
				exit(EXIT_FAILURE);
			}
		
			for (i = 1; i < argc; ++i)
			{	
				if (strncmp(*++argv,"O_RDONLY",8) == 0)
				{	
					open_mode |= O_RDONLY;
				}
				if (strncmp(*argv,"O_WRONLY",8) == 0	)
				{
					open_mode |= O_WRONLY;
				}
				if (strncmp(*argv,"O_NONBLOCK",10) == 0)
				{
					open_mode |= O_NONBLOCK;
				}
			}
		
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
			res = open(FIFO_NAME,open_mode);
			printf("Process %d result %d\n",getpid(),res);
			sleep(5);
			if (res != -1)
			{
				(void)close(res);
			}
			printf("Process %d finished\n",getpid());
			exit(EXIT_SUCCESS);
		}

使用 `O_NONBLOCK`模式会影响到对FIFO的read和write调用。
  
**小结:**  
**1.  从FIFO中读取数据**
  
*  如果有进程写打开FIFO，且当前FIFO为空，则对于设置了阻塞标志的读操作来说，将一直阻塞下去，直到有数据可以读时才继续执行；对于没有设置阻塞标志的读操作来说，则返回0个字节，当前errno值为EAGAIN，提醒以后再试。    
*  对于设置了阻塞标志的读操作来说，造成阻塞的原因有两种：1、当前FIFO内有数据，但有其它进程在读这些数据；2、FIFO本身为空。
解阻塞的原因是：FIFO中有新的数据写入，不论写入数据量的大小，也不论读操作请求多少数据量，只要有数据写入即可。
*  读打开的阻塞标志只对本进程第一个读操作施加作用，如果本进程中有多个读操作序列，则在第一个读操作被唤醒并完成读操作后，其它将要执行的读操作将不再阻塞，即使在执行读操作时，FIFO中没有数据也一样（此时，读操作返回0）。
*  如果没有进程写打开FIFO，则设置了阻塞标志的读操作会阻塞。
*  如果FIFO中有数据，则设置了阻塞标志的读操作不会因为FIFO中的字节数少于请求的字节数而阻塞，此时，读操作会返回FIFO中现有的数据量。

**2.  从FIFO中写入数据**  
FIFO的长度是需要考虑的一个很重要因素。系统对任一时刻在一个FIFO中可以存在的数据长度是有限制的。它由`#define PIPE_BUF`定义，在头文件`limits.h`	中。在Linux和许多其他类UNIX系统中，它的值通常是4096字节，Red Hat Fedora9下是4096，但在某些系统中它可能会小到512字节。
虽然对于只有一个FIFO写进程和一个FIFO的读进程而言，这个限制并不重要，但只使用一个FIFO并允许多个不同进程向一个FIFO读进程发送请求的情况是很常见的。如果几个不同的程序尝试同时向FIFO写数据，能否保证来自不同程序的数据块不相互交错就非常关键了à也就是说，每个写操作必须“原子化”。
 
*  对于设置了阻塞标志的写操作：
   *  当要写入的数据量不大于`PIPE_BUF`时，Linux将保证写入的原子性。如果此时管道空闲缓冲区不足以容纳要写入的字节数，则进入睡眠，直到当缓冲区中能够容纳要写入的字节数时，才开始进行一次性写操作。即写入的数据长度小于等于`PIPE_BUF`时，那么或者写入全部字节，或者一个字节都不写入，它属于一个一次性行为，具体要看FIFO中是否有足够的缓冲区。
   *  当要写入的数据量大于`PIPE_BUF`时，Linux将不再保证写入的原子性。FIFO缓冲区一有空闲区域，写进程就会试图向管道写入数据，写操作在写完所有请求写的数据后返回。
 
*  对于没有设置阻塞标志的写操作：
   *  当要写入的数据量不大于`PIPE_BUF`时，Linux将保证写入的原子性。如果当前FIFO空闲缓冲区能够容纳请求写入的字节数，写完后成功返回；如果当前FIFO空闲缓冲区不能够容纳请求写入的字节数，则返回EAGAIN错误，提醒以后再写。
   *  当要写入的数据量大于`PIPE_BUF`时，Linux将不再保证写入的原子性。在写满所有FIFO空闲缓冲区后，写操作返回。

