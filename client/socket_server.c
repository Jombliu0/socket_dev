#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>


#define DEFAULT_SVR_PORT 2828 // default port :2828
#define FILE_NAME_MAX_LEN 64 // max file name len:64

char filename[FILE_NAME_MAX_LEN + 1];

static void * handle_client( void* arg )
{
	int sock = (int)arg;
	char buff[1024];
	int len;
	printf( "begin send file name \r\n");

	FILE *file = fopen( filename, "r" );

	if ( file == NULL )
	{
		close( sock );
		exit;
	}

	//send file name 
	if ( send( sock, filename, FILE_NAME_MAX_LEN, 0 ) == -1 )
	{
		perror( "Send file name faile\r\n " );
		fclose( file );
		close( sock );
	}

	printf( "begin send file : %s,....\r\n", filename );
	
	//send file 
	while( !feof( file ) )
	{
		len = fread( buff, 1, sizeof( buff ), file );
		
		printf( "server read %s,len %d\r\n ", filename,len );

		if ( send(sock, buff, len, 0) < 0 )
		{
			printf( "send file fail\r\n  " );
			break;
		}
	}

	if ( file )
	{
		fclose( file );
	}
	close( sock );


}


int main( int argc, char * argv[] )
{
	int sockfd,new_fd;

	//Define Ipv4 of source and client
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size,numbytes;
	pthread_t cli_thread;
	unsigned short port;

	//input parameter
	if ( argc < 2 )
	{
		printf( "socket_file_trans [filename]  [port] \r\n" );
		printf( "filename: filename without path\r\n" );
		printf( "port: option,default:2828\r\n" );
		exit( -1 );
	}
	strncpy( filename, argv[1], FILE_NAME_MAX_LEN);
	port = DEFAULT_SVR_PORT;

	if ( argc >= 3 )
	{
		port = (unsigned short)atoi( argv[2] );
	}


	//step1:setup tcp socket
	//AF_INET ---> IP communation
	//SOCK_STREAM ---> TCP
	if ( (sockfd = socket( AF_INET, SOCK_STREAM, 0 ))  == -1 )
	{
		perror( "socket setup fail\r\n" );
		exit( -1 );
	}	

	// step2: set listening port
	memset( &my_addr, 0, sizeof( struct sockaddr ) );

	my_addr.sin_family = AF_INET; /* IPv4 */
	my_addr.sin_port = htons( port );
	my_addr.sin_addr.s_addr = INADDR_ANY;

	//stp3: Binding socket
	
	if ( bind( sockfd, (struct sockaddr*)&my_addr, sizeof( struct sockaddr )) == -1 )
	{
		perror( "bind fail\r\n" );
		close(sockfd);
		exit( -1 );
	}

	// step4 : listening port 
	if ( listen( sockfd, 10 ) == -1 )
	{
		perror( "listening\r\n" );
		close(sockfd);
		exit( -1 );

	}
	printf( " listen port %d\r\n ",port );

	while(1)
	{
		sin_size = sizeof( struct sockaddr_in );
		printf( "server waitting...\r\n" );
	 	if ( (new_fd = accept( sockfd, (struct sockaddr*)&their_addr,&sin_size )) == -1 )
		{
			perror( "accept:\r\n" );
			close( sockfd );
			exit( -1 );
		}		
		printf( "---client (ip = %s:port = %d) request \n",inet_ntoa( their_addr.sin_addr ), ntohs( their_addr.sin_port ) );
		pthread_create( &cli_thread, NULL, handle_client, (void*)new_fd  );

	}

	close( sockfd );
	exit( -1 );

}

