
#define SIZE_FIFO 32

extern unsigned char FIFO_ecriture;
extern CAN can;       /*Configuration des ports du CAN : rd p30 td p29*/
extern CANMessage can_MsgRx[SIZE_FIFO];
extern DigitalOut myled[4];
extern DigitalOut Cled;


void can_ISR_Reader(void);

void CAN_automate_reception(void);