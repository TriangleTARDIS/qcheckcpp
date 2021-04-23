/*
   QRULE.H -- Define QRULE class.  Represents a single rule on the server.
*/

#include "mydefs.h"
#include "mydefs2.h"
#include "myaux.h"
#include "server.h"


#ifndef _QRULE_
#define _QRULE_


class qrule
{
	char *name;	 //rule name : sv_maxspeed, sv_friction, sv_gravity, noexit, 
	      	    //            teamplay, timelimit and fraglimit
	char *value; // floating point or a string, in a string

   public:
	qrule(void) {name = NULLSTR; value = NULLSTR;};   //constructor
	~qrule(void) {myfree(name); myfree(value); };     //destructor
	int fetch(SPTR server, char *rn); //set name, then query the server for the value, return 0 if success
	char *get_name(void) {return name;};
	char *get_value(void) {return value;};

	private:	
   unsigned char *make_rule_packet(void); //create a packet for fetch	
	int recv_rule_info_packet(SPTR server); // parse server's rule request reply, returns 0 if successfull
};

typedef class qrule QRULE;
typedef QRULE *QRPTR;


#endif // _QRULE_


