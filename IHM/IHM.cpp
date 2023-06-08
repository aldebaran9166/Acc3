/* FILE: IHM.c */
/****************************************************************************************/
/* MODULE NAME: IHM 16x2                                                                */
/*                                                                                      */
/* AUTHOR     : Jacques-Olivier KLEIN                                                   */
/*              Bruno LARNAUDIE                                                         */
/*                                                                                      */
/* EMAIL      : jacques-olivier.klein@u-psud.fr                                         */
/*              bruno.larnaudie@u-psud.fr                                               */
/*                                                                                      */
/* INSTITUTION: IUT de CACHAN - 9 av. de la div. Leclerc - 94230 CACHAN                 */
/*                                                                                      */
/* DATE       : 12/07/2012                                                              */
/* CPU        : FREESCALE MC9S12XS128MAA                                                */
/* BOARD      : CACHAN Xboard + IHM16x2                                                 */
/* OS         : None                                                                    */
/*                                                                                      */
/* DESCRIPTION: Fonctions d'accès au Lcd émulé passant par le bus CAN                   */
/****************************************************************************************/

#include <stdarg.h>  // Pour va_list ... dans IHM_printf
#include <stdio.h>   // Pour vprintf et setprintf 
#include "all_includes.h"

extern DigitalOut myled[4];
extern unsigned char Reception_CAN;
CANMessage trame_Tx;
extern CANMessage can_MsgRx[SIZE_FIFO];
extern CAN can;       /*Configuration des ports du CAN : rd p30 td p29*/

