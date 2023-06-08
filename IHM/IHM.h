#ifndef _IHM_H
#define _IHM_H

/* FILE: IHM.h */
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
/* DESCRIPTION: Fonctions d'acc�s au Lcd �mul� passant par le bus CAN                 */
/****************************************************************************************/

extern int COD_valeur;
extern unsigned short MOT_voltage;
extern short MOT_temp1,MOT_temp2;
extern short MOT_i,MOT_vit;
extern unsigned short MOT_pos;
extern unsigned char JOG_valeur,mise_a_jour_JOG,mise_a_jour_COD;

/****************************************************************************************/
/* Variables globales                                                                   */
/****************************************************************************************/
extern UINT8 curseur;
extern UINT8 IHM_is_running;

/****************************************************************************************/
/* Prototypes                                                                                                            */
/****************************************************************************************/

/****************************************************************************************/ 
/* FUNCTION NAME: IHM_init                                                              */
/* DESCRIPTION  : Initialise l'afficheur IHM                                            */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/ 
void IHM_init(void);


/****************************************************************************************/ 
/* FUNCTION NAME: IHM_clear                                                             */
/* DESCRIPTION  : Efface le LCD                                                         */
/* RETURN       : VOID                                                                  */
/****************************************************************************************/ 
void IHM_clear(void);

/****************************************************************************************/ 
/* FUNCTION NAME: IHM_gotoxy                                                            */
/* DESCRIPTION  : Modifie la position du curseur sur l'afficheur LCD de la carte IHM    */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : UINT8 x : indice de la ligne   (0 � 1)                                  */
/* PARAMETER  2 : UINT8 y : indice de la colonne (0 � 15)                               */
/****************************************************************************************/ 
void IHM_gotoxy (UINT8 x, UINT8 y);

/****************************************************************************************/ 
/* FUNCTION NAME: IHM_printf                                                            */
/* DESCRIPTION  : Affichage format� de type "printf" sur un afficheur LCD de l'IHM      */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char* format  : format d'affichage de printf                          */
/* PARAMETER  2 : ...           : parametres � afficher                                 */
/* Attention : le nouvel affichage doit recouvrir le pr�c�dent.                         */
/*********************** NOTE concernant la commande IHM_printf *************************/ 
/*  Les appels melang�s � printf, LCD_printf et IHM_printf peuvent donner des resultats */
/*                                  imprevisibles                                       */
/*                                                                                      */
/*                 L'IHM doit etre initialise par la fonction IHM_init                  */
/****************************************************************************************/ 
void IHM_printf(const char* format,...) ;


/****************************************************************************************/ 
/* FUNCTION NAME: IHM_sendchar                                                          */
/* DESCRIPTION  : Envoie un caractere � afficher au controleur de l'afficheur LCD       */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char data : caract�re � afficher                                      */
/****************************************************************************************/ 
void IHM_sendchar (char data);


/****************************************************************************************/ 
/* FUNCTION NAME: IHM_sendstring                                                        */
/* DESCRIPTION  : Affiche une chaine sur l'IHM                                          */
/* RETURN       : VOID                                                                  */
/* PARAMETER  1 : char* message : chaine de caractere a afficher                        */
/****************************************************************************************/ 
void IHM_sendstring (char * message);
 

/****************************************************************************************/ 
/* FUNCTION NAME: BAR_set                                                             */
/* DESCRIPTION  : commande les 10 LED du bargraph                                                       */
/* PARAMETER  1 : UINT16 valeur : ordre d'allumage, exprim� sur les 10 bits de poids faibles. 
                  Le bit k doit valoir 0 pour allumer la LED k, 1 pour l'�teindre.
                  Exemple :  ~(1<<9) allume la LED 9, ~0x00F allume les 4 LED 0 � 3                                                                 */
/****************************************************************************************/ 
void BAR_set(UINT16 valeur);


