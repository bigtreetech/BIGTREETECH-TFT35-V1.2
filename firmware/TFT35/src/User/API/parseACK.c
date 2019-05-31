#include "parseACK.h"



char ack_rev_buf[ACK_MAX_SIZE];
static u16 ack_index=0;

static char ack_seen(const char *str)
{
  u16 i;	
  for(ack_index=0; ack_index<ACK_MAX_SIZE && ack_rev_buf[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && ack_rev_buf[ack_index+i]!=0 && ack_rev_buf[ack_index+i]==str[i]; i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;      
      return true;
    }
  }
  return false;
}
static char ack_cmp(const char *str)
{
  u16 i;
  for(i=0; i<ACK_MAX_SIZE && str[i]!=0 && ack_rev_buf[i]!=0; i++)
  {
    if(str[i] != ack_rev_buf[i])
    return false;
  }
  if(ack_rev_buf[i] != 0) return false;
  return true;
}


static float ack_value()
{
  return (strtod(&ack_rev_buf[ack_index], NULL));
}

void parseACK(void)
{
  if(infoHost.rx_ok != true) return;      // 未收到应答数据
  if(infoHost.connected == false)         //未连接到打印机
  {
    if(!ack_seen("T:") || !ack_seen("ok"))    goto parse_end;
    infoHost.connected = true;
  }    

  if(ack_cmp("ok\r\n") || ack_cmp("ok\n"))
  {
    infoHost.wait = false;	
  }
  else
  {

    // GCode command response
    bool gcodeProcessed = false;
    if(requestCommandInfo.inWaitResponse && ack_seen(requestCommandInfo.startMagic))
    {
/*       char *buf=malloc(1000);
      sprintf(buf, "Start CMD:%s LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rep_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
      GUI_DispStringInRect(0,BYTE_HEIGHT*1,LCD_WIDTH,LCD_HEIGHT-BYTE_HEIGHT,(u8 *)buf,0);
      free(buf);
 */
      requestCommandInfo.inResponse = true;
      requestCommandInfo.inWaitResponse = false;
      gcodeProcessed = true;
    }
    if(requestCommandInfo.inResponse)
    {
      size_t lln = strlen(ack_rev_buf);
      if(lln + strlen(requestCommandInfo.cmd_rev_buf)  >=  requestCommandInfo.cmd_rev_buf_len)
      {
        char *tmp =  requestCommandInfo.cmd_rev_buf;
        requestCommandInfo.cmd_rev_buf = malloc(lln+strlen(requestCommandInfo.cmd_rev_buf)+1);
        requestCommandInfo.cmd_rev_buf_len+=lln;
        strcpy(requestCommandInfo.cmd_rev_buf,tmp);
        free(tmp);
      }
      strcat(requestCommandInfo.cmd_rev_buf,ack_rev_buf);
      gcodeProcessed = true;
            
/*       char *buf=malloc(1000);
      sprintf(buf, "Recv CMD:%s LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rep_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
      GUI_DispStringInRect(0,(BYTE_HEIGHT*2),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*2),(u8 *)buf,0);
      free(buf);
 */
    }
    if(requestCommandInfo.inResponse && ack_seen(requestCommandInfo.stopMagic))
    {
/*       char *buf=malloc(1000);
      sprintf(buf, "Stop CMD:%s LN:%d %d %d %d \n",requestCommandInfo.command, requestCommandInfo.cmd_rep_buf_len, requestCommandInfo.inResponse, requestCommandInfo.inWaitResponse, requestCommandInfo.done );
      GUI_DispStringInRect(0,(BYTE_HEIGHT*3),LCD_WIDTH,LCD_HEIGHT-(BYTE_HEIGHT*3),(u8 *)buf,0);
      free(buf);
 */
      requestCommandInfo.done = true;
      requestCommandInfo.inResponse = false;
      gcodeProcessed = true;
    }

    // end 

    if(ack_seen("ok"))
    {
      infoHost.wait=false;
    }					
    if(ack_seen("T:")) 
    {
      heatSetCurrentTemp(NOZZLE0, ack_value()+0.5);
      if(ack_seen("B:"))					
      {
        heatSetCurrentTemp(BED,ack_value()+0.5);
      }
    }
    else if(ack_seen("B:"))		
    {
      heatSetCurrentTemp(BED,ack_value()+0.5);
    }
    else if(infoHost.connected && ack_seen(echomagic) && ack_seen(busymagic) && ack_seen("processing") && infoMenu.menu[infoMenu.cur] != menuPopup)
    {
      busyIndicator(STATUS_BUSY);
    }
    else if(infoMenu.menu[infoMenu.cur] != menuPopup)
    {
      if(ack_seen(errormagic))
      {
        popupSetContext((u8* )errormagic, (u8 *)ack_rev_buf + ack_index, textSelect(LABEL_CONFIRM), NULL);
        infoMenu.menu[++infoMenu.cur] = menuPopup;
      }
      else if(ack_seen(busymagic))
      {
        popupSetContext((u8* )busymagic, (u8 *)ack_rev_buf + ack_index, textSelect(LABEL_CONFIRM), NULL);
        infoMenu.menu[++infoMenu.cur] = menuPopup;
      }
      else if(infoHost.connected && ack_seen(echomagic) && !gcodeProcessed)
      {
        popupSetContext((u8* )echomagic, (u8 *)ack_rev_buf + ack_index, textSelect(LABEL_CONFIRM), NULL);
        infoMenu.menu[++infoMenu.cur] = menuPopup;
      }    
    }
  }
  
parse_end: //fixes no connection to printer
  infoHost.rx_ok=false;
  USART1_DMAReEnable();
}



