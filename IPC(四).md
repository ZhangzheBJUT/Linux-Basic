## 七  套接字
###7.1. 套接字简介
之前所讨论的IPC机制都是依靠一台计算机共享系统资源实现的，这些资源可以是文件系统(命名管道)、共享的物理内存(共享内存)和消息队列，这些只有运行在同一台机器上的进程可以使用。

伯克利版本的UNIX系统引入了一种新的通信工具-套接字接口(socket interface)。一台机器上的进程可以通过使用套接字和另外一台机器上的进程通信，这样就可以支持分布在网络中的客户/服务器系统。同一台机器上的进程也可以直接通过套接字进行通信。

套接字是一种通信机制，凭借这种机制客户/服务器系统的开发工作既可以在本地单机上进行，也可以跨网络进行。Linux所提供的功能(如打印服务、连接数据库和提供Web页面)和网络工具(如用于远程登录的rlogin和用于文件传输的ftp)通常都是通过套接字来进行通信的。


![](Screenshots/socket.jpg?raw=true) 

套接字的工作过程如上图所示：  
**对于服务端来说** 
 
1. 服务器进程使用系统调用socket来创建一个套接字，它是系统分配给该服务器进程的类似文件描述符的资源，不能与其它进程共享。
2. 服务器进程使用系统调用bind来给套接字起个名字。本地套接字的名字是Linux文件系统中的文件名，一般放在/tmp或/usr/tmp目录中。对于网络套接字，它的名字是与客户端连接的特定网络有关的服务标识符(端口号或访问点)。这个标识符允许Linux将进入的针对特定端口号的连接转到正确的服务器进程。例如，Web服务器一般在80端口上创建一个套接字，它是一个专门用于此目的的标识符。而Web客户端(比如：浏览器)知道对于用户想要访问的Web的站点应该使用端口号80来建立http连接。
3. 服务器进程使用系统调用listen创建一个队列并将其用于存放来自客户的进入连接。此时，服务端开始等待客户端的连接。
4. 服务器进程使用系统调用accept接受客户端的连接。在调用accept时，它会创建一个与原来的命名套接字不同的新的套接字。这个新套接字只用于与这个特定的客户端进行通信，而命名套接字则被保留下来继续处理来自其它客户的连接。      

**对于客户端来说**    

1. 基于套接字系统的客户端更加简单，客户首先调用socket创建一个未命名的套接字。
2. 然后将服务器的命名套接字作为一个地址来调用connect与服务端建立连接。
3. 一旦建立连接，就可以像是使用底层的文件描述符那样用套接字来实现双向的数据通信。
4. 在使用完后双方调用close来关闭连接

###7.2 socket相关函数
函数原型：  
 
	#include <sys/types.h>
    #include <sys/socket.h>


    int socket(int domain,int type,int protocol);
    int bind(int socke,const struct sockaddr *address,size_t address_len);
    int listen(int socket,int backlog);
    int accept(int socket,0 struct sockaddr *address,size_t address_len);
    int connetc(int socket,const struct sockaddr *address,size_t address_len);
    int close(int socket)

