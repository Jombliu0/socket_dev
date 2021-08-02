#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socket_dev_client.h"

#define FILE_NAME_MAX_LEN 64
#define DEFAULT_SVR_PORT 2828


int main( int argc, char * argv[] )
{
	int sockfd, numbytes;
	char buf[1024], filename[FILE_NAME_MAX_LEN + 1];
	char ip_addr[64];
	int i=0, len, total;
	unsigned short port;
	dev_node_t *socket_dev = malloc(sizeof(dev_node_t));
	socket_dev->opts = malloc(sizeof(dev_opts_t));
	char * dev_name = "client";

	FILE * file = NULL;

	if ( argc < 2 )
	{
		printf( "Socket_file_client [server_ip] [port]\r\n" );
		printf( "server_ip: exam 192.168.1.224\r\n" );
		printf( "port: option,default:2828\r\n" );
		exit( -1 );
	}
	
	strncpy( ip_addr, argv[1], sizeof(ip_addr) );
	port = DEFAULT_SVR_PORT;
	if ( argc >= 3 )
	{
		port = (unsigned short)atoi(argv[2]);
	}

	if((sockfd = devices_init(socket_dev,"123")) == -1)
	{

		perror( "socket client init fail! \r\n" );
		exit(-1);
	}

	if(socket_dev->opts->open(sockfd,ip_addr,port) == -1 )
	{

		perror( "socket client dev open fail! \r\n" );
		exit(-1);
	}


	if (socket_dev->opts->write( sockfd, "hello", 6, 0 ) < 0 )
	{
		perror( "send fail \r\n");
		exit(-1);
	}	

	/* 接收文件名 */
	total = 0;

	while( total < FILE_NAME_MAX_LEN )
	{
		
		len = socket_dev->opts->read( sockfd, filename + total, (FILE_NAME_MAX_LEN - total), 0 );
		if ( len <= 0 )
		{
			break;
		}
		total += len;
	}

	// 接收的文件名称大小不对应
	if ( total != FILE_NAME_MAX_LEN )
	{
		perror( "failure file name\r\n" );
		exit(-3);
	}	
	printf( "recv file %s ....\r\n",filename );

	file = fopen( filename, "wb" );
	if ( file == NULL )
	{
		printf( " create file %s failure\r\n", filename );
		exit(-3);
	}

	// 接收文件数据
	printf( "recv begin\n " );
	total = 0;

	while(1)
	{
		len = socket_dev->opts->read( sockfd, buf, sizeof(buf), 0 );
		if ( len == -1 )
		{
			break;
		}
		total += len;
		// 写入本地文件
		fwrite( buf, 1, len, file );
	}

	fclose(file);
	printf("recv file %s success total lenght %d\r\n",filename,total);
	socket_dev->opts->close(sockfd);
		
	return 0;
}

