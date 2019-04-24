#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "includes.h"

#define ACK_MAX_SIZE 300
extern char ack_rev_buf[ACK_MAX_SIZE];

void parseACK(void);

#endif

