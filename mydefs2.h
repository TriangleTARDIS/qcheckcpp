/*

   MYDEFS2.H -- Hooks for other source files into the
	global vars since only one can define them.  See
	qcheck.h for actual declartions.

*/

#ifndef _MYDEFS2_
#define _MYDEFS2_

//global err msg system
extern char *DOWN;
extern char *SYSERROR;
extern char *TIMEOUT;
extern char *NOERR;
extern char *NULLSTR;
extern char *BADPKT;
extern char *ERRMSG;


//global copies for network functions to modify if needed
extern int retry_interval ;
extern int n_retries;


#endif // _MYDEFS2_