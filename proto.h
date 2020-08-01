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
/* 16/02/04 | J.BRIGAUD |  0002  | Ajout prototypes gestion K7              */
/*          |           |        |                                          */
/*----------+-----------+--------+------------------------------------------*/

void DisplayReg      (register Z80 *R);
void Load_Rom        (void);
void Active_Debugger (int a);
void Active_Int      (int a);

/* XO7.C */
/*-------*/
void Print           (byte Cmd, PorT_FX *Port);
void Send_to_K7      (PorT_FX *Port);
void Receive_from_K7 (PorT_FX *Port);
void Init_Etat_Xo7   (void);
void init_k7         (void);
int  open_k7         (char *K7_Name);
void close_k7        (void);
void display_info_k7 (void);

/* T6834.c */
/*---------*/
void Send_to_T6834        (PorT_FX *Port);
void Receive_from_T6834   (byte Cmd, PorT_FX *Port);
int Init_Reponse_T6834    (byte Ordre, byte *Rsp, PorT_FX *Port);
void AffUdk_ON            (byte State);

/* Video.c */
/*---------*/
int  Voir_Xevent  ();
void iniscreen    (char *name);
void stopintr     ();
void startintr    ();
void new_timer    (int a);
void Pset         (byte x, byte y);
void Preset       (byte x, byte y);
void AffCar       (byte x, byte y, byte Car);
void LineClear    (byte y);
void ClrScr       (void);
void Scroll_Video (void);
void Line         (byte x1,byte y1,byte x2,byte y2);
void Aff_Curseur  (void);

/* Lit_ini.c */
/*-----------*/
void   GetProfile_MultiString (char *,char *,int  *,char  [][],char *);
void   GetProfile_String (char *,char *,char *,char  *,char *);
char   GetProfile_Char   (char *,char *,char  ,char  *,char *);
short  GetProfile_Short  (char *,char *,short ,short *,char *);
int    GetProfile_Int    (char *,char *,int   ,int   *,char *);
