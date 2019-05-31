#ifndef _GCODE_H_
#define _GCODE_H_
#include "stdbool.h"
#include "interfaceCmd.h"

#define CMD_MAX_SIZE = ACK_MAX_SIZE * 10;

typedef struct {
    char command[CMD_MAX_CHAR];     // The command sent to printer
    char startMagic[CMD_MAX_CHAR];  // The magic to identify the start
    char stopMagic[CMD_MAX_CHAR];   // The magic to identify the stop
    bool inResponse;                // true if between start and stop magic
    bool inWaitResponse;            // true if waiting for start magic
    bool done;                      // true if command is executed and response is received
    char *cmd_rev_buf;              // buffer where store the command response
    uint8_t cmd_rev_buf_len;        // actual size of the cmd_rev_buf ( allocated memory not string length)
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

bool request_M21(void);
char * request_M20(void);

#endif