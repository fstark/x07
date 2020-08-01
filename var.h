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

#ifdef VAR_GLOB
#define LIEU
#else
#define LIEU extern
#endif

/*----------------------------------------------------------------------*/
/*                 Declaration des registres du Z80                     */
/*----------------------------------------------------------------------*/
LIEU Z80     Reg_Xo7;

/*----------------------------------------------------------------------*/
/*    Declaration des registres de L'EPLD de dialogue avec le T6834     */
/*----------------------------------------------------------------------*/
LIEU PorT_FX Port_FX;

/*----------------------------------------------------------------------*/
/*                 Declaration des variables globales                   */
/*----------------------------------------------------------------------*/
LIEU byte *RAM;
LIEU byte Int_nsc800_BB;
LIEU unsigned int Tmp;
LIEU int IT_T6834;
LIEU CLAVIER Clavier;
LIEU GENERAL_INFO General_Info;