函数描述：
  
    socket函数：创建套接字
               创建一个套接字并返回一个描述符，该描述符可以用来访问该套接字。
               套接字由三个属性确定：域(协议族 protocol family)，类型和协议。   

                  domain（域）： 指定套接字通信中使用的网络介质，最常见的套接字是AF_INET和AF_UNIX，前者指的是
                                Internet网络。AF_UNIX是利用UNIX或Linux文件系统实现本地套接字，这个域的底层
								协议就是文件的输入\输出，而它的地址就是文件名。 其它的域还包括：AF_ISO 和 AF_XNS。
                    type(类型)： 分为流套接字和数据包套接字
    								流套接字 （SOCK_STREAM）：提供一个可靠的、有序的、双向的字节流的连接。
    								数据报套接字(SOCK_DGRAM)：无序的、不可靠、 但速度快是基于数据报的服务。服务器通常不保
															留连接信息，所以它们可以在不打扰其客户的前提下停止并重启。
                protocol(协议)： 底层传输机制允许不止一个协议来提供要求的套接字类型，可以为套接字选择一个特定的协议。

    bind函数：命名套接字
             将参数address中的地址分配给文件描述符socket关联的未命名的套接字，地址结构长度由参数address_len指定。
             命名对于AF_UNIX域的套接字来说就是关联到一个文件系统的路径名，对于AF_INET域的套接字来说就是关联到一个IP端口号。
             端口号用来标识服务,知名服务所分配的端口号所在Linux和UNIX机器上都有一样的，它们通常小于1024。
             套接字地址都有由一个套接字域的成员开始，每个套接字域都有自己的地址格式，对于AF_UNIX套接字来说，它的地址由结构
             sockaddr_un来描述，该结构定义在头文件sys/un.h中：
                     struct sockaddr_un{
                          sa_family_t sum_family;
                          char sunpath[];
                     }
  			 AF_INET由sockaddr_in来指定，包括套接字域、IP地址和端口号
   					struct sockaddr_in{
     					 short int sin_family;  //AF_INET
     					 unsigned short int;    //Prot Number
     					 struct in_addr;        //Internet address
   					}
   					struct in_addr {
      					 unsigned long int s_addr;
  				    } 
   			 地址的长度和格式取决于地址组，bind需要将一个特定的地址结构指针转化为指向通用地址类型(struct sockaddr*)。
  			 函数调用成功时，返回0，失败时返回-1。

      listen函数：创建套接字队列
			     创建套接字缓存队列，用于设置等待处理的进入连接的个数
    	         等待处理的进入连接的个数最多不能超过这个数字，再往后的连接将被拒绝，导致客户的连接请求失败。
                 函数调用成功时返回0，失败时返回-1。
   
    connetc函数： 请求连接
    		     参数socket指定的套接字将连接到参数address指定的服务器套接字，address指向的结构的长度由参数address_len指定。
                 注：参数socket指定的套接字必须是通过socket调用获得的一个有效的文件描述符。
                 成功时返回０,失败返回-1，可能的错误码：
				      EBADF          文件描述符无效
					  EALREADY　　　　该套接字上已有一个正在进行的中的连接
					  ETIMEDOUT　　　 连接超时
			          ECONNREFUSED　 连接请求被服务器拒绝

	 accept函数： 接受连接
                 将创建一个新的套接字来与客户端通信，并且返回新套接字的描述符。新套接字的类型和服务器
                 监听套接字类型是一样的。
                 连接客户的地址被放入address参数指定的sockaddr结构中，如果不关心客户的地址，可以将address参数指针设为空。参数
                 address_len指定客户结构的长度，如果客户地址的长度超过这个值，它将被截断。在调用accept之前，address_len必须
                 设置为预期的地址长度。当这个调用返回时，address_len将被设置为连接客户地址结构的实际长度。
                 如果队列中没有未处理的连接请求，accept会阻塞直到有客户请求建立连接为止。这一行为可以被改变，例如用

                    int flag = fcntl(socket, F_GETFL, O_NONBLOCK);
                  
				 fcntl函数将套接字描述符的flags设为O_NONBLOCK，因此队列中没有未处理的连接时，accept不会
				 阻塞，而是返回-1,并设errno为EWOULDBLOCK，如果accept被信号中断，errno则设为EINTR。可以
				 使用fcntl函数将套接字描述符的标志设回０。      
             
      close函数：关闭套接字
			    用来终止服务器和客户上的套接字连接，就如同对底层文件描述符进行关闭一样。

###7.3 使用实例
    AF_UNIX类型套接字
      client1.c
    #include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	
	int main()
	{
	    int sockfd;
	    int len;
	    struct sockaddr_un address;
	    int result;
	    char ch = 'A';
	    //create socket 
	    sockfd = socket(AF_UNIX,SOCK_STREAM,0);
	    address.sun_family = AF_UNIX;
	    strcpy(address.sun_path,"server_socket");
	    len = sizeof(address);
	
	    //连接套接字
	    result = connect(sockfd,(struct sockaddr*)&address,len);
	    
	    if (result==-1)
	    {
	         perror("oops:client");
		 exit(1);
	    }
	    write(sockfd,&ch,1);
	    read(sockfd,&ch,1);
	    printf("char from server = %c\n",ch);
	    close(sockfd);
	    exit(0);
	}
    server1.c
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <stdio.h>
	#include <sys/un.h>
	#include <unistd.h>
	#include <stdlib.h>
	
	
	int main()
	{
	    int server_sockfd,client_sockfd;
	    int server_len,client_len;
	
	    struct sockaddr_un server_address;
	    struct sockaddr_un client_address;
	    
	    unlink("server_socket");
	    //创建套接字
	    server_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
	    server_address.sun_family = AF_UNIX;
	    strcpy(server_address.sun_path,"server_socket");
	    server_len = sizeof(server_address);
	
	    //命名套接字
	    bind(server_sockfd,(struct sockaddr*)&server_address,server_len);
	    
	    //创建套接字队列
	    listen(server_sockfd,5);
	
	    //接受连接
	    while(1)
	    {
	       char ch;
	       printf("server waiting\n");
	       client_len = sizeof(client_address);
	       client_sockfd=accept(server_sockfd,(struct sockaddr*)&client_address,&client_len);
	       read(client_sockfd,&ch,1);
	       printf("get data from server:%c\n",ch);
	       ch++;
	     
	       write(client_sockfd,&ch,1);
	       //关闭套接字
	       close(client_sockfd);
	     }
	}
    AF_NET类型套接字
      client3.c
    #include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	
	
	int main()
	{
	    int sockfd;
	    int len;
	    struct sockaddr_in address;
	    int result;
	    char ch= 'A';
	    
	
	    sockfd = socket(AF_INET,SOCK_STREAM,0);
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    address.sin_port = htons(9734);//9734;
	    len = sizeof(address);
	    
	     //连接套接字
	    result = connect(sockfd,(struct sockaddr*)&address,len);
	    
	    if (result==-1)
	    {
	         perror("oops:client");
		 exit(1);
	    }
	
	    write(sockfd,&ch,1);
	    read(sockfd,&ch,1);
	    printf("char from server = %c\n",ch);
	    close(sockfd);
	    exit(0);
	}
    server3.c
    #include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	
	
	
	int main()
	{
	    int server_sockfd,client_sockfd;
	    int server_len,client_len;
	    struct sockaddr_in server_address;
	    struct sockaddr_in client_address;
	    
	    server_sockfd = socket(AF_INET,SOCK_STREAM,0);
	    server_address.sin_family= AF_INET;
	    server_address.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("127.0.0.1");
	    server_address.sin_port = htons(9734);//9734;
	    server_len  = sizeof(server_address);
	    bind(server_sockfd,(struct sockaddr*)&server_address,server_len);
	    
	     
	    //创建套接字队列
	    listen(server_sockfd,5);
	
	    //接受连接
	    while(1)
	    {
	       char ch;
	       printf("server waiting\n");
	       client_len = sizeof(client_address);
	       client_sockfd=accept(server_sockfd,(struct sockaddr*)&client_address,&client_len);
	       read(client_sockfd,&ch,1);
	       printf("get data from server:%c\n",ch);
	       ch++;
	     
	       write(client_sockfd,&ch,1);
	       //关闭套接字
	       close(client_sockfd);
	     }
	}


