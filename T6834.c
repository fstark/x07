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


#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "Z80.h"

#include "struct.h"
#include "const.h"
#include "var.h"
#include "proto.h"
//#include "xo7_car.h"
extern byte Car_Def[256][8];

/*--------------------------------------------------*/
/*                                                  */
/*   Declaration du tableau de commande du T6834    */
/*                                                  */
/* Longueur de l'envoie de la commande              */
/* Longueur de la réponse attendue                  */
/* Chaine de caractere definissant la commande      */
/*                                                  */
/*--------------------------------------------------*/
CMD_T6834 Cmd_T6834[0x48] =
{{   1,   0,"Unknown"},          // 0x00
 {   1,   8,"TimeCall"},         // 0x01
 {   1,   1,"Stick"},            // 0x02
 {   1,   1,"Strig"},            // 0x03
 {   1,   1,"Strig1"},           // 0x04
 {   3,   1,"RamRead"},          // 0x05
 {   4,   0,"RamWrite"},         // 0x06
 {   3,   0,"ScrollSet"},        // 0x07
 {   1,   0,"ScrollExet"},       // 0x08
 {   2,   0,"LineClear"},        // 0x09
 {   9,   0,"TimeSet"},          // 0x0A
 {   1,   0,"CalcDay"},          // 0x0B
 {   9,   0,"AlarmSet"},         // 0x0C
 {   1,   0,"BuzzerOff"},        // 0x0D
 {   1,   0,"BuzzerOn"},         // 0x0E
 {   2, 120,"TrfLine"},          // 0x0F
 {   3,   1,"TestPoint"},        // 0x10
 {   3,   0,"Pset"},             // 0x11
 {   3,   0,"Preset"},           // 0x12
 {   3,   0,"Peor"},             // 0x13
 {   5,   0,"Line"},             // 0x14
 {   4,   0,"Circle"},           // 0x15
 {0x82,   0,"UDKWrite"},         // 0x16
 {   2,0x80,"UDKRead"},          // 0x17
 {   1,   0,"UDKOn"},            // 0x18
 {   1,   0,"UDKOff"},           // 0x19
 {  10,   0,"UDCWrite"},         // 0x1A
 {   2,   8,"UDCRead"},          // 0x1B
 {   1,   0,"UDCInt"},           // 0x1C
 {0x81,   0,"StartPgmWrite"},    // 0x1D
 {0x81,   0,"SPWriteCont"},      // 0x1E
 {   1,   0,"SPOn"},             // 0x1F
 {   1,   0,"SPOff"},            // 0x20
 {   1,0x80,"StartPgmRead"},     // 0x21
 {   1,   1,"OnStat"},           // 0x22
 {   1,   0,"OFFReq"},           // 0x23
 {   4,   0,"Locate"},           // 0x24
// {   38,   0,"Locate"},           // 0x24
 {   1,   0,"CursOn"},           // 0x25
 {   1,   0,"CursOff"},          // 0x26
 {   3,   1,"TestKey"},          // 0x27
 {   2,   1,"TestChr"},          // 0x28
 {   1,   0,"InitSec"},          // 0x29
 {   2,   0,"InitDate"},         // 0x2A
 {   1,   0,"ScrOff"},           // 0x2B
 {   1,   0,"ScrOn"},            // 0x2C
 {   1,   0,"KeyBufferClear"},   // 0x2D
 {   1,   0,"ClsScr"},           // 0x2E
 {   1,   0,"Home"},             // 0x2F
 {   1,   0,"AffUdkOn"},         // 0x30
 {   1,   0,"AffUDKOff"},        // 0x31
 {   1,   0,"RepateKeyOn"},      // 0x32
 {   1,   0,"RepateKeyOff"},     // 0x33
 {   1,   0,"UDK=KANA"},         // 0x34
 {0x82,   0,"UdkContWrite"},     // 0x35
 {   1,   8,"AlarmRead"},        // 0x36
 {   1,   1,"BuzzZero"},         // 0x37
 {   1,   0,"ClickOff"},         // 0x38
 {   1,   0,"ClickOn"},          // 0x39
 {   1,   0,"LocateClose"},      // 0x3A
 {   1,   0,"KeybOn"},           // 0x3B
 {   1,   0,"KeybOff"},          // 0x3C
 {   1,   0,"ExecStartPgm"},     // 0x3D
 {   1,   0,"UnexecStartPgm"},   // 0x3E
 {   1,   0,"Sleep"},            // 0x3F
 {   1,   0,"UDKInit"},          // 0x40
 {   9,   0,"AffUDC"},           // 0x41
 {   1,   8,"ReadCar"},          // 0x42
 {   3,   2,"ScanR"},            // 0x43
 {   3,   2,"ScanL"},            // 0x44
 {   1,   1,"TimeChk"},          // 0x45
 {   1,   1,"AlmChk"}};          // 0x46

