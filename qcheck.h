/*
   QCHECK.H -- Includes, defs for the main routine.
*/

#include "mydefs.h"
#include "myaux.h"
#include "qrule.h"
#include "qplayer.h"
#include "qserver.h"


#ifndef _QCHECK_
#define _QCHECK_


//Begin Global vars
char *NULLSTR = "";  //reusable nullstr

 
//global error message system
char *DOWN = "DOWN";
char *SYSERROR = "ERROR";
char *TIMEOUT = "TIMEOUT";
char *NOERR ="NOERR";
char *BADPKT = "CORRUPT PACKET";
char *ERRMSG = NULL;


//copies for network functions to modify if needed
int retry_interval = DEFAULT_RETRY_INTERVAL;
int n_retries = DEFAULT_RETRIES;
//End Global vars


#endif // _QCHECK_