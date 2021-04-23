/*
	QPlayer -- Quake player record

   Holds, name, frags, etc.
*/ 


#include "qplayer.h"

//Quake Player Info Network Packet
unsigned char  player_info_request[] = { 0x80,0x00,0x00,0x06,0x03,0x00 };
					// 0x80 0x00 control msg
					// 0x00 0x06 6 byte msg length
					// 0x03      msg code
					// 0x00      player number 0-15


QPLAYER::qplayer(char pn) //constructor, pn is the # of the player on the server
{
   player_number = pn;
	name = (unsigned char *)NULLSTR;
	colors = unused1 = unused2 = unused3 = 0;
	frags = connect_time = 0;
	client_address = NULLSTR;
}


int QPLAYER::fetch(SPTR server) //get info from server, and parse into class, returns 0 if success
{   
   unsigned char *pp; //temp for the player packet

	pp = (unsigned char *)malloc(sizeof(player_info_request));
   check_memory(pp);
   memcpy(pp,&player_info_request,sizeof(player_info_request)); 

   *(pp+5) = player_number;   //set which player   
	  
   if (server->send_packet(pp,sizeof(player_info_request)) != 0)
      return 1;
	
	return recv_player_info_packet(server);
}


int QPLAYER::recv_player_info_packet(SPTR server) // returns 0 if successfull
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
	   return 1; 
	}

	if (server->get_fd() == -1 || (!FD_ISSET(server->get_fd(), &read_fds)))
	{
	   ERRMSG = SYSERROR; //or timeout????
	   return 1;
	}
	
	p = pkt;
	if ((pktlen = recv(server->get_fd(), (char *)p, sizeof(pkt), 0)) == SOCKET_ERROR)  
	{		
	   ERRMSG = DOWN;
	   return 1;
	}
	
	if (pkt[4] == 0x84 && pkt[0] == 0x80 && pkt[1] == 0x00 && (pktlen == MAKEWORD(pkt[3],pkt[2])))
	{
	   player_number = pkt[5];
	   p = &pkt[6];
	   name = set_pname(p);	   
	   p += strlen((char *)p) + 1;
	   colors = *p;
	   p++;
	   unused1 = *p;
	   p++;
	   unused2 = *p;
	   p++;
	   unused3 = *p;
	   p++;	
	   frags = MAKELONG(MAKEWORD(*p,*(p+1)),MAKEWORD(*(p+2),*(p+3)) );	
	   p += 4;	
	   connect_time = MAKELONG(MAKEWORD(*p,*(p+1)),MAKEWORD(*(p+2),*(p+3)) );	
	   p += 4;
	   client_address = strdup((char *)p);	  
	   check_memory(client_address);
	}



   if (!strcmp((char *)name,NULLSTR))
   {     
      ERRMSG=SYSERROR;
	   return 1;
   }

   return 0;
}


unsigned char *QPLAYER::set_pname(unsigned char *name) //'cool' names back to normal letters
{
   unsigned char *tn,*n2;
   unsigned char  c,c2,tc;

   tn = (unsigned char *)strdup((char *)name); //dupe of original
   check_memory(tn);
   n2 = (unsigned char *)strdup((char *)name); //store converted string here
   check_memory(n2);

	//values above 128 show 'cool' chars in Quake, convert them back to below 127
	//so the names are readable
   for (c = 0, c2 = 0; c <= strlen((char *)tn)-1; c++, c2++)
   {
      tc = *(tn+c);
	  if (tc >= 32 && tc <= 126) 
	    *(n2+c2) = tc;
	  else if (tc >= 160 && tc <= 254)
	    *(n2+c2) = tc - 128;
	  else if (tc >= 146 && tc <= 155)
	    *(n2+c2) = tc - 98;
	  else if (tc >= 18 && tc <= 27)
	    *(n2+c2) = tc + 30;	  
	  else if ((tc >= 6 && tc <= 9) || (tc >= 1 && tc <= 4) || (tc >= 134 && tc <= 137))
	    *(n2+c2) = 35;	  
	  else if (tc == 5 || tc == 14 || tc == 15 || tc == 28 || tc == 133 || tc == 142 || tc == 143)
	  	*(n2+c2) = '.';	    
	  else if (tc == 128 || tc == 144 || tc == 157)
		*(n2+c2) = '[';	    
	  else if (tc == 129 || tc == 158)
		*(n2+c2) = '=';	    
	  else if (tc == 130 || tc == 145 || tc == 159)
		*(n2+c2) = ']';	    
	  else        
		c2--;
   }
   *(n2+c2) = 0;
   return n2;
}

char *QPLAYER::color_name(char c) //c = false for shirt, true for pants color
{
	char n;

	if (!c)
	  n = (colors >> 4) & 0xF;		//4bits for shirt color
   else
     n = colors & 0xF;           //4bits for pants color

	switch (n)
	{
	   case 0  : return "White";
	   case 1  : return "Olive";
	   case 2  : return "Lt. Blue";
	   case 3  : return "Green";
	   case 4  : return "Red";
	   case 5  : return "Mustard";
	   case 6  : return "Pink";
	   case 7  : return "Flesh";
	   case 8  : return "Purple";
	   case 9  : return "Purple";
	   case 10 : return "Gray";
	   case 11 : return "Green";
	   case 12 : return "Yellow";
	   case 13 : return "Blue";
	}
	return "Bad Color";
}
