#include "gcode.h"
#include "includes.h"

REQUEST_COMMAND_INFO requestCommandInfo;


static void resetRequestCommandInfo(void) 
{
    if(requestCommandInfo.cmd_rev_buf_len > 0)
        memset(requestCommandInfo.cmd_rev_buf,0,requestCommandInfo.cmd_rev_buf_len);
    requestCommandInfo.inWaitResponse = true;
    requestCommandInfo.inResponse = false;
    requestCommandInfo.done = false;
}


/*
    Send M21 command and wait for response

    >>> M21
    SENDING:M21
    echo:SD card ok
    echo:SD init fail

*/
bool request_M21(void)
{
//    GUI_DebugMessage(132,100,(u8 *)"COMANDO M21",0);

    strcpy(requestCommandInfo.command,"M21\n");
    strcpy(requestCommandInfo.startMagic,echomagic);
    strcpy(requestCommandInfo.stopMagic,"\n");
    strcpy(requestCommandInfo.errorMagic,"Error");

    resetRequestCommandInfo();
//    GUI_DebugMessage(132,100,(u8 *)"COMANDO INVIO M21",0);
    storeCmd(requestCommandInfo.command);
    // Wait for response
    while (!requestCommandInfo.done)
    {
//        GUI_DebugMessage(132,100,(u8 *)"M21,WAIT DONE...",0);
        loopProcess();
//        GUI_DebugMessage(132,100,(u8 *)"M21,WAIT DONE ...",0);
    }
//    GUI_DebugMessage(132,100,(u8 *)"OK!",0);
    // Check reponse
    if(strstr(requestCommandInfo.cmd_rev_buf,echomagic) != NULL && strstr(requestCommandInfo.cmd_rev_buf,"SD card ok") != NULL){
          return true;
    } else {
          return false;            
    }
}

/*
SENDING:M20
Begin file list
PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~1.GCO 11081207
/YEST~1/TEST2/PI3MK2~3.GCO 11081207
/YEST~1/TEST2/PI3MK2~2.GCO 11081207
/YEST~1/TEST2/PI3MK2~4.GCO 11081207
/YEST~1/TEST2/PI3MK2~5.GCO 11081207
/YEST~1/PI3MK2~1.GCO 11081207
/YEST~1/PI3MK2~3.GCO 11081207
/YEST~1/PI3MK2~2.GCO 11081207
End file list
*/
char *request_M20(void)
{

/*     GUI_Clear(BK_COLOR);
 *///    GUI_DispString(10,10,(u8 *)"COMANDO M20",0);
    strcpy(requestCommandInfo.command,"M20\n");
    strcpy(requestCommandInfo.startMagic,"Begin file list");
    strcpy(requestCommandInfo.stopMagic,"End file list");
    strcpy(requestCommandInfo.errorMagic,"Error");
    resetRequestCommandInfo();
    storeCmd(requestCommandInfo.command);
    // Wait for response
    while (!requestCommandInfo.done)
    {
/*         char *buf=malloc(1000);
        sprintf(buf, "WAIT DONE CMD:%s \n LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rep_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
        GUI_DispString(10,BYTE_HEIGHT*10,(u8 *)buf,0);
 */        loopProcess();
/*         sprintf(buf, "wait DONE CMD:%s \n LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rep_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
        GUI_DispString(10,BYTE_HEIGHT*10,(u8 *)buf,0);
        free(buf);
 */    }
 /*    GUI_Clear(BK_COLOR);
    GUI_DispStringInRect(0,0,LCD_WIDTH,LCD_HEIGHT,(u8 *)requestCommandInfo.cmd_rev_buf,0);
    Delay_ms(5000); */

    return requestCommandInfo.cmd_rev_buf;
}


/**
 * Select the file to print 
 * 
 * >>> m23 YEST~1/TEST2/PI3MK2~5.GCO
 * SENDING:M23 YEST~1/TEST2/PI3MK2~5.GCO
 * echo:Now fresh file: YEST~1/TEST2/PI3MK2~5.GCO
 * File opened: PI3MK2~5.GCO Size: 11081207
 * File selected
 **/
long request_M23(char *filename)
{
    sprintf(requestCommandInfo.command, "M23 %s\n",filename);
    strcpy(requestCommandInfo.startMagic,echomagic);
    strcpy(requestCommandInfo.stopMagic,"File selected");
    strcpy(requestCommandInfo.errorMagic,"open failed");
    resetRequestCommandInfo();
    storeCmd(requestCommandInfo.command);
    // Wait for response
    while (!requestCommandInfo.done)
    {
//        char *buf=malloc(1000);
//        sprintf(buf, "WAIT DONE CMD:%s \n LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rev_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
//        GUI_DispString(10,BYTE_HEIGHT*10,(u8 *)buf,0);
         loopProcess();
//        sprintf(buf, "wait DONE CMD:%s \n LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rev_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
//       GUI_DispString(10,BYTE_HEIGHT*10,(u8 *)buf,0);
//        free(buf);  
    }

    // Find file size and report its.



//    GUI_Clear(BK_COLOR);
//    GUI_DispStringInRect(0,0,LCD_WIDTH,LCD_HEIGHT,(u8 *)requestCommandInfo.cmd_rev_buf,0);
//    Delay_ms(5000); 


    char *ptr;
    return strtol(strstr(requestCommandInfo.cmd_rev_buf,"Size:")+5, &ptr, 10);
}

/**
 * Start o resume print 
 **/
bool request_M24(int pos)
{
    if(pos == 0){
        storeCmd("M24\n");
    } else {
        char command[100];
        sprintf(command, "M24 S%d\n",pos);
        storeCmd(command);
    }
    return true;
}

/**
 * Abort print 
 **/
bool request_M524(void)
{
    storeCmd("M524\n");
    return true;
}
/**
 * Pause print 
 **/
bool request_M25(void)
{
    storeCmd("M25\n");
    return true;
}

/**
 * Print status ( start auto report)
 * ->  SD printing byte 123/12345
 * ->  Not SD printing
 **/
bool request_M27(int seconds)
{
    char command[10];
    sprintf(command, "M27 S%d\n",seconds);
    storeCmd(command);
    return true;
}

