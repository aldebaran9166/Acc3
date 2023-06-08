#include "all_includes.h"



// FIFO CAN
unsigned char FIFO_ecriture=0;
int COD_valeur;
unsigned short MOT_voltage;
short MOT_temp1,MOT_temp2;
short MOT_i,MOT_vit;
unsigned short MOT_pos;
unsigned char JOG_valeur,mise_a_jour_JOG=0,mise_a_jour_COD=0;

void can_ISR_Reader(void)
{
    if (can.read(can_MsgRx[FIFO_ecriture]))
    {
      // FIFO gestion
       FIFO_ecriture=(FIFO_ecriture+1)%SIZE_FIFO;
      // myled[0] = !myled[0];
      // ledC= !ledC;
    }
    

}

void CAN_automate_reception(void)
{
  
  static signed char FIFO_lecture=0,FIFO_occupation=0,FIFO_max_occupation=0;
  FIFO_occupation=FIFO_ecriture-FIFO_lecture;
  if(FIFO_occupation<0)
    FIFO_occupation=FIFO_occupation+SIZE_FIFO;  
  if(FIFO_max_occupation<FIFO_occupation)
    FIFO_max_occupation=FIFO_occupation;
  if(FIFO_occupation>SIZE_FIFO)
    while(!can.write(CANMessage(LCD_OVERFLOW,CANStandard)));
  if(FIFO_occupation!=0)
  {
    switch(can_MsgRx[FIFO_lecture].id)
    {
       
      case JOG_DATA:  JOG_valeur=can_MsgRx[FIFO_lecture].data[0];
                      mise_a_jour_JOG=1;
                      break;
                                    
      case COD_DATA:  COD_valeur=can_MsgRx[FIFO_lecture].data[0];
                      mise_a_jour_COD=1;
                      break;
                      
      case MOT_DAT_1 : MOT_vit=(unsigned short)(can_MsgRx[FIFO_lecture].data[1]*256)+can_MsgRx[FIFO_lecture].data[0];
                       MOT_i=(unsigned short)(can_MsgRx[FIFO_lecture].data[3]*256)+can_MsgRx[FIFO_lecture].data[2];
                       MOT_pos=( short)(can_MsgRx[FIFO_lecture].data[5]*256)+can_MsgRx[FIFO_lecture].data[4];
                       break;
                        
      case MOT_DAT_2 : 
                       MOT_voltage=(unsigned short)(can_MsgRx[FIFO_lecture].data[3]*256)+can_MsgRx[FIFO_lecture].data[2];
                       MOT_temp1=( short)(can_MsgRx[FIFO_lecture].data[5]*256)+can_MsgRx[FIFO_lecture].data[4];
                       MOT_temp2=( short)(can_MsgRx[FIFO_lecture].data[7]*256)+can_MsgRx[FIFO_lecture].data[6];
                       
                       
                       break;
      
                      
     // case RTC_SET:   set_time(1256729737);
     //                 myled[3]=1;  
                        //set_time(*((unsigned long *)can_MsgRx[FIFO_lecture].data[0]));  // Set RTC time to Wed, 28 Oct 2009 11:35:37
                               
                    //  break;
      
      
      default:        break;              
      
    }
    FIFO_lecture=(FIFO_lecture+1)%SIZE_FIFO;
  }

}