/*
	QRule -- Quake Game Rule

   Akin to enviroment variables.
*/ 


#include "qrule.h"



//Quake network packet
unsigned char  rule_info_request[] = { 0x80,0x00,0x00,0x06,0x04,0x00 };
					// 0x80 0x00 control msg
					// 0x00 0x06 6 byte msg length (variable)
					// 0x04      msg code
					// 0x00      null str


int QRULE::fetch(SPTR server, char *rn)
{   
	unsigned char *rp; //temp for the rule packet

	name = strdup(rn);
	rp = make_rule_packet();
   if (server->send_packet(rp,*(rp+3)) != 0)
		return 1;
	
	return recv_rule_info_packet(server);
}



unsigned char *QRULE::make_rule_packet(void)
{
   unsigned char  *rp;   

	rp = (unsigned char *)malloc(sizeof(rule_info_request)+strlen(name)); 
   check_memory(rp);

   memcpy(rp,&rule_info_request,sizeof(rule_info_request)-1); //copy in the generic rule packet
   memcpy((rp+5),name,strlen(name)+1);                          //insert the requested rule name
   *(rp+3) = (6 + strlen(name));                               //adjust packet's length field
   
   return (unsigned char *)rp;
}


int QRULE::recv_rule_info_packet(SPTR server) // returns 0 if successfull
{
   int pktlen, rc;
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
	   return 2; 
	}


	if (server->get_fd() == -1 || (!FD_ISSET(server->get_fd(), &read_fds)))
	   return 3;
	
	
	if ((pktlen = recv(server->get_fd(), (char *)pkt, sizeof(pkt), 0)) == SOCKET_ERROR)  
	{			   
	   ERRMSG = DOWN;
	   return 4;
	}
	
	if (pkt[4] == 0x85 && pkt[0] == 0x80 && pkt[1] == 0x00 && (pktlen == MAKEWORD(pkt[3],pkt[2])))
	{  
	   if (pktlen != 5)
	   {
			p = &pkt[5];
			name = strdup((char *)p);   //server holds these as a link list, so this is our rule's real name
			//printf("RRN : %s\n",name); //DEBUG
         check_memory(name);         
			p += strlen((char *)p) + 1;
		   value = strdup((char *)p);  //set the value
		   check_memory(value);			
			return 0;  //success, finish
	   }
   }


	ERRMSG = BADPKT;
	return 5; //if we got here our packet was corrupted
}














