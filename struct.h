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

typedef struct 
 {
  byte F0;
  byte F1;
  byte F2;
  byte F3;
  byte F4;
  byte F5;
  byte F6;
  byte F7;
 } PORT_FX;

typedef struct
 {
  PORT_FX R;
  PORT_FX W;
 } PorT_FX;
 
typedef struct
 {
  byte lng_send;
  byte lng_rsp;
  char *Str_Cmd;
 } CMD_T6834;
 
typedef struct
 {
  int size_point_x;
  int size_point_y;
  byte Scroll_Min_Y;
  byte Scroll_Max_Y;
  byte Curs_X;
  byte Curs_Y;
  byte Curseur;
  byte Aff_Udk;
  byte Stick;
  byte Strig;
  byte Strig1;
  FILE *K7_File;
  byte Baud_Out_Mode;
  byte Rem_Canal;
  byte Break;
  byte F_Key [12][80];

 } GENERAL_INFO;
 
 typedef struct
  {
   byte Buff_Key[20];
   byte Nb_Key;
   byte Pt_Lec;
   byte Pt_Ecr;
  } CLAVIER;
