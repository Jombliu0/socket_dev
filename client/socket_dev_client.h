#ifndef DEV_OPTS_H
#define DEV_OPTS_H
#include <stdio.h>

#define DEV_DEINIT 0
#define DEV_INIT 1
#define DEV_OPEN  2
#define DEV_CLOSE 3

typedef struct
{   
  	int (*read)(int fd, char *buf,int size, int flag);
  	int (*write)(int fd, char *buf, int size, int flag);
  	int (*open)(int fd,char *ip, int port);
 	int (*close)(int fd);
} dev_opts_t;

typedef struct 
{
	char *name;
	int flag;
	dev_opts_t *opts;
}dev_node_t;


int devices_deinit(dev_node_t *dev,int fd);
int devices_init(dev_node_t *dev,char * nodename);












#endif

