#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"

#define MAX_FD 65536   // 最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000  // 监听的最大的事件数量

// 添加文件描述符
extern void addfd( int epollfd, int fd, bool one_shot );
extern void removefd( int epollfd, int fd );

void addsig(int sig, void( handler )(int)){
    struct sigaction sa; // 信号处理数据结构
    memset( &sa, '\0', sizeof( sa ) ); // 将 sa 的信息写入内存
    sa.sa_handler = handler;
    sigfillset( &sa.sa_mask ); // 将所有信号加入 set 集合
    assert( sigaction( sig, &sa, NULL ) != -1 ); // 其括号内是错误的就终止进程
}

int main( int argc, char* argv[] ) {
    
    if( argc <= 1 ) { // 判断参数个数传入是否正确
        printf( "usage: %s port_number\n", basename(argv[0])); // basename 地址最后的名字，打印地址
        return 1;
    }

    int port = atoi( argv[1] ); // 端口号转换
    addsig( SIGPIPE, SIG_IGN ); // 注册信号

    threadpool< http_conn >* pool = NULL;
    try {
        pool = new threadpool<http_conn>;
    } catch( ... ) {
        return 1;
    }

    http_conn* users = new http_conn[ MAX_FD ]; // 会调用 .h 文件的函数声明（没有调用 .cpp 的）

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 ); // 创建一个用于监听的 socket 

    int ret = 0;
    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons( port );

    // 端口复用
    int reuse = 1;
    setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    ret = listen( listenfd, 5 );

    // 创建epoll对象，和事件数组，添加
    epoll_event events[ MAX_EVENT_NUMBER ]; // 最大的事件监听个数
    int epollfd = epoll_create( 5 ); // 返回 epoll 实例，>0 返回文件描述符，这个 5 像是 epoll 的编号
    // 添加到epoll对象中
    addfd( epollfd, listenfd, false );
    http_conn::m_epollfd = epollfd;

    while(true) {
        
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        
        if ( ( number < 0 ) && ( errno != EINTR ) ) {  // 成功返回 socket 的变化个数，失败返回 -1 
            printf( "epoll failure\n" );
            break;
        }

        for ( int i = 0; i < number; i++ ) {
            
            int sockfd = events[i].data.fd; // 得到变化了的 fd 
            
            if( sockfd == listenfd ) {
                
                // 客户端和服务器进行 accept
                struct sockaddr_in client_address; // 创建空的 client_addr
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );  // 在这时 client_addr 才被赋值，值为客户端的 socket 
                
                if ( connfd < 0 ) {
                    printf( "errno is: %d\n", errno );
                    continue;
                } 

                if( http_conn::m_user_count >= MAX_FD ) {
                    close(connfd);
                    continue;
                }
                users[connfd].init( connfd, client_address); // connfd 的这个值是 5 

            } else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) ) {

                users[sockfd].close_conn();

            } else if(events[i].events & EPOLLIN) { // 监听客户端的连入操作

                if(users[sockfd].read()) { // 
                    pool->append(users + sockfd);
                } else {
                    users[sockfd].close_conn();
                }

            }  else if( events[i].events & EPOLLOUT ) { // 有数据要写

                if( !users[sockfd].write() ) {
                    users[sockfd].close_conn();
                }

            }
        }
    }
    
    close( epollfd );
    close( listenfd );
    delete [] users;
    delete pool;
    return 0;
}