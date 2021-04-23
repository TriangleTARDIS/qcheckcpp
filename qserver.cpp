#include "qserver.h"

unsigned char  server_info_request[] = { 0x80, 0x00, 0x00, 0x0C, 0x02, 0x51,
                                 0x55, 0x41, 0x4B, 0x45, 0x00, 0x03 };
					// 0x80 0x00 control msg
					// 0x00 0x0C 12 byte msg length
					// 0x02      msg code
					// 0x51 0x55 0x41 0x4B 0x45 0x00  "QUAKE"
					// 0x01 network version (0x01 = qtest1, 0x02 = ??, 0x03 = Quake 0.91 to 1.09)


QSERVER::qserver(void)
{    
   q_server_address = NULLSTR;
	q_host_name = NULLSTR;
   q_level_name = NULLSTR;
	q_current_players = 0;
	q_net_protocol_version = 0;
}

QSERVER::~qserver(void)
{    
   myfree(q_server_address);
	myfree(q_host_name);
   myfree(q_level_name);
}


int QSERVER::fetch(SPTR server)
{  
   if (server->send_packet(server_info_request,sizeof(server_info_request)) != 0)
		return 1;		     
	
   return recv_qserver_info_packet(server);
}



int QSERVER::recv_qserver_info_packet(SPTR server) // returns 0 if successfull
{
   short int pktlen, rc;
   unsigned char  pkt[PACKET_LEN], *p;
	fd_set read_fds;
   fd_set error_fds;
   struct timeval timeout;
	

	FD_ZERO(&read_fds);
	FD_ZERO(&error_fds);
	server->set_fds(&read_fds);
	server->set_fds(&error_fds);

	timeout.tv_sec= retry_interval / 1000;
	timeout.tv_usec= (retry_interval % 1000) * 1000;

	rc = select(64, &read_fds, NULL, &error_fds, &timeout); // first arg (64) is ignored

	if (rc == SOCKET_ERROR)
	{
	    ERRMSG = SYSERROR;
	    perror("select");
	    return 1;
	}
	else if (rc == 0)
	{	
	   ERRMSG = TIMEOUT;
	   return 1; 
	}


	if (server->get_fd() == -1 || (!FD_ISSET(server->get_fd(), &read_fds)))
	{
	   ERRMSG = SYSERROR;
	   return 1;
	}
	
   p = pkt;	
	if ((pktlen = recv(server->get_fd(), (char *)pkt, sizeof(pkt), 0)) == SOCKET_ERROR)  
	{			   
	   ERRMSG = DOWN;
	   return 1;
	}
	
	
   if (pkt[4] == 0x83 && pkt[0] == 0x80 && pkt[1] == 0x00 && (pktlen == MAKEWORD(pkt[3],pkt[2])))
	{
	   p = &pkt[5];
      q_server_address = strdup((char *)p);
	   check_memory(q_server_address);
	   p += strlen((char *)p) + 1;
	   q_host_name = strdup((char *)p);
	   check_memory(q_host_name);
	   p += strlen((char *)p) + 1;
	   q_level_name = strdup((char *)p);
	   check_memory(q_level_name);
	   q_current_players = pkt[pktlen-3];
	   q_max_players = pkt[pktlen-2];	
	   q_net_protocol_version = pkt[pktlen-1];
	}
	else
	{ 
	   ERRMSG=SYSERROR;
	   return 1;
	}
	

	return 0;
}


char *QSERVER::net_protocol_version_name(void)
{
    switch (q_net_protocol_version)  
    {
       case 1  :	return "Qtest 1";
       case 2  :	return "Unknown (Ver 2)";
       case 3  :	return "Quake v0.91 to 1.06";
       default :	return "Unknown ???";
    }   
}


