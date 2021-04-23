#include "server.h"


SERVER::server(char *arg)
{
   struct hostent *ent = NULL;
   struct hostent *name_ent = NULL;
   int i, qport;
   char **a,*name,*port_s;
   unsigned long ipaddr;

 
	name = strdup(arg);
	check_memory(name);

	port_s = strchr(name,':');
	if (port_s != NULL)
	{
	   if (sscanf(port_s+1,"%d",&port) != 1) 
		{
	      qport = DEFAULT_PORT;
	      printf("Bogus port, using default of %d",DEFAULT_PORT);
		   pause_for_key();
		}
	   *port_s = 0; //clip the port off the string by terminating it at the colon
	}
	else
	   qport = DEFAULT_PORT;
	
	
	ipaddr = inet_addr(name);
	
	if (ipaddr == INADDR_NONE)
	   ent = gethostbyname(name);	
	   
	if (ent == NULL && ipaddr != INADDR_NONE) //gethostbyname() didnt parse it right...	  
	   ent = gethostbyaddr((char *)&ipaddr,4,PF_INET); //size has to be 4 for PF_INET addresses

	if (ent == NULL)	
	{
	   printf("Unable to find host : %s\n",name);
      exit(1);
	}

	name_ent = (struct hostent *)((ent != NULL) ? ent : NULL);

	
   arg = strdup(arg);
	check_memory(arg);
   host_name = strdup((name_ent != NULL) ? name_ent->h_name : ent->h_name);
	check_memory(host_name);
   for (a = ent->h_addr_list, i= 0; *a != NULL && i < MAXIP; a++, i++)
   {
	   memcpy( &ip[i].s_addr, *a, sizeof(ip[i].s_addr));    
   }
   port = qport;    
   fd = -1;
   retry = n_retries;   
}

SERVER::~server(void)
{   
   myfree(arg);
	myfree(host_name);
	closesocket(fd);
	fd= -1;
}











int SERVER::bind_server(void)
{
    struct sockaddr_in addr;

    if ((fd = socket(PF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET) 
    {
        perror("socket");			    
		ERRMSG = SYSERROR;
        return 1;
    }

    addr.sin_family = PF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset( &(addr.sin_zero), 0, sizeof(addr.sin_zero) );

    if (bind(fd, (struct sockaddr *)&addr,sizeof(struct sockaddr)) == SOCKET_ERROR) 
    {
        perror("bind");		     	
		ERRMSG = SYSERROR;
        return 1;
    }

    addr.sin_family = PF_INET;
    addr.sin_port = htons((short)port);
    addr.sin_addr = ip[0];
    memset( &(addr.sin_zero), 0, sizeof(addr.sin_zero) );

    if ( connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)  
    {
      perror("connect");	  	  
	  ERRMSG = SYSERROR;
	  return 1;
    }
	
	return 0;
}

int SERVER::bind_socket(void)
{    
	if (fd == -1)  
	{
	    return bind_server();	   
	}	
	else
	   return 1;
}

void SERVER::set_fds(fd_set *fds)
{  
	if (fd != -1)
	    FD_SET(fd, fds);   
}



int SERVER::flush_server(void) // returns 0 if flushed something
{
   int pktlen, rc;
   char pkt[PACKET_LEN];
	fd_set read_fds;
   fd_set error_fds;
   struct timeval timeout;
	int retry_interval= DEFAULT_RETRY_INTERVAL;    


	FD_ZERO(&read_fds);
	FD_ZERO(&error_fds);
	set_fds(&read_fds);
	set_fds(&error_fds);
	
	timeout.tv_sec= retry_interval / 1000;
	timeout.tv_usec= (retry_interval % 1000) * 1000;
	
	rc = select(64, &read_fds, NULL, &error_fds, &timeout); // first arg (64) is ignored

	if (rc == SOCKET_ERROR)
	   return 1;	
	else if (rc == 0)
	   return 1; 	

	if (fd == -1 || (!FD_ISSET(fd, &read_fds)))
	   return 1;
	
	if ((pktlen= recv(fd, pkt, sizeof(pkt), 0)) == SOCKET_ERROR)  
	   return 0;
    
   return 1;
}


int SERVER::send_packet(const unsigned char  *pkt,int pktlen) // returns 0 if success
{

	char sent=0;
	int oretry;
	    
	oretry = retry;
	
	if (fd != -1)  
	{
		while (!sent && (retry >= 0))
		{
		   if (send(fd,(char *)pkt,pktlen,0) == SOCKET_ERROR)
		   {			  
		      perror("send");
		      retry--;
		   }
		   else 
		     sent = 1;   	   
	     }
		
		if (retry <= 0) 
		  ERRMSG = TIMEOUT;		
	}	

	
	//good debug code, dump sent packet
	/*printf("Sent %d bytes.\n",pktlen);
	for (i = 0; i <= pktlen; i++)
	{
	   printf("Sent byte[%d] : %X\n",i,*(pkt+i));
	}
	printf("press a key\n");
	getch();
	*/
	retry = oretry;
	return (!sent);
}