/****************************************************************************************/
/* Variables globales                                                                   */
/****************************************************************************************/
UINT8 curseur=0;
static char tableau_ecran[32]= {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
UINT8 IHM_is_running = 0;

/****************************************************************************************/
/* FUNCTION NAME: IHM_init                                                              */
/* DESCRIPTION  : Initialise l'afficheur IHM                                            */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/
void IHM_init(void)
{
}


/****************************************************************************************/
/* FUNCTION NAME: IHM_gotoxy                                                            */
/* DESCRIPTION  : Modifie la position du curseur sur l'afficheur LCD de la carte IHM    */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : UINT8 x : indice de la ligne   (0 à 1)                                */
/* PARAMETER  2 : UINT8 y : indice de la colonne (0 à 15)                               */
/****************************************************************************************/
void IHM_gotoxy (UINT8 x, UINT8 y)
{
    curseur=(x*16+y)%32;
}


/****************************************************************************************/
/* FUNCTION NAME: IHM_sendchar                                                          */
/* DESCRIPTION  : Envoie un caractere à afficher au controleur de l'afficheur LCD       */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char data : caractère à afficher                                      */
/****************************************************************************************/
void IHM_sendchar (char data)
{
    tableau_ecran[curseur]=data;
    curseur=(curseur+1)%32;
}


/****************************************************************************************/
/* FUNCTION NAME: IHM_printf                                                            */
/* DESCRIPTION  : Affichage formaté de type "printf" sur un afficheur LCD              */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char* format  : format d'affichage de printf                          */
/* PARAMETER  2 : ...           : parametres à afficher                                 */
/* Attention : le nouvel affichage doit recouvrir le précédent.                         */
/*********************** NOTE concernant la commande IHM_printf *************************/
/*  Les appels melangés à printf, LCD_printf et IHM_printf peuvent donner des resutlats */
/*                                  imprevisibles                                       */
/*                                                                                      */
/*                 L'IHM doit etre initialise par la fonction IHM_init                  */
/****************************************************************************************/
void IHM_printf(const char* format, ...)
{
    //oldcurseur=curseur;
    unsigned char i,j;
    va_list args;
    //sprintf(
    //set_printf(IHM_sendchar); /* Placement des datas dans le tableau */
    //vputc(IHM_sendchar);
    va_start(args, format);
    vsprintf(tableau_ecran+curseur,format, args);
    va_end(args);
    // envoi sur le bus can du tableau complet de l'ecran
    for(j=0; j<4; j++) {
        trame_Tx.id=LCD_CHAR0+j;
        trame_Tx.type=CANData;
        trame_Tx.len=8;
        for(i=0; i<8; i++)
            trame_Tx.data[i]=tableau_ecran[i+j*8];
        while(!can.write(trame_Tx));
    }
}
/****************************************************************************************/
/* FUNCTION NAME: IHM_sendstring                                                        */
/* DESCRIPTION  : Affiche une chaine sur l'IHM                                          */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char* message : chaine de caractere a afficher                        */
/****************************************************************************************/
void IHM_sendstring (char* message)
{
    unsigned char i,j;
    sprintf(tableau_ecran+curseur,message);

    for(j=0; j<4; j++) {
        trame_Tx.id=LCD_CHAR0+j;
        trame_Tx.type=CANData;
        trame_Tx.len=8;
        for(i=0; i<8; i++)
            trame_Tx.data[i]=tableau_ecran[i+j*8];
        while(!can.write(trame_Tx));
    }
}
/****************************************************************************************/
/* FUNCTION NAME: IHM_clear                                                             */
/* DESCRIPTION  : Efface le LCD                                                         */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/
void IHM_clear(void)
{
    UINT8 i;
    curseur=0;
    for(i=0; i<32; i++)
        tableau_ecran[i]=32;
    while(!can.write(CANMessage(LCD_CLEAR,CANStandard)));

}
/****************************************************************************************/
/* FUNCTION NAME: BAR_set                                                             */
/* DESCRIPTION  : commande les 10 LED du bargraph                                                       */
/* PARAMETER  1 : UINT16 valeur : ordre d'allumage, exprimé sur les 10 bits de droite.
                  Le bit k doit valoir 0 pour allumer la LED k, 1 pour l'éteindre.
                  Exemple :  ~(1<<9) allume la LED 9, ~0x00F allume les 4 LED 0 à 3                                                                 */
/****************************************************************************************/
void BAR_set(UINT16 valeur)
{
    trame_Tx.id=BAR_SET;
    trame_Tx.type=CANData;
    trame_Tx.len=2;
    trame_Tx.data[0]=valeur>>8;
    trame_Tx.data[1]=(UINT8)valeur;
    while(!can.write(trame_Tx));
}


/****************************************************************************************/
/* FUNCTION NAME: JOG_read                                                             */
/* DESCRIPTION  : indique la position (parmi les 8 possibles) et l'enfoncement du Jog. */
/* RETURN       : un octet UINT8 qui indique la position (RIGHT, LEFT, UP, DOWN + 4 combinaisons)
                  et l'enfoncement (PUSH) du Jog, exprimés sur 5 bits utiles.*/

/*         - Si le Jog est au repos : la fonction renvoie 0 (0x00).
         - si le Jog n'est pas au repos : 1 à 3 bits peuvent être mis à 1 dans l'octet renvoyé.
           Il est intéressant d'utiliser les masques définis en constantes.
           La signification des 8 bits de la valeur renvoyée est:
                     0   0   0   RIGHT   UP   PUSH   LEFT   DOWN
           Plusieurs bits peuvent être à 1 en même temps. Par exemple :  RIGHT + UP + PUSH
           à 1 indiquent que le Jog est en position "Nord Est " (UP+RIGHT) et qu'il est enfoncé.
           Si le Jog est simultanément enfoncé et positionné à droite, la fonction renvoie
           (0001 0100) en binaire, soit 0x14. Exprimée avec les masques, la valeur renvoyée vaut
           JOG_MSK_RIGHT | JOG_MSK_PUSH  (0x14). */
/**************************************************************************************************/
/**************************************************************************************************/
/* utiliser les Constantes symboliques de IHM.h pour interpréter la valeur renvoyée par le jog... */
/**************************************************************************************************/

UINT8 JOG_read(void)
{
    while(!can.write(CANMessage(JOG_REQ,CANStandard)));
    while(mise_a_jour_JOG==0)
        CAN_automate_reception();
    mise_a_jour_JOG=0;
    return JOG_valeur;

}


/***************************************************************************************************/
/* FUNCTION NAME: COD_read                                                                         */
/* DESCRIPTION  : indique la rotation effectuée par le codeur incrémental.                         */
/* RETURN       : un octet UINT8 dont la valeur entre 0 et 255 indique la rotation du
                  codeur incrémental depuis l'initialisation du codeur (cad de la carte IHM),
                  exprimée en douzièmes de tours. Le codeur dispose en effet de 12 positions
                  et la valeur retournée s'incrémente/se décrémente à chaque 12e de tour.
                  Par exemple, si le codeur a été tourné de 2 tours et demi, la valeur
                  retournée vaut 12+12+6 = 30 = 0x1E (le codeur a effectué 30 douzièmes de tour)   */
/***************************************************************************************************/

SINT8 COD_read(void)
{
    while(!can.write(CANMessage(COD_REQ,CANStandard)));
    while(mise_a_jour_COD==0)
        CAN_automate_reception();
    mise_a_jour_COD=0;
    return COD_valeur;

}


/****************************************************************************************/

/****************************************************************************************/
void MOT_set(SINT16 valeur)
{
    trame_Tx.id=MOT_SET;
    trame_Tx.type=CANData;
    trame_Tx.len=2;
    trame_Tx.data[0]=(UINT8)valeur; //valeur>>8;
    trame_Tx.data[1]=valeur>>8;    //(UINT8)valeur;
    while(!can.write(trame_Tx));
}

void MOT_conf(U8 mode,U8 delay_1,U8 delay_2)
{
    trame_Tx.id=MOT_CONF;
    trame_Tx.type=CANData;
    trame_Tx.len=3;
    trame_Tx.data[0]=mode;
    trame_Tx.data[1]=delay_1;
    trame_Tx.data[2]=delay_2;
    while(!can.write(trame_Tx));
}

void MOT_cal(UINT16 calage,UINT16 delta)
{
    trame_Tx.id=MOT_CAL;
    trame_Tx.type=CANData;
    trame_Tx.len=4;
    trame_Tx.data[0]=(UINT8)calage; //
    trame_Tx.data[1]=calage>>8;     //
    trame_Tx.data[2]=(UINT8)delta;  //
    trame_Tx.data[3]=delta>>8;      //

    while(!can.write(trame_Tx));
}




