/*
   MYAUX.H -- Misc. auxillary routines.  Some OS dependent.
*/

#ifndef _MYAUX_
#define _MYAUX_

#include "mydefs.h"
#include "mydefs2.h"
#include "qserver.h"
#include "qplayer.h"
#include "qrule.h"
#include "server.h"

#ifdef unix
//this needs some work, termcap stuff etc
int kbhit(void); 
int getch(void);
#endif


#ifdef _WIN32
void cls(HANDLE hConsole);  //do the Win32 console subsystem calls for a clear screen
#endif


int connection_refused(void); //test for refused connection, OS dependent
void clrscr(void);            //clear the screen
void pause_for_key(void);     //show a prompt and wait for keypress
char my_delay(int s);         //delay for S seconds, break and return 1 if keypressed
unsigned long endian_swap_long(unsigned long src);     //swap a long from big/little endian to opposite
unsigned short endian_swap_short(unsigned short src);  //swap a long from big/little endian to opposite
char * english_time(unsigned long t);  //convert seconds to h, m, s, etc.
char * get_last_err(void);   //grab the pointer to the last err msg, and clear the gloabl
char * strherror(int h_err); //turn a error into a human readable string
char check_for_inp(void);    //if keypressed return it
void do_help(void);          //print help
void usage(void);            //print usage, exit
void check_memory(void *p);  //if the ptr is null, exit (out of memory)



//Misc functions that didn't fit on the classes
void do_pings(SPTR server,char  n); //ping the quake server, and display the results
void do_rules(SPTR server);         //list all the server's rules
void do_players(SPTR server,QSPTR qserver); //list all players on the server
void display_server(SPTR server,QSPTR qserver); //show SERVER and QSERVER info header
#endif // _MYAUX_

void myfree(void *p);        //free a pointer, but not if it's a reusable global string