/****************************************************************************************/ 
/* FUNCTION NAME: JOG_read                                                             */
/* DESCRIPTION  : indique la position (parmi les 8 possibles) et l'enfoncement du Jog.                                                      */
/* RETURN       : un octet UINT8 qui indique la position (RIGHT, LEFT, UP, DOWN + 4 combinaisons) 
                  et l'enfoncement (PUSH) du Jog, exprim�s sur 5 bits utiles.
                   
          - Si le Jog est au repos : la fonction renvoie 0 (0x00).
          - si le Jog n'est pas au repos : 1 � 3 bits peuvent �tre mis � 1 dans l'octet renvoy�. 
            Il est int�ressant d'utiliser les masques d�finis en constantes. 
            La signification des 8 bits de la valeur renvoy�e est: 
                      0   0   0   RIGHT   UP   PUSH   LEFT   DOWN
            Plusieurs bits peuvent �tre � 1 en m�me temps. Par exemple :  RIGHT + UP + PUSH
            � 1 indiquent que le Jog est en position "Nord Est " (UP+RIGHT) et qu'il est enfonc�.   
            Si le Jog est simultan�ment enfonc� et positionn� � droite, la fonction renvoie 
            (0001 0100) en binaire, soit 0x14. Exprim�e avec les masques, la valeur renvoy�e vaut 
            JOG_MSK_RIGHT | JOG_MSK_PUSH  (0x14).                                                                                      */
/**************************************************************************************************/ 
UINT8 JOG_read(void);

/***************************************************************************/
/* Constantes symboliques  pour interpr�ter la valeur renvoy�e par le jog :*/                                 
/***************************************************************************/
// les valeurs suivantes peuvent se combiner dans la valeur envoy�e (exemple JOG_MSK_UP | JOG_MSK_PUSH) :

#define JOG_MSK_PUSH   (1<<2)   // valeur renvoy�e si le Jog est enfonc� (en position de repos)
#define JOG_MSK_RIGHT  (1<<4)   // valeur renvoy�e si le Jog est pouss� vers la droite (non enfonc�).
#define JOG_MSK_UP     (1<<3)   // valeur renvoy�e si le Jog est pouss� vers le haut (non enfonc�).
#define JOG_MSK_LEFT   (1<<1)   // valeur renvoy�e si le Jog est pouss� vers la gauche (non enfonc�).
#define JOG_MSK_DOWN   1        // valeur renvoy�e si le Jog est pouss� vers le bas (non enfonc�).



/***************************************************************************************************/ 
/* FUNCTION NAME: COD_read                                                                         */
/* DESCRIPTION  : indique la rotation effectu�e par le codeur incr�mental.                         */   
/* RETURN       : un octet UINT8 dont la valeur entre 0 et 255 indique la rotation du   
                  codeur incr�mental depuis l'initialisation du codeur (cad de la carte IHM), 
                  exprim�e en douzi�mes de tours. Le codeur dispose en effet de 12 positions
                  et la valeur retourn�e s'incr�mente/se d�cr�mente � chaque 12e de tour.
                  Par exemple, si le codeur a �t� tourn� de 2 tours et demi, la valeur 
                  retourn�e vaut 12+12+6 = 30 = 0x1E (le codeur a effectu� 30 douzi�mes de tour)   */
/***************************************************************************************************/ 
SINT8 COD_read(void);


/****************************************************************************************/ 
/* FUNCTION NAME: MOT_set                                                             */
/* DESCRIPTION  : consigne moteur                                                       */
/* PARAMETER  1 : SINT16 valeur : ordre d'allumage, exprim� sur les 10 bits de poids faibles. 
                  Le bit k doit valoir 0 pour allumer la LED k, 1 pour l'�teindre.
                  Exemple :  ~(1<<9) allume la LED 9, ~0x00F allume les 4 LED 0 � 3                                                                 */
/****************************************************************************************/ 
void MOT_set(SINT16 valeur);
void MOT_conf(U8 mode,U8 delay_1,U8 delay_2);
void MOT_cal(UINT16 calage,UINT16 delta);
#endif

