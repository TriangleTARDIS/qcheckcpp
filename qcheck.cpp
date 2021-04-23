/*
 * qstat 1.4 beta
 * by Steve Jankowski
 * steve@activesw.com
 * http://www.activesw.com/people/steve/qstat.html
 *
 * Inspired by QuakePing by Len Norton
 *
 * Copyright 1996 by Steve Jankowski
 *
 * Permission granted to use for any purpose you desire as long as
 * you maintain this file prolog in the source code and you
 * derive no monetary benefit from use of this source or resulting
 * program.
 */

/*
  QCheck v2.7 alpha 
  by Michael Thompson (aka Shadow_RAM)
  shadowr@ipass.net
  http://www.geocities.com/SiliconValley/5789/
  http://www.warzone.com/qmud/
  
  Inspired by Qstat by Steve Jankowski, and by Top originally by 
  Roger Binns.
  
  Copyright (C) 1996-1997 by Michael Thompson.
  Portions of this code are copyright (C) 1994 Microsoft Corporation,
  and Copyright (C) 1996 by Steve Jankowski
 
  See above disclaimer.
*/ 

#include "qcheck.h"


int main(int argc, char *argv[])
{		
	char *servername,done=0,qu=0,*quf,npings=DEFAULT_PINGS;
	int ds=DEFAULT_DELAY,arg;
   SPTR server;			
	FILE *ofile;  //use this to dump to a file if needed
	
   #ifdef _WIN32
   WORD version= MAKEWORD(1,1);
   WSADATA wsa_data;    
		
   if ( WSAStartup(version,&wsa_data) != 0)  
	{
	   printf("Could not open winsock\n");
	   return 1;
   }
   #endif 

  
	if (!(argc > 1)) 	
		usage();	    
  
	
	servername = strdup(argv[1]);
	check_memory(servername);

	//Begin parsing arguments
   for (arg = 2;arg < argc;arg++)  
	{
	   if (strcmp(argv[arg], "-r") == 0)  
	   {
     	   arg++;

	      if ( arg >= argc)  
	      {
	   	   printf("Missing argument for -r\n\n");
		      usage();				
	      }
	        
	      n_retries = atoi(argv[arg]);
	      if ( n_retries <= 0)  
	      {
		      printf("retries must be greater than 0\n");
		      exit(1);
			}
		}
	   else if ( strcmp( argv[arg], "-i") == 0)  
		{
	      double value = 0.0;

	      arg++;

	      if ( arg >= argc)  
			{
		      printf("Missing argument for -i\n\n");
		      usage();
			}

	      sscanf( argv[arg], "%lf", &value);			

	      if ( value < 0.1)  
			{
		      printf("Retry interval must be greater than 0.1\n");
		      exit(1);
			}

         retry_interval= (int)(value * 1000);
		}
		else if ( strcmp( argv[arg], "-d") == 0)  
		{ 
	      arg++;

			if ( arg >= argc)  
			{
		      printf("Missing argument for -d\n\n");
		      usage();
			}
	        
	      ds = atoi(argv[arg]);

	      if ( ds < 1)  
			{
		      printf("delay must be greater than 0\n");
		      exit(1);
			}	        
		}
		else if ( strcmp( argv[arg], "-q") == 0)  
		{ 
	      qu = 1; // quick update
		}
		else if ( strcmp( argv[arg], "-qf") == 0)  
		{  
	      qu = 1; // quick update to file
	      arg++;

	      if ( arg >= argc)  
			{
		      printf("Missing argument for -qf\n\n");
		      usage();
			}
	        
         quf = strdup(argv[arg]); // which file			
   	   check_memory(quf);			

	   	//reopen stdout to a file, OS dependant
         #ifdef _WIN32
		   ofile = freopen(quf,"w",_fdopen(1,"w"));
         #else         
		   ofile = freopen(quf,"w",stdout);
         #endif

		   if (ofile == NULL)
			{
			   printf("Couldn't open %s for output.\n",quf);
			   exit(1);
			}
		}    
		else if ( strcmp( argv[arg], "-p") == 0)  
		{ 
	      arg++;

	      if (arg >= argc)  
			{
		      printf("Missing argument for -p\n\n");
		      usage();
			}
			
		   npings = atoi(argv[arg]);


		   if ((npings < 0) || (npings > MAX_PINGS))
			{
		      printf("Number of pings must be at least 0 and less than %d .\n",MAX_PINGS);
			}     	        					
		} //end main if    
   } //end main for
   //End parsing arguments

	
	server = new SERVER(servername);
   check_memory(server);

   if (server->bind_socket() != 0) 
	{
	   printf("Unabled to bind/connect socket.\n");
	   return 1;
	}  
    
   if (!qu)
      clrscr();
			  
	while (!done) //main loop
	{	
      char cp=1,sab=0; //cp gets set to false if there is a problem in this iteration, means to jump to next
		                 //sab gets set to true if a command is issued, immediately handle, then jump to next
	   QSPTR qserver;
	   
	   
	   if (!qu)
	   {
	     clrscr();
	     printf("Flushing buffers...");
	   }
	      
	   
	   server->flush_server();

	   qserver = new QSERVER;       
	   
	   if (!qu)
	     clrscr();

		if (qserver->fetch(server) != 0)
	   {
	      printf("Failed on initial server info recv : %s\n",get_last_err());	   
		   cp = 0;
	   }

	   
	   if (cp)
	   {					
		  display_server(server,qserver);  //display SERVER and QSERVER info header
		  do_pings(server,npings);         //ping the server, and display results
	   }

	   if (!qu)
	   {
         switch (toupper(check_for_inp()))
	      {
   	      case 'Q' : done = 1;
		                 sab = 1;
		                 break;
		      case 'H' : do_help();
		                 sab = 1;
		                 break;
		      case '?' : do_help();
		                 sab = 1;
		                 break;
		      case 'D' : clrscr();
		                 printf("Old Delay : %u\n\nNew Delay? ",ds);		             
		                 getch();					 
		                 scanf("%d",&ds);
				           ds = (ds < 1) ? ds = 1 : ds;		             
					        sab = 1;
				           break;
		      case 'R' : do_rules(server);
				      	  clrscr();					 
					        sab = 1; 					 
					        break;
	      }
	   }

	   if (cp && !sab)
		  do_players(server,qserver); //display current player list
	   
	   delete qserver; //should be done with the qserver by now	   

	   if (!qu && !sab)
	   { 
	      if (my_delay(ds))
			{
		      switch (toupper(check_for_inp()))
	         { 
   	         case 'Q' : done = 1;		             
		  			           sab = 1;
		                    break;
		         case 'H' : do_help();		             
		                    sab = 1;
		                    break;
		         case '?' : do_help();		             
		                    sab = 1;
		                    break;
		         case 'D' : clrscr();
		                    printf("Old Delay : %u\n\nNew Delay? ",ds);
		                    getch();					 
		                    scanf("%d",&ds);
		    		           ds = (ds < 1) ? ds = 1 : ds;		             
					           sab = 1;
				              break;
	            case 'R' : do_rules(server);
					           clrscr();
					           sab = 1;
					           break;
				}		   
			}
		}
	   
	   if (qu) 
	      done = 1;

	} //end main loop
   
	
	delete server;
   #ifdef _WIN32
	WSACleanup(); //Winsock cleanup
   #endif

	 
   return 0;
}


