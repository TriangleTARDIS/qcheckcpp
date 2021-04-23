/*
	MYAUX.CPP -- Misc. auxillary routines.  Some OS dependent.
*/

#include "myaux.h"


//Begin UNIX routines
#ifdef unix
int kbhit(void) // thanks to the top source for a clue on this one...
{
   fd_set read_fds;
   struct timeval timeout;
	int rc;

	FD_ZERO(&read_fds);
	FD_SET(1,&read_fds);	
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	rc = select(32, &read_fds, NULL, NULL, &timeout); 	
   return rc;  
}

int getch(void)  //an annoyance to say the least
{
   char c = 0;
    
   read(0,&c,1);	
	return c;
}
#endif
//End UNIX routines



//Begin WIN32 routines
#ifdef _WIN32
void cls(HANDLE hConsole) //do the Win32 console subsystem calls for a clear screen
{
   COORD coordScreen = { 0, 0 }; 
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi; 
   DWORD dwConSize; 

   GetConsoleScreenBufferInfo(hConsole, &csbi);
   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;  
   FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',dwConSize, coordScreen, &cCharsWritten);
   GetConsoleScreenBufferInfo(hConsole, &csbi);  
   FillConsoleOutputAttribute(hConsole, csbi.wAttributes,dwConSize, coordScreen, &cCharsWritten);  
   SetConsoleCursorPosition(hConsole, coordScreen);  
   return;
}

void clrscr(void)
{
   HANDLE hStdOut;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	cls(hStdOut);
}
//End WIN32 routines

#else
void clrscr(void) //hella kludge
{
  char i;
  
  for (i = 0;i < 25;i++)
     printf("\n");
}
#endif // _WIN32




//OS Independent routines
int connection_refused(void) //test for refused connection, OS dependent
{   
   #ifdef unix
   return errno == ECONNREFUSED;
   #endif

   #ifdef _WIN32
   //    printf( "err = %d\n", WSAGetLastError());  //DEBUG
   return WSAGetLastError() == WSAECONNABORTED;
   #endif
}

void pause_for_key(void) //show a prompt and wait for keypress
{
   printf("Press any key to continue.");
   #ifdef _WIN32
   while (!_kbhit());
   #else
   while (!kbhit())
   { 
     0; //i dunno if this is needed...
   };
   #endif
   getch();  
   printf("\n");
}

char my_delay(int s) //delay s seconds, if keypressed break out
{
   char  d;

   #ifdef _WIN32   
   for (d = 0;d < s*5;d++)
   {        
      if (_kbhit()) return 1;
	      _sleep(200); //get better resolution than with polling every second
   }
   #else
   for (d = 0;d < s;d++)
   {
	  if (kbhit()) return 1;
	     sleep(1);  //have to poll by the second
   }
   #endif
   
   return 0;
}

unsigned long endian_swap_long(unsigned long src) //swap a long from big/little endian to opposite
// Thanks to Dr.J@BayMOO
// Intel : Little-Endian ??
{
   return  (
           (src >> 24) +
           ((src >> 8)  & 0x0000ff00) +
           ((src << 8)  & 0x00ff0000) +
           (src << 24)
           );
}

unsigned short endian_swap_short(unsigned short src) //swap a long from big/little endian to opposite
{
   return  (            
           ((src >> 8)  & 0x00ff) +
           ((src << 8)  & 0xff00)            
           );
}

char * get_last_err(void) //grab the pointer to the last err msg, and clear the gloabl
{
   char *p;

   p = (ERRMSG == NULL) ? NOERR : ERRMSG;
   ERRMSG = NOERR;

   return p;
}


char * english_time(unsigned long t) //convert seconds to h, m, s, etc.
{
	unsigned long tt;
	unsigned int d,h,m,s;
	char *ts,b[80];	


	ts = (char *)malloc(80);
	check_memory(ts);

	tt = t;
	d = tt/86400;    
	tt = tt % 86400;
	h = tt/3600;	  	
	tt = tt % 3600;
	m = tt/60;	  	
	s = tt % 60;

	if (t >= 86400) 
	{
	   sprintf(b,"%ud %02u:%02u:%02u",d,h,m,s);
	}
	else if (t >= 3600)
	{	   
	   sprintf(b,"%02u:%02u:%02u",h,m,s);
	}
	else if (t >= 60)
	{	   
	   sprintf(b,"%02u:%02u",m,s);
	}
	else
	{
	   sprintf(b,"0:00");   	   
	}

	memcpy(ts,&b,80);
	return ts;
}

char *strherror(int h_err) //turn a error into a human readable string
{
   static char msg[100];
   switch (h_err)  
   {
      case HOST_NOT_FOUND:	return "host not found";
      case TRY_AGAIN:		return "try again";
      case NO_RECOVERY:	   return "no recovery";
      case NO_ADDRESS:		return "no address";	   
      default:	sprintf(msg, "%d", h_err); return msg;
   }
}

char check_for_inp(void) //if keypressed return it
{   
   char  k;
   
   #ifdef _WIN32
   if (_kbhit()) 
      k = _getch();
   #else
   if (kbhit())
      k = getch();
   #endif
   else
      k = 0;
   return k;   
}

void do_help(void)
{
   clrscr();
   printf("QCheck Commands :\n\n");
   printf("h or ? : this screen\n");
   printf("d      : change delay between updates\n");
   printf("r      : get a rules listing from the current server\n");
   printf("q      : exit program\n\n\n");
   pause_for_key();
}

