## 五  共享内存
###5.1. 共享内存简介
共享内存指多个进程共享同一块物理内存,它只能用于同一台机器上的两个进程之间的通信。在进程的逻辑地址空间中有一段地址范围是用来进行内存映射使用的，该段逻辑地址空间可以映射到共享的物理内存地址上(进程空间介绍:[http://blog.csdn.net/zhangzhebjut/article/details/39060253](http://blog.csdn.net/zhangzhebjut/article/details/39060253 "进程空间介绍")）。
        

大多数共享内存的具体实现，都是把由不同进程之间共享的内存映射为同一段物理内存。 多个进程都把该物理内存区域映射到自己的虚拟地址空间，这些进程就都可以直接访问该共享内存区域，从而可以通过该区域进行通信。

![](Screenshots/IPC06.jpg?raw=true) 

共享内存允许两个不相关的进程访问同一段物理内存， 由于数据不需要在不同的进程间复制，所以它是在两个正在运行的进程之间传递数据的一种非常有效的方式，一个进程向共享内存区域写入数据，共享该区域的所有进程就可以立刻看到其中的数据内容。  


**注意：  
1.如上图所示，共享虚拟内存的页面，出现在每一个共享该页面的进程的页表中。但是它不需要在所有进程的虚拟内存中都有相同的虚拟地址。  
2.共享内存的同步控制必须由程序员来负责。用共享内存来提供对大块内存区域的有效访问，同时通过传递小道消息来同步对该内存的访问。**

###5.2 共享内存函数
函数原型：  
 
	#include <sys/types.h> 
	#include <sys/ipc.h> 
	#include <sys/shm.h> 
	int   shmget(key_t key, int size, int flag); 
	void* shmat(int shmid,  const void *addr, int flag); 
	int   shmdt(char *shmaddr);
	int   shmctl(int shmid, int cmd, struct shmid_ds *buf);

函数描述：
  
    shmget函数：用于开辟或指向一块共享内存，返回获得共享内存区域的ID，如果不存在指定的共享区域就创建相应的区域。   
               keyt key： 共享内存的标识符。如果是父子关系的进程间通信的话，这个标识符用IPC_PRIVATE来代替。
						  如果两个进程没有任何关系，所以就用ftok()算出来一个标识符（或者自己定义一个）使用了。
               int size： 以字节为单位指定需要共享的内存容量。  
               int flag： 包含9个比特的权限标志，它是这块内存的模式(mode)以及权限标识。  
                          模式可取如下值：        
                     		IPC_CREAT 新建（如果已创建则返回目前共享内存的id）  
                     		IPC_EXCL   与 IPC_CREAT结合使用，如果已创建则返回错误  
                          将“模式” 和“权限标识”进行或运算，做为第三个参数。如：IPC_CREAT | IPC_EXCL | 0640 
                          其中0640为权限标识,4/2/1 分别表示读/写/执行3种权限，第一个0是UID,第一个6（4+2）表示拥
                          有者的权限，第二个4表示同组权限，第3个0表示他人的权限。
              函数调用成功时返回共享内存的ID，失败时返回-1。
			  注：创建共享内存时，shmflg参数至少需要 IPC_CREAT | 权限标识，如果只有IPC_CREAT 则申请的地址都是
                 k=0xffffffff，不能使用；

    shmat函数：用来允许本进程访问一块共享内存的函数。
			  第一次创建共享内存时，它不能任何进程访问，要想启用对该共享内存的访问，必须将其连接到一个进程的地址空间中。
		      shmat函数就是用来完成此工作的。
   			  int   shmid  ： 共享内存的ID，即共享内存的标识。     
   			  char *shmaddr： 共享内存连接到进程中的起始地址，如果shmaddr为NULL，内核会把共享内存映射到系统选定的地
                              址空间中；如果shmaddr不为NULL，内核会把共享内存映射到shmaddr指定的位置。
                              注：一般情况下我们很少需要控制共享内存连接的地址，通常都是让系统来选择一个地址，否则就会使应
                                 用程序对硬件的依赖性过高。所以一般把shmaddr设为NULL。
    		  int shmflag ：  本进程对该内存的操作模式，可以由两个取值：SHM_RND和SHM_RDONLY。SHM_RND为读写模式，
                              SHM_RDONLY是只读模式。需要注意的是，共享内存的读写权限由它的属主、它的访问权限和当
                              前进程的属主共同决定。如果当shmflg & SM_RDONLY为true时，即使该共享内存的访问权限允许写操
                              作，它也不能被写入。该参数通常会被设为0。
  			 函数调用成功时，返回共享内存的起始地址，失败时返回-1。

    shmdt函数：用于函数删除本进程对这块内存的使用。
              shmdt()与shmat()相反，是用来禁止本进程访问一块共享内存的函数。 
    	      char *shmaddr  是那块共享内存的起始地址。
              函数调用成功时返回0，失败时返回-1。
   
    shmctl函数： 控制对这块共享内存的使用。
    		    int shmid： 共享内存的ID，即共享内存标识。
                int cmd  ： 控制命令，表示要采取的动作，可取值如下：
        			IPC_STAT  得到共享内存的状态：把shmid_ds结构中的数据设置为共享内存的当前关联值
        			IPC_SET   改变共享内存的状态：把共享内存的当前关联值设置为shmid_ds结构中给出的值
        			IPC_RMID  删除共享内存段
		          shmid_ds结构至少包含以下成员：
      			      struct shmid_ds {
         			       uid_t shm_perm.uid;
         			       uid_t shm_perm.gid;
         			       uid_t shm_perm.mode;
  			          }
			    struct shmid_ds *buf： 一个结构体指针。IPC_STAT的时候，取得的状态放在这个结构体中。
				 				      如果要改变共享内存的状态，用这个结构体指定。
    		 函数调用成功时返回0，失败时返回-1。

###5.3 使用实例
    shm1.c
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	
	#include <sys/shm.h>
	#include "shm_com.h"
	
	int main()
	{
		int running  = 1;
		void *shared_memory = (void*) 0;
		struct shared_use_st *shared_stuff;
		int shmid;
	
		srand((unsigned int)getpid());
	
		shmid = shmget((key_t)1234,sizeof(struct shared_use_st),0666|IPC_CREAT);
	
		if(shmid == -1)
		{
			fprintf(stderr,"shmget failed\n");
			exit(EXIT_FAILURE);
		}
	
		shared_memory = shmat(shmid,(void*)0,0);
		if(shared_memory==(void*)-1) {
			fprintf(stderr,"shmat failed\n");
			exit(EXIT_FAILURE);
		}
	
		printf("Memory attached at %X\n",(int)shared_memory);
	
		shared_stuff = (struct shared_use_st*)shared_memory;
		shared_stuff->written_by_you = 0;
	
		while(running) 
		{
			if(shared_stuff->written_by_you)
			{
				printf("You wrote:%s",shared_stuff->some_text);
				sleep(rand()%4);
				shared_stuff->written_by_you = 0;
				if (strncmp(shared_stuff->some_text,"end",3) == 0) 
				{
						running = 0;
				}
			}
		}
	    
		if (shmdt(shared_memory) == -1)
		{
		    fprintf(stderr,"shmdt faied\n");
		}
	
		if (shmctl(shmid,IPC_RMID,0) == -1)
		{
		    fprintf(stderr,"shmctl(IPC_RMID) failed\n");
			exit(EXIT_FAILURE);
		}
	
	
		exit(EXIT_FAILURE);
	
	}
    
    shm2.c
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	
	#include <sys/shm.h>
	#include "shm_com.h"
	
	int main()
	{
		int running = 1;
		void *shared_memory = (void*)0;
		struct shared_use_st *shared_stuff;
		char buffer[BUFSIZ];
	
		int shmid;
	
	    shmid = shmget((key_t)1234,sizeof(struct shared_use_st),0666|IPC_CREAT);
		if(shmid == -1)
		{
			fprintf(stderr,"shmget failed.\n");
			exit(EXIT_FAILURE);
		}
	
		shared_memory = shmat(shmid,(void*)0,0);
		if(shared_memory == (void*)-1)
		{
			fprintf(stderr,"shmat failed.\n");
			exit(EXIT_FAILURE);
		}
	
		printf("Memory attached at %X\n",(int)shared_memory);
	
		shared_stuff = (struct shared_use_st*)shared_memory;
		while(running)
		{
			while(shared_stuff->written_by_you==1)
			{
				sleep(1);
				printf("Waiting for client...\n");
			}
	
			printf("Enter some text:");
			fgets(buffer,BUFSIZ,stdin);
	
			strncpy(shared_stuff->some_text,buffer,TEXT_SZ);
	        shared_stuff->written_by_you = 1;
	
			if(strncmp(buffer,"end",3) == 0) 
			{
				running  = 0;
			}
	
		}
	
	    if (shmdt(shared_memory) == -1)
		{
			fprintf(stderr,"shmdt failed.\n");
			exit(EXIT_FAILURE);
		}
	    
		exit(EXIT_FAILURE);
	}
 可以使用`ipc -m` 命令来查看系统的共享内存情况：  
   ![](Screenshots/shm.png?raw=true)
###5.4 小结
共享内存允许一个或多个进程通过同时出现在它们虚拟地址空间中的内存来通讯,此虚拟内存的页面出现在每个共享进程页表中。但此页面并不一定位于所有共享进程虚拟内存的相同位置。和其它系统V IPC对象的使用方法一样，对共享内存区域的访问是通过键和访问权限检验来控制的。一旦内存被共享，则再不会检验进程对对象的使用方式。它依赖于其它机制，如系统V信号灯，来同步对共享内存的访问。

每个新创建的共享内存区域由一个`shmid_ds`数据结构来表示。它们被保存在`shm_segs`数组中。 `shmid_ds`数据结构描叙共享内存的大小，进程如何使用以及共享内存映射到其各自地址空间的方式。由共享内存创建者控制对此内存的存取权限以及其键是公有还是私有。如果它由足够权限，它还可以将此共享内存加载到物理内存中。

每个使用此共享内存的进程必须通过系统调用将其连接到虚拟内存上。这时进程创建新的`vm_area_struct`来描叙此共享内存。进程可以决定此共享内存在其虚拟地址空间的位置，或者让Linux选择一块足够大的区域。 新的`vm_area_struct`结构将被放到由`shmid_ds`指向的`vm_area_struct`链表中。通过`vm_next_share`d和`vm_prev_shared` 指针将它们连接起来。**虚拟内存在连接时并没有创建,进程访问它时才创建。**

   ![](Screenshots/shm.gif?raw=true)

当进程首次访问共享虚拟内存中的页面时将产生缺页错误。当取回此页面后，Linux找到了描叙此页面的`vm_area_struct`数据结构。它包含指向使用此种类型虚拟内存的处理函数地址指针。共享内存页面错误处理代码将在此`shmid_ds`对应的页表入口链表中寻找是否存在此共享虚拟内存页面。如果不存在，则它将分配物理页面并为其创建页表入口。同时还将它放入当前进程的页表中，此入口被保存在`shmid_ds`结构中。这意味着下个试图访问此内存的进程还会产生页面错误，共享内存错误处理函数将为此进程使用其新创建的物理页面。这样，第一个访问虚拟内存页面的进程创建这块内存，随后的进程把此页面加入到各自的虚拟地址空间中。

当进程不再共享此虚拟内存时，进程和共享内存的连接将被断开。如果其它进程还在使用这个内存，则此操作只影响当前进程。其对应的`vm_area_struct`结构将从`shmid_ds`结构中删除并回收。当前进程对应此共享内存地址的页表入口也将被更新并置为无效。当最后一个进程断开与共享内存的连接时，当前位于物理内存中的共享内存页面将被释放，同时释放的还有此共享内存的`shmid_ds`结构。

## 六  消息队列
###6.1. 消息队列简介
消息队列用于运行于同一台机器上的进程间通信，它和管道很相似。消息队列提供了一种在两个不相关的进程之间传递数据的相当简单而且有效的方法，与命名管道相比，消息队列的优势在于，它独立于发送和接收进程而存在，这消除了在同步命名管道的打开和关闭时产生的一些困难。  
 
通过使用消息队列，发送消息时几乎可以完全避免命名管道的同步和阻塞问题，不再需要进程来提供同步方法，而且还可以用一些方法提前查看紧急消息。与管道一样，使用消息队列的限制是，每个数据块都有一个最大长度限制，系统中所有队列所包含的全部数据块的总长度也有一个上限。  

消息队列提供了一种从一个进程向另一个进程发送一个数据块的方法。而且每个数据块必须以一个长整型成员变量开始，该变量成员可以用来标识消息的种类。接收进程可以独立的接收含有不同类型的数据块，从而实现对消息的过滤。

###6.2 消息队列函数

函数原型：  
 
	#include <sys/types.h> 
	#include <sys/ipc.h> 
	#include <sys/msg.h> 
	int   msgget(key_t key,int msgflg); 
	int   msgsnd(int msqid,const void *msg_ptr,size_t msg_sz,int msgflg);
	int   msgrcv(int msqid,void *msg_ptr,size_t msg_sz,long int msgtype,int msgflg);
	int   msgctl(int msqid,int cmd, struct msqid_ds *buf);

函数描述：

    msgget函数：系统调用用来创建一个消息队列。
               key_t key : 是一个长整型，可以自己设定或通过 ftok() 获得。
               int msgflg: 八进制的消息队列操作权和控制命令的组合。
                              其中操作权定义为：
							    用户可读0400
								用户可写0200
								同组可读0040
								同组可写0020
								其它可读0004
								其它可写0002
                                操作权可相加而派生，如用户可"读"、"写"的权限为：0400|0200=0600
                             控制命令可以取：IPC_CREAT或IPC_EXCL
                          如果要创建一个key=888且属主和同组可读写的消息队列,执行以下系统调用msgget(0x888,0660|IPC_CREAT)。
    			函数调用成功时返回一个正整数，即队列标识符，用来识别或引用相关的消息队列和数据结构。失败时返回-1。

    msgsnd函数：用于向消息队列中发送一条信息。
               int msqid：消息队列描述符，由 msgget 函数调用创建。
               const void *msg_ptr：	指向消息队列的指针，该指针所指的结构含有消息的类型和要发送或接受消息内容。
                                    struct msgbuf {
						               longmtype;       /*消息类型*/
						               charmtext[2048]; /*消息正文*/
					                } 
                                    注：消息必须要以一个长整型变量开始。
                
               size_t msg_sz：消息的长度。
                              注：它不包括消息结构体中的长整型成员变量的长度。
               int msgflg：控制当前消息队列满或队列消息达到系统范围的限制时将要发生的事情。
						   当消息队列满时(队列中无空闲空间):
                                    如果 msgflg&IPC_NOWAIT= 真，调用进程立即返回,不发送该消息。
                                    如果 msgflg&IPC_NOWAIT= 假，调用进程暂停执行,处于"挂起"状态,且不发送该消息。直到下列情况之一出现:
    								    引起暂停的条件不再存在,如队列出现空闲。
                                        调用进程接收到一个要捕捉的信号,如中断信号,此时不发送消息,调用进程按signal中描述的方式执行。
    			函数调用成功返回0，失败返回-1，具体错误类型可具体查看errno。

    msgrcv函数：用于从消息队列中获取一条消息。
               int msgid    ： 消息队列描述符，由 msgget 函数调用创建。
               void *msg_ptr： 指向消息队列的指针，该指针所指的结构含有消息的类型和要发送或接受消息内容。
               size_t mgs_sz： 消息的长度，它不包括消息结构体中的长整型成员变量的长度。
                               注：如果所接收的消息比msg_sz大且msgflg&MSG_NOERROR为真，则按msgsz的大小截断而不通知调用进程。
               long int msgtype：用于指定接受消息的类型，它可以实现一种简单形式的接受优先级。
                                 可取参数类型如下:
                                         msgtyp=0 接收消息队列中的第一个消息,即获取队列中的第一个可用消息。
    									 msgtyp>0 接收消息队列中的类型为msgtyp 的第一个消息。
     									 msgtyp<0 接收消息队列中小于等于msgtyp 绝对值的第一个消息。
                                 如果只是想按照消息发送的顺序来接受它们，就把msgtype设置为0。如果只是想获取某一特定类型的消息，就把
                                 msgtype设置为相应类型值。如果想接受类型等于或小于n的消息，就把msgtype设置为-n。
               int msgflg： 用于控制当队列上没有所期望类型的消息或消息队列为空时调用进程要采取的行动。
                            如果 msgflg&IPC_NOWAIT 为真,则调用进程立即结束并返回-1。
                            如果 msgflg&IPC_NOWAIT 为假,则调用进程暂停执行，直至出现：
    						              队列中放入所需类型的消息,调用进程接收该消息
     						              msqid消息队列从系统中删除
     						              调用进程接收到捕获的信号,此时不接收消息,调用进程按signal描述的方式执行
    			函数调用成功，返回放入接收缓存区中的字节数,失败返回-1,具体错误类型可查errno。

    msgctl函数：用于消息队列的控制。
               int msqid： 消息队列描述符，由 msgget 函数调用创建。
               int cmd  ： 指出将要采取的动作：
                                IPC_STAT  查看消息队列的状态,将msqid_ds结构中的数据设置为消息队列的当前关联值。
     							IPC_SET   设置消息队列的状态,将消息队列中的关联值设置为msqid_ds结构中给出的值。
     							IPC_RMID  删除指定的msqid以及相关的消息队列和结构
               struct msqid_ds *buf：结构体指针，用于存储消息队列的访问控制模式和状态。
    		   函数调用成功返回0，失败返回-1.如果删除消息队列时，某个进程正在msgsnd和msgrcv函数中等待，这两个函数将失败。 

###6.3 使用实例
	msg1.c
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	
	
	#include <sys/msg.h>
	
	
	struct my_msg_st {
		long int my_msg_type;
		char some_text[BUFSIZ];
	};
	
	
	int main()
	{
		int running = 1;
		int msgid;
		struct my_msg_st some_data;
		long int msg_to_receive = 0;
	
		msgid = msgget((key_t)1234,0666 | IPC_CREAT);
		if (msgid == -1)
		{
			fprintf(stderr,"msgget failed with error: %d\n",errno);
			exit(EXIT_FAILURE);
		}
	
	
		while(running)
		{
			if(msgrcv(msgid,(void*)&some_data,BUFSIZ,msg_to_receive,0) == -1)
			{
				fprintf(stderr,"msgrcv failed with error: %d\n",errno);
				exit(EXIT_FAILURE);
			}
	
			printf("You wrote:%s",some_data.some_text);
	
			if(strncmp(some_data.some_text,"end",3) == 0)
			{
				running = 0;
			}
	
		}
	     
	    if (msgctl(msgid,IPC_RMID,0) == -1 ) 
	    {
			fprintf(stderr,"msgctl(IPC_RMID) failed.\n");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_FAILURE);
	}
   
    msg2.c
    #include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
	
	
	#include <sys/msg.h>
	
	#define MAX_TEXT 512
	
	struct my_msg_st {
		long int my_msg_type;
		char some_text[BUFSIZ];
	};
	
	
	int main()
	{
		int running = 1;
		int msgid;
		struct my_msg_st some_data;
	    char buffer[BUFSIZ];
	
		msgid = msgget((key_t)1234,0666 | IPC_CREAT);
		if (msgid == -1)
		{
			fprintf(stderr,"msgget failed with error: %d\n",errno);
			exit(EXIT_FAILURE);
		}
	
	
		while(running)
		{
			printf("Enter som text:");
			fgets(buffer,BUFSIZ,stdin);
			some_data.my_msg_type = 1;
			strcpy(some_data.some_text,buffer);
	
			if(msgsnd(msgid,(void*)&some_data,MAX_TEXT,0) == -1)
			{
				fprintf(stderr,"msgsnd failed.\n"); 
				exit(EXIT_FAILURE);
			}
	
	
			if(strncmp(some_data.some_text,"end",3) == 0)
			{
				running = 0;
			}
	
	    }
		
	   exit(EXIT_FAILURE);
	}
 可以使用`ipc -q` 命令来查看系统的消息队列：  
   ![](Screenshots/msg.png?raw=true)
###6.4 小结
消息队列就是一个消息的链表。可以把消息看作一个个记录或者报文，这些记录具有特定的格式以及特定的优先级。对消息队列有写权限的进程可以向其中按照一定的规则添加新消息；对消息队列有读权限的进程则可以从消息队列中读走消息。每个消息的最大长度有一个上限值，由MSGMAX定义，每个消息队列的总的字节数有上限制，由MSGMNB定义，系统中消息队列总数有一个上限值，由MSGMNI定义。  
    
Linux采用消息队列的方式来实现消息传递。这种消息的发送方式是：发送方不必等待接收方检查它所收到的消息就可以继续工作下去，而接收方如果没有收到消息也不需等待。新的消息总是放在队列的末尾，接收的时候并不总是从头来接收，可以从中间来接收。虽然这种通信机制相对简单，但是应用程序使用起来就需要使用相对复杂的方式来应付了。

消息队列是随内核持续的并和进程相关，只有在内核重起或者显示删除一个消息队列时，该消息队列才会真正被删除。因此系统中记录消息队列的数据结构 (`struct ipc_ids msg_ids`) 位于内核中，系统中的所有消息队列都可以在结构msg_ids中找到访问入口。

消息队列允许一个或者多个进程向它写入与读取消息。Linux维护着一个`msgque`消息队列链表，其中每个元素 指向一个描叙消息队列的`msqid_ds`结构。当创建新的消息队列时，系统将从系统内存中分配一个`msqid_ds`结构，同时将其插入到数组中。

   ![](Screenshots/msg.gif?raw=true)

每个`msqid_ds`结构包含一个`ipc_perm`结构和指向已经进入此队列消息的指针。另外，Linux保留有关队列修改时间信息，如上次系统向队列中写入的时间等。`msqid_ds`包含两个等待队列：一个为队列写入进程使用而另一个由队列读取进程使用。

每次进程试图向写入队列写入消息时，系统将把其有效用户和组标志符与此队列的`ipc_perm`结构中的模式进行比较。如果允许写入操作，则把此消息从此进程的地址空间拷贝到msg数据结构中，并放置到此消息队列尾部。由于 Linux严格限制可写入消息的个数和长度，队列中可能容纳不下这个消息。此时，此写入进程将被添加到这个消息队列的等待队列中，同时调用调度管理器选择新进程运行。当由消息从此队列中释放时，该进程将被唤醒。

从队列中读的过程与之类似。进程对这个写入队列的访问权限将被再次检验。读取进程将选择队列中第一个消息（不管是什么类型）或者第一个某特定类型的消息。如果没有消息可以满足此要求，读取进程将被添加 到消息队列的读取等待队列中，然后系统运行调度管理器。当有新消息写入队列时，进程将被唤醒继续执行。