byte Date[8]={128,192,224,240,248,252,254,255};

static byte Send_Cmd_T6834 [255];
static byte Locate_OnOff = 0;
static byte Loc_X=0;
static byte Loc_Y=0;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Send_to_T6834 (PorT_FX *Port)
{
 static int Cpt=0;
 static int Lng_Cmd;
 static int Lng_Rsp;
// int i;
 
 if (!Cpt)
  {
   if (Locate_OnOff && 
      ((Port->W.F1 & 0x7F) != 0x24) && 
      ((Port->W.F1) >= 0x20) &&
      ((Port->W.F1) <  0x80))
    {
     General_Info.Curs_X ++;
     //General_Info.Curs_Y ++;
//     fputc (Port->W.F1,stderr);
     fprintf (stderr,"(%02X) %c",Port->W.F1,Port->W.F1);
     AffCar (General_Info.Curs_X,General_Info.Curs_Y,Port->W.F1);
    }
   else
    {
     Locate_OnOff = 0;
//     Loc_X = Loc_Y = 0xff;
     if (((Port->W.F1) & 0x7F) < 0x47)
      {
       Send_Cmd_T6834 [Cpt++] = Port->W.F1 & 0x7F;
       Lng_Cmd = Cmd_T6834[Port->W.F1 & 0x7F].lng_send;
       Lng_Rsp = Cmd_T6834[Port->W.F1 & 0x7F].lng_rsp;
       fprintf (stderr,"0x%02X: Send:%02X Rsp:%d (%d)\n",Port->W.F1,Lng_Cmd,Lng_Rsp,Cpt);
      }
     else
      fprintf (stderr,"Cmd T6834 inconnu ($%02X)[%02X]\n",Port->W.F1,RAM [0x020F]);
    }
  }
 else
  {
   Send_Cmd_T6834 [Cpt++] = Port->W.F1;
   if ((Lng_Cmd & 0x80) && (Cpt>(Lng_Cmd&0x7F)) && (!Port->W.F1))
    {
     fprintf (stderr,"0x%02X: Send:%02X Rsp:%d (%d)[%d]\n",Send_Cmd_T6834 [0],Lng_Cmd,Lng_Rsp,Cpt,Port->W.F1);
     Lng_Cmd = Cpt;
    }
   if (Cpt == 2)
    switch (Send_Cmd_T6834 [0])
     {
      case 0x0C : if (Send_Cmd_T6834 [1] == 0xB0)
                   {
                    ClrScr ();
                    Send_Cmd_T6834 [0] = Send_Cmd_T6834 [1]&0x7f;
                    Lng_Cmd = Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_send;
                    Lng_Rsp = Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_rsp;
                    Cpt --;
                   }
                  break;
      case 0x07 : if (Send_Cmd_T6834 [1] > 4)
                   {
                    fputc (Send_Cmd_T6834[0],stderr);
                    Send_Cmd_T6834 [0] = Send_Cmd_T6834 [1]&0x7f;
                    Lng_Cmd = Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_send;
                    Lng_Rsp = Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_rsp;
                    Cpt --;
                   }
                  break;
     }
       
  }

 if (Cpt && (Cpt == Lng_Cmd))
  {
#if AFF_SND_T6834
   for (i=0; i < Cpt; i++)
    fprintf (stderr,"%02X ",Send_Cmd_T6834[i]);
#endif
#if AFF_STR_SND_T6834
   fprintf (stderr,"(%s) \n",Cmd_T6834[Send_Cmd_T6834[0]].Str_Cmd);
#endif
   Receive_from_T6834 (INIT_T6834,Port);
    Cpt = 0;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Receive_from_T6834 (byte Cmd, PorT_FX *Port)
{
 static byte Rsp[80];
 static int  pt;
 static int  lng_rsp;
 static byte Ordre;
// static byte Day=0;
// byte   x,y;
// byte  i;
 
 switch (Cmd)
  {
   case INIT_T6834    : memset ((void*)Rsp,0,sizeof(Rsp));
                        pt=0;
                        lng_rsp = RAM [0x020F]; // RAM [0x026D];
                        Ordre = Send_Cmd_T6834[0];
#if AFF_DEBUG
                        fprintf (stderr,"INIT_T6834: Ordre=%02X Lng_Rsp=%02X (%02X)\n",Ordre,lng_rsp,Cmd_T6834[Ordre].lng_rsp);
#endif
                        lng_rsp = Init_Reponse_T6834 (Ordre, Rsp, Port);
                        if (lng_rsp)
                         {
                          IT_T6834 = 1;
#if AFF_IT
                          fprintf (stderr,"Attente reponse...\n");
#endif
                         }
                        break;

   case LEC_T6834     : /*if (RAM[0x020F] != 0)*/
                         {
                          Port_FX.R.F0  = 0x40;
                          Port_FX.R.F1  = Rsp[pt];
                          Port_FX.R.F2 |= 0x01;
#if AFF_RCV_T6834
                          fprintf (stderr,"LEC_T6834: Data=%02X\n",Rsp[pt]);
#endif
                         }
                        break;

   case LEC_T6834_ACK : pt++;
#if AFF_IT
                        fprintf (stderr,"LEC_T6834_ACK: IT_");
#endif
                        if (pt < lng_rsp)
                         {
                          IT_T6834 = 1;
#if AFF_IT
                          fprintf (stderr,"ON\n");
#endif
                         }
                        else
#if AFF_IT
                          fprintf (stderr,"OFF\n");
#endif
                        Port_FX.R.F2 &= 0xFE;
                        
                        break;
                        
   default : fprintf (stderr,"Commande au T6834 inconnue[%02X]!!!\n",RAM [0x020F]);
             break;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int Init_Reponse_T6834 (byte Ordre, byte *Rsp, PorT_FX *Port)
{
 static byte R5 = 0;
 int    Lng_rsp;
 word   Adresse;
 byte  i;
 
 Lng_rsp = Cmd_T6834[Ordre].lng_rsp;
 
 switch (Ordre & 0x7F)
  {
   case 0x00: //lng_rsp = Cmd_T6834[Ordre].lng_rsp;
              break;
              
   case 0x01: 
              {
               time_t timer = time(NULL);
               struct tm *my_t;
               my_t = localtime(&timer);
               my_t->tm_year += 1900;
               Rsp[0] = (my_t->tm_year>>8) & 0xFF;
               Rsp[1] = my_t->tm_year & 0xFF;
               Rsp[2] = my_t->tm_mon+1;
               Rsp[3] = my_t->tm_mday;
               Rsp[4] = Date[my_t->tm_wday];
               Rsp[5] = my_t->tm_hour;
               Rsp[6] = my_t->tm_min;
               Rsp[7] = my_t->tm_sec;
              fprintf (stderr,"TimeCall:%d/%d/%d (%d) %d:%d:%d\n",(Rsp [0]<<8)+
                                                                   Rsp [1],
                                                                   Rsp [2],
                                                                   Rsp [3],
                                                                   Rsp [4],
                                                                   Rsp [5],
                                                                   Rsp [6],
                                                                   Rsp [7]);
              }
              break;

   case 0x02: 
#if AFF_CMD_T6834
              fprintf (stderr,"Stick = %02X\n",General_Info.Stick);
#endif
              Rsp[0] = General_Info.Stick;
              break;
   case 0x03: 
#if AFF_CMD_T6834
              fprintf (stderr,"Strig = %02X\n",General_Info.Strig);
#endif
              Rsp[0] = General_Info.Strig;
              break;
   case 0x04: 
#if AFF_CMD_T6834
              fprintf (stderr,"Strig1 = %02X\n",General_Info.Strig1);
#endif
              Rsp[0] = General_Info.Strig1;
              break;
                                     
   case 0x05: Adresse = Send_Cmd_T6834[1] + ( Send_Cmd_T6834[2] << 8);
#if AFF_CMD_T6834
              fprintf (stderr,"Adresse = %04X\n",Adresse);
#endif
              if (!R5) {Rsp[0]=0x0A;R5++;}
              else     {Rsp[0]='0';}
              break;
                                     
   case 0x06:
              Adresse = Send_Cmd_T6834[2] + ( Send_Cmd_T6834[3] << 8);
#if AFF_CMD_T6834
              fprintf (stderr,"Adresse = %04X Data:%02X\n",Adresse,Send_Cmd_T6834[1]);
#endif
              break;
                                     
   case 0x07: // Scroll Set
#if AFF_CMD_T6834
                fprintf (stderr,"Scroll_Set[%d,%d]\n",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
                General_Info.Scroll_Min_Y = Send_Cmd_T6834[1];
                General_Info.Scroll_Max_Y = Send_Cmd_T6834[2]+1;
              break;
                                     
   case 0x08: 
              fputc ('\n',stderr);
              Scroll_Video ();
              break;
                                     
   case 0x09: 
              LineClear (Send_Cmd_T6834[1]);
              break;
                                     
   case 0x0A: 
              fprintf (stderr,"Time set:%d/%d/%d (%d) %d:%d:%d\n",(Send_Cmd_T6834[1]<<8)+
                                                                   Send_Cmd_T6834[2],
                                                                   Send_Cmd_T6834[3],
                                                                   Send_Cmd_T6834[4],
                                                                   Send_Cmd_T6834[5],
                                                                   Send_Cmd_T6834[6],
                                                                   Send_Cmd_T6834[7],
                                                                   Send_Cmd_T6834[8]);
   case 0x0B: // Calc Day (not used)
              break;
                                     
   case 0x0C: // AlarmSet (not used)
              fprintf (stderr,"AlarmSet:[%d %d]%d/%d/%d (%d) %d:%d:%d\n",
                                                                   Send_Cmd_T6834[1],
                                                                   Send_Cmd_T6834[2],
                                                                  (Send_Cmd_T6834[1]<<8)+
                                                                   Send_Cmd_T6834[2],
                                                                   Send_Cmd_T6834[3],
                                                                   Send_Cmd_T6834[4],
                                                                   Send_Cmd_T6834[5],
                                                                   Send_Cmd_T6834[6],
                                                                   Send_Cmd_T6834[7],
                                                                   Send_Cmd_T6834[8]);
              break;
                                     
   case 0x11: 
#if AFF_CMD_T6834
              fprintf (stderr,"Pset %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              Pset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;
                                     
   case 0x12: 
#if AFF_CMD_T6834
              fprintf (stderr,"Preset %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              Preset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;

   case 0x14: // Line (x1,y1)-(x2,y2)
#if AFF_CMD_T6834
              fprintf (stderr,"Line(%d,%d)-(%d,%d)\n", Send_Cmd_T6834[1],
                                                       Send_Cmd_T6834[2],
                                                       Send_Cmd_T6834[3],
                                                       Send_Cmd_T6834[4]);                                     Line (Send_Cmd_T6834[1],
#endif
              Line (Send_Cmd_T6834[1],Send_Cmd_T6834[2],
                    Send_Cmd_T6834[3],Send_Cmd_T6834[4]);
              break;

   case 0x16: // UDKWrite
//#if AFF_CMD_T6834
                 fprintf (stderr,"UDKWrite[%d] ", Send_Cmd_T6834[1]);
                 for (i=2;(i<50) && (Send_Cmd_T6834[i]);i++)
                  if (Send_Cmd_T6834[i]>=' ') fputc (Send_Cmd_T6834[i],stderr);
                  else fprintf (stderr," $%02X",Send_Cmd_T6834[i]);
                 fputc ('\n',stderr);
//#endif
                 strcpy (General_Info.F_Key [Send_Cmd_T6834[1]-1],(char*)&Send_Cmd_T6834[2]);
                 break;

   case 0x18: // UDKOn
   case 0x19: // UDKOff
                 break;

   case 0x1A: // UDCWrite
#if AFF_CMD_T6834
              fprintf (stderr,"UDCWrite[%02X]\n",Send_Cmd_T6834[1]);
#endif
              for (i=0;i<8;i++)
               Car_Def [Send_Cmd_T6834[1]][i] = Send_Cmd_T6834[2+i];
              break;

   case 0x1B: // UDCRead
#if AFF_CMD_T6834
              fprintf (stderr,"UDCRead[%02X]\n",Send_Cmd_T6834[1]);
#endif
              for (i=0;i<8;i++)
               Rsp[i] = Car_Def [Send_Cmd_T6834[1]][i];
              break;

   case 0x1F: // SPOn
   case 0x20: // SPOff
                 break;

   case 0x22: Rsp[0]=0x04; // 0x41
              break;
                                     
   case 0x24: 
#if AFF_CMD_T6834
              fprintf (stderr,"Locate %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              if ((Send_Cmd_T6834[1] == 0) && (Send_Cmd_T6834[2]==General_Info.Curs_Y+1))
               fputc ('\n',stderr);
              if ((Loc_X == Send_Cmd_T6834[1]) && (Loc_Y == Send_Cmd_T6834[2]))
                {
                fprintf (stderr,"Locate_OnOff = 0\n");
                Locate_OnOff = 0;
                }
               else
                {
                fprintf (stderr,"Locate_OnOff = 1\n");
                Locate_OnOff = 1;
                }
              Loc_X = General_Info.Curs_X = Send_Cmd_T6834[1];
              Loc_Y = General_Info.Curs_Y = Send_Cmd_T6834[2];
              if (Send_Cmd_T6834[3])
               {
                fputc (Send_Cmd_T6834[3],stderr);
                AffCar (Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
               }
              else
               {
                Aff_Curseur ();
               }
#if AFF_CMD_T6834
              fputc ('\n',stderr);
#endif
              break;
              
   case 0x25: // CursOn
#if AFF_CMD_T6834
              fprintf (stderr,"Curseur ON\n");
#endif
              General_Info.Curseur = 1;
              Aff_Curseur ();
              break;

   case 0x26: // CursOff
#if AFF_CMD_T6834
              fprintf (stderr,"Curseur OFF\n");
#endif
              General_Info.Curseur = 0;
              break;
              
   case 0x2D: // KeyBufferClear
              break;

   case 0x30: // UDKOn
              General_Info.Aff_Udk = 1;
              AffUdk_ON (0);
              break;
   case 0x31: // UDKOff
              General_Info.Aff_Udk = 0;
              LineClear (3);
              break;
              
   case 0x3b: // KeybOn
   case 0x3c: // KeybOff
                 break;

   case 0x3F: // Sleep
#if AFF_CMD_T6834
              fprintf (stderr,"Sleep\n");
#endif
                 Reg_Xo7.Trace=1;
                 break;

   default:    fprintf (stderr,"(%s) \n",Cmd_T6834[Send_Cmd_T6834[0]].Str_Cmd);
               break;

  }
 return (Lng_rsp);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void AffUdk_ON (byte State)
{
 byte Offset,i,j;
 byte x=0;
 
 Offset = (State)?1:0;
 for (i=0;i<5;i++)
  {
   AffCar (x++,3,131);
   for (j=0;j<3;j++)
    AffCar (x++,3,General_Info.F_Key[i+(6*Offset)][j]);
  }
}

