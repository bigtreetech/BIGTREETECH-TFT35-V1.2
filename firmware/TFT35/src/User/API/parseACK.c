#include "parseACK.h"

static const char errormagic[]  = "Error:";
static const char echomagic[]   = "echo:";

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

extern SCROLL   titleScroll;
extern GUI_RECT titleRect;

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
    else if(infoHost.connected && ack_seen(echomagic) && ack_seen("busy:") && ack_seen("processing") && infoMenu.menu[infoMenu.cur] != menuPopup)
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
      else if(infoHost.connected && ack_seen(echomagic))
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