void usage(void)
{
	printf("Usage : qcheck host[:port] [-r retries] [-i interval] [-d delay] [-q]\n");
	printf("\t[-qf file] [-p numpings]\n\n");
	printf("-r\tnumber of retries, default is %d\n", DEFAULT_RETRIES);
   printf("-i\tinterval between retries, default is %.2lf seconds\n",DEFAULT_RETRY_INTERVAL / 1000.0);	
	printf("-d\tamount of delay between updates, default is %d seconds\n", DEFAULT_DELAY);
	printf("-q\tdo one quick update, ignore input, and exit\n");
	printf("-qf\tdo one quick update, ignore input, dump to file and exit\n");
	printf("\t(for www pages, etc.)\n");
	printf("-p\tnumber of pings, default is %d, max is %d\n", DEFAULT_PINGS,MAX_PINGS);
	printf("\t(use 0 to disable pinging)\n");
	exit(0);
}


void check_memory(void *p) //if the ptr is null, exit (out of memory)
{
    if (p == NULL) 
    {
       printf("Out of memory.");
       exit(1);  
    }
}


void myfree(void *p) //free a pointer, but not if it's a reusable global string
{
    if (p != NULL && p != NULLSTR)     
       free(p);
}
//OS Independent routines


//Misc display routines
void do_pings(SPTR server,char  n) //ping the quake server, and display the results
{
   char i,rv,nsp = 0;
	time_t tot = 0,temp;
	QRPTR qrp;
	#ifdef _WIN32
	struct _timeb t0,tf;
	#else
	struct timeb t0,tf;	  
	#endif


	if (n == 0) 
	  return;
		
	for (i=0;i<n;i++) 
	{
      qrp = new QRULE;
		#ifdef _WIN32	        		
		_ftime(&t0);		  	
		#else
		ftime(&t0);	  
		#endif

		if (!(rv = qrp->fetch(server,NULLSTR)))
		{
		   #ifdef _WIN32
		   _ftime(&tf);
			#else
			ftime(&tf);
			#endif

			if (strcmp((qrp->get_name()),NULLSTR) != 0)
			{	
			   temp = (time_t)difftime(tf.time,t0.time);
			   tot = tot + ((time_t)temp*1000 + tf.millitm - t0.millitm);			   
			   nsp++;  	   
		   }    
		}  
		delete qrp;
	} //end for		   

	if (nsp) //if false all pings failed
	   printf("Ping avg   : %5.2fms    Dropped : %u of %u\n",((float)tot / (float)nsp),(n-nsp),n);
	else
	   printf("Ping avg   : failed\n");	       
}


void do_rules(SPTR server) //list all the server's rules
{
   char d=0,*rn,*estr="fraglimit"; //fraglimit is the last rule, used as an end marker
	QRPTR qrp;
		
	clrscr();
	printf("Rules for %s\n\n",server->get_host_name());
		  
	rn = strdup(NULLSTR); //tell server we want the list from the start
		
	while (!d)
	{
	   qrp = new QRULE;
		check_memory(qrp);

		if (qrp->fetch(server,rn) == 0)
		{			
		   rn = strdup(qrp->get_name());					
         check_memory(rn);

			if (!(rn == NULLSTR))
			   printf("%-11s : %4s\n",rn,qrp->get_value());		   
		   
			d = ((rn == NULLSTR) || (!strcmp(rn,estr)));
			//printf("RN : %s\n",rn); //DEBUG
			printf("---\n");
		}
		else
		{
		    printf("\nRules list retrieve failed : %s\n",get_last_err());
			 d = 1;
		}	

		delete qrp;
		//check for failure, or end of list to stop loop
	}		   

	printf("\n\n");
	pause_for_key();
	clrscr();
}


void do_players(SPTR server,QSPTR qserver) //list all players on the server
{
   QPPTR player;  
   unsigned char pn;

   printf("\nPID PLAYERNAME      FRAGS SHIRT    PANTS            TIME               ADDRESS\n\n");
   	
   for (pn = 0; pn < qserver->get_q_current_players(); pn++)
   {
      player = new QPLAYER(pn);

		if (player->fetch(server) != 0)
		{
		   printf("#%2u %-15s\n",pn,get_last_err());  //retrieve failed
    	   delete player;	      
	      continue;
	   }
    
      printf("#%2u %-15s %5d",pn,player->get_name(),player->get_frags()); //print out the info, formatted
	   printf(" %-8s",player->color_name(0));  
      printf(" %-8s",player->color_name(0));
	   printf(" %12s",english_time(player->get_connect_time()));
	   printf("  %20s\n",player->get_client_address());
	   	   
      delete player;         
	} //endfor
}

void display_server(SPTR server, QSPTR qserver) //show SERVER and QSERVER info header
{	
    if (!(server == NULL || qserver == NULL))
    {
	  printf("Server     : %s at %s (%s)\n",qserver->get_q_host_name(),qserver->get_q_server_address(),server->get_host_name());
	  printf("Level Name : %s\n",qserver->get_q_level_name());
	  printf("Players    : %u of %u\n",qserver->get_q_current_players(),qserver->get_q_max_players());
	  printf("Proto. Ver : %s\n",qserver->net_protocol_version_name()); 

	}
	else
	  printf("Not a valid server.\n");
}
//Misc display routines