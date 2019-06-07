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

void ackPopupInfo(const char *info)
{
    popupDrawPage(&bottomSingleBtn ,(u8* )info, (u8 *)ack_rev_buf + ack_index, textSelect(LABEL_CONFIRM), NULL);    
    if(infoMenu.menu[infoMenu.cur] != menuPopup)
      infoMenu.menu[++infoMenu.cur] = menuPopup;
}

void parseACK(void)
{
  if(infoHost.rx_ok != true) return;      //not get response data
  if(infoHost.connected == false)         //not connected to Marlin
  {
    if(!ack_seen("T:") || !ack_seen("ok"))    goto parse_end;  //the first response should be such as "T:25/50 ok\n"
    infoHost.connected = true;
  }    

  // GCode command response
  bool gcodeProcessed = false;
  if(requestCommandInfo.inWaitResponse && ack_seen(requestCommandInfo.startMagic))
  {
    requestCommandInfo.inResponse = true;
    requestCommandInfo.inWaitResponse = false;
    gcodeProcessed = true;
  }
  if(requestCommandInfo.inResponse)
  {
    if(strlen(requestCommandInfo.cmd_rev_buf)+strlen(ack_rev_buf) < CMD_MAX_REV)
    {
        strcat(requestCommandInfo.cmd_rev_buf,ack_rev_buf);
    }
    else 
    {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        ackPopupInfo(errormagic);
    }
    gcodeProcessed = true;
  }
  if(requestCommandInfo.inResponse && ( ack_seen(requestCommandInfo.stopMagic) || ack_seen(requestCommandInfo.errorMagic) ))
  {
    requestCommandInfo.done = true;
    requestCommandInfo.inResponse = false;
    gcodeProcessed = true;
  }
  // end 

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
      heatSetCurrentTemp(heatGetCurrentToolNozzle(), ack_value()+0.5);
      for(TOOL i = BED; i < HEATER_NUM; i++)
      {
        if(ack_seen(toolID[i])) 
        {
          heatSetCurrentTemp(i, ack_value()+0.5);
        }
      
      }
    }
#if defined ONBOARD_SD_SUPPORT && !defined M27_AUTOREPORT    
    if(infoPrinting.printing && OS_GetTime() - infoPrinting.lastUpdate  > M27_REFRESH * 1000) {
       request_M27(0); 
       infoPrinting.lastUpdate = OS_GetTime();
    }
#endif    
    else if(ack_seen("B:"))		
    {
      heatSetCurrentTemp(BED,ack_value()+0.5);
    }
    else if(infoHost.connected && ack_seen(echomagic) && ack_seen(busymagic) && ack_seen("processing") && infoMenu.menu[infoMenu.cur] != menuPopup)
    {
      busyIndicator(STATUS_BUSY);
    }
#ifdef ONBOARD_SD_SUPPORT     
    else if(ack_seen(bsdnoprintingmagic) && infoMenu.menu[infoMenu.cur] == menuBSDPrinting)
    {
      infoPrinting.printing = false;
      infoPrinting.lastUpdate = OS_GetTime();
      endPrinting();
    }
    else if(ack_seen(bsdprintingmagic))
    {
      if(infoMenu.menu[infoMenu.cur] != menuBSDPrinting) {
          infoMenu.menu[++infoMenu.cur] = menuBSDPrinting;
      }
      // Parsing printing data
      // SD printing byte 123/12345
      char *ptr;
      setPrintCur(strtol(strstr(ack_rev_buf,"byte ")+5, &ptr, 10));
      infoPrinting.lastUpdate = OS_GetTime();
    }    
#endif    
    else if(ack_seen(errormagic))
    {
        ackPopupInfo(errormagic);
    }
    else if(ack_seen(busymagic))
    {
        ackPopupInfo(busymagic);
    }
    else if(infoHost.connected && ack_seen(echomagic) && !gcodeProcessed)
    {
        ackPopupInfo(echomagic);
    }
  }
  
parse_end:
  infoHost.rx_ok=false;
  USART1_DMAReEnable();
}



