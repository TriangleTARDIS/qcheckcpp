/*
   SERVER.H -- Define SERVER class.  Represents a single internet server.
   This is not necessarilly a Quake server.  Uses UDP to communicate with
	the server.
	
*/

#include "mydefs.h"
#include "mydefs2.h"
#include "myaux.h"


#ifndef _SERVER_
#define _SERVER_


class server
{
   char *arg;	  //obsolete field...
   char *host_name;
   struct in_addr ip[MAXIP];
	int port;
	int fd;	    
   int retry;

	public:
	server(char *arg); //constructor, arg is what the user called the server, be it IP or name
	~server(void);     //desctructor
   char *get_host_name(void) {return host_name;};   
	int get_port(void) {return port;};
	int get_fd(void) {return fd;};
	int get_retry(void) {return retry;};  
   int flush_server(void); //dump any pending input
   int bind_server(void);  //create the socket, then bind input/output IP's ports to it, called by bind_socket
   int bind_socket(void);  //sort of redundant wrapper for bind_server
   void set_fds(fd_set *fds);   //set a file descriptor set to include just the server's fd
   int send_packet(const unsigned char  *pkt,int pktlen); //send a UDP packet of any length
};

typedef class server SERVER;
typedef SERVER *SPTR;


#endif // _SERVER_



