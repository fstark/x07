/*

     CCCCCC       A       NNN     NN   OOOOOOOO   NNN     NN 
    CC          AA AA     NN N    NN  OO      OO  NN N    NN
    CC         AA   AA    NN  N   NN  OO      OO  NN  N   NN
    CC        AA    AA    NN   N  NN  OO      OO  NN   N  NN
    CC       AAAAAAAAAA   NN    N NN  OO      OO  NN    N NN
    CC       AA      AA   NN     NNN  OO      OO  NN     NNN
     CCCCCC  AA      AA   NN      NN   OOOOOOOO   NN      NN 

             XX      XX    OOOOOOOO   7777777777
              XX    XX    OO      OO         77
               XX  XX     OO      OO        77
                XXXX      OO      OO       77
               XX  XX     OO      OO      77
              XX    XX    OO      OO     77
             XX      XX    OOOOOOOO     77
           
*/
/*--------------------------------------------------------------------------*/
/*                         History                                          */
/*--------------------------------------------------------------------------*/
/*   Date   | Author    | Vers.  | Changes                                  */
/*----------+-----------+--------+------------------------------------------*/
/* 18/12/00 | J.BRIGAUD |  0001  | Creation                                 */
/*          |           |        |                                          */
/*----------+-----------+--------+------------------------------------------*/

/* Define pour l'affichage lors du debuggage */
/*-------------------------------------------*/
#define AFF_OUT             0  // Affichage des OUT
#define AFF_IN              0  // Affichage des IN
#define AFF_SND_T6834       0  // Affichage des Octets envoyés au T6834
#define AFF_RCV_T6834       0  // Affichage des Octets envoyés par le T6834
#define AFF_STR_SND_T6834   0  // Affichage du nom de la commande envoyé au T6834
#define AFF_CMD_T6834       0  // Affichage de la traduction des commandes
#define AFF_IT              0  // Affichage de debuggage lors d'une IT
#define AFF_DEBUG           0  // Affichage de debuggage (poubelle)

#define BEG_RAM 0x0000
#define END_RAM 0xA000

#define Taille_Ram 0x10000

#define INIT_T6834     0xf0
#define LEC_T6834      0xf1
#define LEC_T6834_ACK  0xf2

#define PRT_DATA       0x00
#define PRT_ACK        0x01
#define MASK_PRT_DATA  0x20
#define MASK_PRT_ACK   0x40

#define IT_RST_A       0x3C  // Interuptions T6834
#define IT_RST_B       0x34  // Interuptions Liaisons séries et Timer
#define IT_RST_C       0x2C  // Interuptions non utilisées
#define IT_NMI         0x66  // Non Maskable Interrupt

#define INI_FILE       "x07.ini"
#define RC_OK          0
#define RC_KO          1

#define END_PGM        0
#define NOT_END_PGM    1
