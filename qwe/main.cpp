
/*客户端   两个线程 主线程发送信息 副线程接受信息*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<string.h>

#define PORT 1234
#define MAXDATASIZE 100

char sendbuf[1024];
char recvbuf[1024];
char name[100];
int fd;
//副线程 接受消息
void pthread_recv(void* ptr)
{
    while(1)
    {
        if ((recv(fd,recvbuf,MAXDATASIZE,0)) == -1){
            printf("recv() error\n");
            exit(1);
        }
        printf("%s",recvbuf);
        memset(recvbuf,0,sizeof(recvbuf));
    }
}



int main(int argc, char *argv[])
{
    //int  numbytes;
    //char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in server;
    
    //设置需要输入IP地址参数
    if (argc !=2) {printf("Usage: %s <IP Address>\n",argv[0]);
        exit(1);
    }
    
    //
    if ((he=gethostbyname(argv[1]))==NULL){
        printf("gethostbyname() error\n");
        exit(1);
    }
    //创建套接字
    if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){
        printf("socket() error\n");
        exit(1);
    }
    //清空 可以用memset代替
    bzero(&server,sizeof(server));
    
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    //connect 函数
    if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
        printf("connect() error\n");
        exit(1);
    }
    
    printf("connect success\n");
    char str[]="已进入聊天室\n";
    printf("请输入用户名：");
    //从 目标文件流 file 中读取 n-1 个字符，放入以 buf 起始地址的内存空间中。stdin 键盘输入文件
    fgets(name,sizeof(name),stdin);
    send(fd,name,(strlen(name)-1),0);
    send(fd,str,(strlen(str)),0);
    
    
    pthread_t tid;
    //
    pthread_create(&tid,NULL,pthread_recv,NULL);
    
    while(1)
    {
        //清空套接字缓存区域
        memset(sendbuf,0,sizeof(sendbuf));
        //读取输入的字符到sendbuf
        fgets(sendbuf,sizeof(sendbuf),stdin);
        //退出
        if(strcmp(sendbuf,"exit")==0){
            memset(sendbuf,0,sizeof(sendbuf));
            printf("您已退出群聊\n");
            send(fd,sendbuf,(strlen(sendbuf)),0);
            break;
        }
        send(fd,name,(strlen(name)-1),0);
        send(fd,":",1,0);
        send(fd,sendbuf,(strlen(sendbuf)),0);
        
        
    }
    close(fd);
}
