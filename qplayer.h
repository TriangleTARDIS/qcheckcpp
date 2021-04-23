/*
   QPLAYER.H -- Define QPLAYER class.  Represents a single
	player on the server.
*/

#include "mydefs.h"
#include "mydefs2.h"
#include "myaux.h"
#include "server.h"


#ifndef _QPLAYER_
#define _QPLAYER_


class qplayer
{	      
   char     player_number;           // 0-15
   unsigned char *name;              // name/alias of the player (can contain > 128 values)
   char     colors;                  // (shirt_color << 4) +  pants_color
   char     unused1;                 // ?
   char     unused2;                 // ?
   char     unused3;                 // ?
   long     frags;                   // number of frags (little-endian)
   unsigned long connect_time;       // connection time (secs)(little-endian)
   char     *client_address;         // IP:port (local, IPX#, serial port?)

   public:
	qplayer(char pn); //constructor, pn is the # of the player on the server
	~qplayer(void) {myfree(name); myfree(client_address);}; //destructor
	int fetch(SPTR server);  //get info from server, and parse into class, returns 0 if success
	unsigned char *get_name(void) {return name;};  
	char get_player_number(void) {return player_number;};   
   long get_frags(void) {return frags;};
   unsigned long get_connect_time(void) {return connect_time;};
   char *get_client_address(void) {return client_address;};
   char *color_name(char c); //c = false for shirt, true for pants color

	private:
	int recv_player_info_packet(SPTR server); // parse server's player info request reply, returns 0 if successfull
	unsigned char * set_pname(unsigned char *name); //'cool' names back to normal letters
};

typedef class qplayer QPLAYER;
typedef QPLAYER *QPPTR;


#endif // _QPLAYER_