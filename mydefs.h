/*
   MYDEFS.H -- All the global defs. and includes
*/


#ifndef _MYDEFS_
#define _MYDEFS_


/*Begin includes */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/timeb.h>


#ifdef unix
  #include <unistd.h>
  #include <sys/socket.h>
  #include <sys/param.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <arpa/inet.h> 
 //#include <ncurses/curses.h>   Gave up on trying to mess with this
  
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define closesocket(a) close(a)    
  typedef unsigned char  BYTE;
  typedef unsigned short WORD;
  typedef unsigned long  DWORD;  
  #define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
  #define MAKELONG(a, b)      ((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
  
  #ifndef sgi
    #ifndef linux
      #define INADDR_NONE ~0
    #endif
  #endif
#endif


#ifdef _WIN32	
  #include <windows.h>
  #include <winsock.h>
  #include <conio.h>    
  #include <fcntl.h>
  #include <io.h>    
#endif
/*End includes */


#define DEFAULT_PORT		26000             // Quake definition
#define DEFAULT_RETRIES		5              // for all network functions
#define DEFAULT_RETRY_INTERVAL	700		// milli-seconds 
#define DEFAULT_DELAY		5 			      // seconds 
#define DEFAULT_PINGS 5                   
#define MAX_PINGS 50      // if someone uses this many they're crazy...
#define MAXIP 2           // how many IPs should we get from the lookup
#define hostname_lookup 1 // bother to do a reverse lookup?
#define PACKET_LEN 1600   // Quake definition


#endif // _MYDEFS_