![](Screenshots/netstat.png?raw=true) 

使用netstat命令来查看网络连接状况。  
上面的命令用于显示客户/服务器正在等待关闭。  

* Proto 是 Protocol 的简称,它可以是TCP或UDP。
* Recv-Q和Send-Q指的是接收队列和发送队列,这些数字一般都应该是 0,如果不是,则表示软件包正在队列中堆积。
* Local Address指本机的IP和端口号。
* Foreign Address指所要连接的主机名称和服务。
* State指现在连接的状态。三种常见的 TCP 状态如下所示:
  * LISTEN 等待接收连接;
  * ESTABLISHED 一个处于活跃状态的连接;
  * TIME_WAIT 一个刚被终止的连接。它只持续1至2分钟,然后就会变成LISTEN状态。由于 UDP 是无状态的,所以其 State 栏总是空白。
 


此外:

	netstat -a 显示所有的服务,列出所有端口 (包括监听与非监听端口)，显示结果可能会有数百行
    netstat --inet -a  显示结果将只有网络连接,包括所有正处在"LISTEN"状态和"ESTABLISHED"状态的连接    
	netstat -n 查看端口的网络连接情况，常用netstat -an    
	netstat -v 查看正在进行的工作    
	netstat -p 协议名 例：netstat -p tcq/ip 查看某协议使用情况（查看tcp/ip协议使用情况） 
	netstat -s 查看正在使用的所有协议使用情况 


**注：主机字节序和网路字节序(大端模式）:
       通过套接字传递的端口号和地址都是二进制数字，不同的计算机使用不同的字节序来表示整数。如果计算机上的主机字序和网络字序相同，你将不会看到任何差异。为了使不同类型的计算机就可以通过网络传输的多字节整数的值达成一致，需要定义网络字节。客户端和服务端在必须在传输之前，将它们的内部整数表示方式转换为网络字节序。**

###7.4 小结
   以上只是网络通信的最基本的模型，在实际应用中为了提高并发效率会使用select、poll、epoll等网络模型。

## 八  IPC 小结

* 管道( pipe )：管道是一种半双工的通信方式，数据只能单向流动，而且只能在具有亲缘关系的进程间使用，进程的亲缘关系通常是指父子进程关系。
有名管道 (FIFO or named pipe) ： 有名管道也是半双工的通信方式，但是它允许无亲缘关系进程间的通信。
* 信号量( semophore ) ： 信号量是一个计数器，可以用来控制多个进程对共享资源的访问。它常作为一种锁机制，防止某进程正在访问共享资源时，其它进程也访问该资源。因此，主要作为进程间以及同一进程内不同线程之间的同步手段。
* 信号 ( sinal ) ： 信号是一种比较复杂的通信方式，用于通知接收进程某个事件已经发生。
* 共享内存( shared memory ) ：共享内存就是映射一段能被其它进程所访问的内存，这段共享内存由一个进程创建，但多个进程都可以访问。共享内存是最快的IPC方式，它是针对其它进程间通信方式运行效率低而专门设计的。它往往与其他通信机制，如信号量，配合使用，来实现进程间的同步和通信。
* 消息队列( message queue ) ： 消息队列是由消息的链表，存放在内核中并由消息队列标识符标识。消息队列克服了信号传递信息少、管道只能承载无格式字节流以及缓冲区大小受限等缺点。但是消息队列和管道的使用都会受到系统的限制(包括创建个数、传递消息的长度等)。
* 套接字( socket ) ： 套接字也是一种进程间通信机制，与其它通信机制最大的不同的是，它可用于不同机器间的进程通信。