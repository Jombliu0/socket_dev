#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "socket_dev_client.h"


static int open_dev(int fd , char * ip, int port)
{
	int ret = 0;
	char ip_addr[64];
	unsigned short s_port;
	struct sockaddr_in their_addr;
	
   	printf("open_dev\r\n"); 
	
	s_port = port; 
	strncpy( ip_addr, ip, sizeof(ip_addr) );

	//step 2:设置服务器地址和端口
	memset( &their_addr, 0, sizeof(their_addr) );
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(s_port);
	their_addr.sin_addr.s_addr = inet_addr(ip_addr);
	printf("connect server %s: %d ... \r\n",ip_addr,port);
	//step 3:和服务器建立连接
	if (connect( fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr) ) == -1)
	{
		perror("connect fail\r\n");
		return -1;
	}

	return ret;
}

static int read_dev(int fd,char *buf ,int size , int flag)
{
	int len = 0;

	printf("read_dev\r\n");

	len = recv( fd, buf, size , flag);

	return len;
}

static int write_dev(int fd, char *buf, int size, int flag)
{
	int ret = 0;

	printf("write_dev\r\n");
	printf("write name: %s,len: %d\n\r",buf,size);
	
	if( send( fd, buf, size, flag) < 0) 
	{
		perror("send fail\r\n");
		return -1;
	}

	return ret;
}

static int close_dev(int fd)
{
	int ret = 0;

	printf("close_Dev\r\n");

	
	close(fd);

	return ret;
}

int devices_init(dev_node_t *dev,char * nodename)
{
	int fd = 0;

	printf("init_Dev\r\n");
	
	if ( dev == NULL )
	{
		printf("dev struct is NULL!\r\n");
		return -1;
	}
	if ( dev->opts == NULL )
	{
		printf("opt is NULL!\r\n");
		return -1;
	}
	
	if ((fd = socket( AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		perror("socket setup fail\r\n");
		return -1;
	}

	dev->name = nodename;
	dev->opts->open = open_dev;
	dev->opts->close = close_dev;
	dev->opts->read = read_dev;
	dev->opts->write = write_dev;

	return fd;
	
}

int devices_deinit(dev_node_t *dev,int fd)
{
	int ret = 0;

	printf("deinit_Dev\r\n");

	dev->name = NULL;
	dev->opts->open = NULL;
	dev->opts->close = NULL;
	dev->opts->read = NULL;
	dev->opts->write = NULL;
	close(fd);

}

