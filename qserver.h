/*
   QSERVER.H -- Define QSERVER class.  Represents a single NetQuake server.
	Not to be confused with a 'generic' inet server.
*/

#include "mydefs.h"
#include "mydefs2.h"
#include "myaux.h"
#include "server.h"


#ifndef _QSERVER_
#define _QSERVER_


class qserver  
{		
   char *q_server_address;
   char *q_host_name;
   char *q_level_name;
   char  q_current_players;
   char  q_max_players; 
   char  q_net_protocol_version;

   public:
	qserver(void);  //constructor
	~qserver(void); //destructor
	int fetch(SPTR server); //fetch info about server, parse into class, return 0 if success
   char *get_q_server_address(void) {return q_server_address;};
   char *get_q_host_name(void) {return q_host_name;};
   char *get_q_level_name(void) {return q_level_name;};
   char  get_q_current_players(void) {return q_current_players;};
   char  get_q_max_players(void) {return q_max_players;}; 
   char  get_q_net_protocol_version(void) {return q_net_protocol_version;};
   char *net_protocol_version_name(void); //return a name for the server's protocol version

   private:
	int recv_qserver_info_packet(SPTR server); // returns 0 if successfull
};


typedef class qserver QSERVER;
typedef QSERVER *QSPTR;


#endif // _QSERVER_



