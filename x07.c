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
/* 16/02/04 | J.BRIGAUD |  0002  | Ajout de la gestion des K7               */
/*          |           |        |                                          */
/*----------+-----------+--------+------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "Z80.h"

#define VAR_GLOB
#include "struct.h"
#include "const.h"
#include "var.h"
#include "proto.h"

/*
           M�moire du XO7
  0000    ----------------
         |  XXXX  |  RAM  |
  2000    ----------------
         | Carte RAM 8Ko  |
  4000    ----------------
         | Prise ROM 8Ko  |
  6000    ----------------
         | ROM Carte 8Ko  |
  8000    ----------------
         | V-RAM 6Ko      |
  9800    ----------------
         | ~~~~~~~~~~~~~~ |
  A000    ----------------
         | ROM TV 4Ko     |
  B000    ----------------
         | ROM 20Ko BASIC |
  FFFF    ----------------

*/
static byte Mode_K7      = 0;
static byte Presence_k7  = 0; /* Une k7 est presente */
static FILE *Fichier_k7  = 0; /* Fichier de k7 */
static char K7_Name[80];

/*----------------------------------------------------------------------*/
/*                         Programme principal                          */
/*----------------------------------------------------------------------*/
int main (void)
{
 /* Initialisation des variables globales */
 /*---------------------------------------*/
 Tmp = 0;
 IT_T6834 = 0;

 /* Allocation de la memoire du XO7 */
 /*---------------------------------*/
 if (!(RAM= (byte *) malloc (Taille_Ram)))
  {
   fprintf (stderr,"Allocation Memoire impossible!/n");
   exit (1);
  }
  
 /* Effacementt de la memoire */
 /*---------------------------*/
 memset ((void*)RAM,0xFF,Taille_Ram);
 memset ((void*)&Port_FX,0,sizeof (Port_FX));

 /* Reset des registres du XO7 */
 /*----------------------------*/
 ResetZ80 (&Reg_Xo7);
 Reg_Xo7.IPeriod = 100; /* Verification d'IT a tout les cycles     */
 Reg_Xo7.Trace   = 1; /* Mise en route des traces                */
 Int_nsc800_BB   = 0; /* Toutes les IT du NSC800 sont d�valid�es */
 
 /* Chargement des rom du XO7 */
 /*---------------------------*/
 Load_Rom ();

 Reg_Xo7.PC.W = 0xC3C3;
 
 /* Mise en place des touches speciales */
 /*-------------------------------------*/
 signal (SIGINT, Active_Debugger); /* Ctrl C */
 signal (SIGTSTP, Active_Int);     /* Ctrl Z */
 
 /* Initialisation de l'etat du XO7 */
 /*---------------------------------*/
 Init_Etat_Xo7 ();

 /* Init le fichier k7 */
 /*--------------------*/ 
 init_k7 ();
 
 /* Lancement de la fenetre X11 */
 /*-----------------------------*/
 iniscreen ("Terminal Xo7");
 
 /* Lancement du debugueur */
 /*------------------------*/
 RunZ80(&Reg_Xo7);

 /* Ferme le fichier k7 */
 /*---------------------*/ 
 close_k7 ();

 /* Liberation de la memoire */
 /*--------------------------*/
 free (RAM);

 /* Fin du programme */
 /*------------------*/
 return (0);
}

/*----------------------------------------------------------------------*/
/*                 Affichage des registres du Z80                       */
/*----------------------------------------------------------------------*/
void DisplayReg(register Z80 *R)
{
 char Flag[9];
 strcpy (Flag,"        ");
 if (R->AF.B.l & S_FLAG) Flag[0]='S';
 if (R->AF.B.l & Z_FLAG) Flag[1]='Z';
 if (R->AF.B.l & H_FLAG) Flag[3]='H';
 if (R->AF.B.l & V_FLAG) Flag[5]='V';
 if (R->AF.B.l & N_FLAG) Flag[6]='N';
 if (R->AF.B.l & C_FLAG) Flag[7]='C';
 fprintf (stderr,"AF :%04X BC :%04X DE :%04X HL :%04X\n",
           R->AF.W, R->BC.W, R->DE.W, R->HL.W);
 fprintf (stderr,"AF':%04X BC':%04X DE':%04X HL':%04X\n",
           R->AF1.W, R->BC1.W, R->DE1.W, R->HL1.W);
 fprintf (stderr,"PC :%04X SP :%04X IX :%04X IY :%04X\n",
           R->PC.W,R->SP.W,R->IX.W,R->IY.W);
 fprintf (stderr,"I  :%04X IFF:%04X %s\n\n",
           R->I,R->IFF,Flag);
}

/** RdZ80()/WrZ80() ******************************************/
/** These functions are called when access to RAM occurs.   **/
/** They allow to control memory access.                    **/
/************************************ TO BE WRITTEN BY USER **/
void WrZ80(register word Addr,register byte Value)
{
 /* Test de la plage d'adresse */
 /*----------------------------*/
 if ((Addr >= BEG_RAM) &&
     (Addr <  END_RAM))
  RAM[Addr] = Value;
 return;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
byte RdZ80(register word Addr)
{
 return(RAM[Addr]);
}

/** InZ80()/OutZ80() *****************************************/
/** Z80 emulation calls these functions to read/write from  **/
/** I/O ports. There can be 65536 I/O ports, but only first **/
/** 256 are usually used                                    **/
/************************************ TO BE WRITTEN BY USER **/
void OutZ80(register word Port,register byte Value)
{
#if AFF_OUT
 if ((Port!=0xf0) && (Value!=0x44))
 fprintf (stderr,"(%04X) Out %02X,%02X\n",Reg_Xo7.PC.W,Port,Value);
#endif

 switch (Port)
  {
   case 0xBB : /* Validation des interuptions du NSC800 */
               Int_nsc800_BB = Value;
               break;
               
   case 0xF0 : /* Controle des interruptions */
               Port_FX.W.F0 = Value;
               break;
   case 0xF1 : /* XBTR : Stockage des Informations pour CCU */
               Port_FX.W.F1 = Value;
               break;
   case 0xF2 : /* Controle de BAUDS (poids faible) */
               Port_FX.W.F2 = Value;
               break;
   case 0xF3 : /* Controle de BAUDS (poids fort) */
               Port_FX.W.F3 = Value;
               break;
   case 0xF4 : /* Modes */
               Port_FX.W.F4 = Value;
               Port_FX.R.F4 = Value;
               if ((Value & 0x0D) == 0x09) Mode_K7=1;
               else                        Mode_K7=0;
               if ((Mode_K7==1) && (Port_FX.W.F5& 0x04))
                Receive_from_K7 (&Port_FX);
               break;
   case 0xF5 : /* Interruptions (RESET) */
               Port_FX.W.F5 = Value;
               /* Reception d'un octet venant du T6834 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x01)
                Receive_from_T6834 (LEC_T6834_ACK,&Port_FX);
               /* Envoie d'un octet a destination du T6834 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x02)
                Send_to_T6834 (&Port_FX);
               /* Reception d'un octet EN PROVENANCE du lecteur de K7 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x04)
                Receive_from_K7 (&Port_FX);
               /* Envoie d'un octet a destination du lecteur de K7 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x08)
                Send_to_K7 (&Port_FX);
               /* Envoie d'un bit sur le port imprimante  *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x20)
                Print (PRT_DATA,&Port_FX);
               break;
   case 0xF6 : /* Configuration de L'UART */
               Port_FX.W.F6 = Value;
               break;
   case 0xF7 : /* Donn�es �mises par l'UART */
               Port_FX.W.F7 = Value;
               break;
  }
 return;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
byte InZ80(register word Port)
{
 byte Value=0;
 switch (Port)
  {
   case 0xF0 : /* Controle des interruptions */
               Value = Port_FX.R.F0;
               break;
   case 0xF1 : /* XBRR : Stockage des Informations venant du CCU */
               Value = Port_FX.R.F1;
/*               Port_FX.R.F2 &= 0xFE;*/
               break;
   case 0xF2 : /* Flags sur l'etat du Canon XO7 */
/*               if (Port_FX.W.F5 & 0x08) Port_FX.R.F2 |=0x01;
               else                     Port_FX.R.F2 &=0xFE;*/
               if (Port_FX.W.F5 & 0x04) Port_FX.R.F2 |=0x02;
               else                     Port_FX.R.F2 &=0xFD;
               Value = Port_FX.R.F2 | 2;
               break;
   case 0xF3 : /* ~~~Vide~~~ */
               Value = Port_FX.R.F3;
               break;
   case 0xF4 : /* Modes */
               Value = Port_FX.R.F4;
               break;
   case 0xF5 : /* ~~~Vide~~~ */
               Value = Port_FX.R.F5;
               break;
   case 0xF6 : /* Status de l'UART */
               if (Mode_K7) Port_FX.R.F6 |= 0x05;
               Value = Port_FX.R.F6;
               break;
   case 0xF7 : /* Donn�es recu par l'UART */
               Value = Port_FX.R.F7;
               break;
  }
#if AFF_IN
 fprintf (stderr,"(%04X) IN  %02X = %02X\n",Reg_Xo7.PC.W,Port,Value);
#endif
 return (Value);
}

/** PatchZ80() ***********************************************/
/** Z80 emulation calls this function when it encounters a  **/
/** special patch command (ED FE) provided for user needs.  **/
/** For example, it can be called to emulate BIOS calls,    **/
/** such as disk and tape access. Replace it with an empty  **/
/** macro for no patching.                                  **/
/************************************ TO BE WRITTEN BY USER **/
void PatchZ80(register Z80 *R)
{
 fprintf (stderr,"PatchZ80\n");
 return;
}

/** DebugZ80() ***********************************************/
/** This function should exist if DEBUG is #defined. When   **/
/** Trace!=0, it is called after each command executed by   **/
/** the CPU, and given the Z80 registers. Emulation exits   **/
/** if DebugZ80() returns 0.                                **/
/*************************************************************/
#ifndef DEBUG
byte DebugZ80(register Z80 *R)
{
 do
  {
   ExecZ80    (R);
   DisplayReg (R);
  }
 while (1);
 return (0);
}
#endif

/** LoopZ80() ************************************************/
/** Z80 emulation calls this function periodically to check **/
/** if the system hardware requires any interrupts. This    **/
/** function must return an address of the interrupt vector **/
/** (0x0038, 0x0066, etc.) or INT_NONE for no interrupt.    **/
/** Return INT_QUIT to exit the emulation loop.             **/
/************************************ TO BE WRITTEN BY USER **/
word LoopZ80(register Z80 *R)
{
 static int Nb=0;
 static int Lec_K7=0;
 
 
 if (R->IFF & 0x01) 
  {
   /* Traitement si les interruption ont ete valid�es */
   /*-------------------------------------------------*/
  /* fprintf (stderr,"LoopZ80\n");
   Tmp ++;
   if (Tmp%100000 == 0)
    {
     fprintf (stderr,"Loopz80   0X026D: %d   0X020F = %d\n",RAM[0x026d],RAM[0x020f]);
    }
    
   if ((Tmp % 500000 == 0) && (RAM[0x020f]))
    {
     fprintf (stderr,"0X020F = %d\n",RAM[0x020f]);
     RAM[0x020f]--;
    }*/

   Voir_Xevent ();
   if (Clavier.Nb_Key)
    {
     Clavier.Nb_Key --;
     Port_FX.R.F1 = Clavier.Buff_Key[Clavier.Pt_Lec];
     Clavier.Pt_Lec ++;
     if (Clavier.Pt_Lec >=20)
      Clavier.Pt_Lec = 0;
     Port_FX.R.F0  = 0x00;
     Port_FX.R.F2 |= 0x01;
     IT_T6834      = 0;
     return (IT_RST_A);
    }
/*   if (Event)
    {
     Port_FX.R.F0  = 0x00;
     Port_FX.R.F1  = (char) Event;
     Port_FX.R.F2 |= 0x01;
     IT_T6834      = 0;
     return (IT_RST_A);
    }*/
    
   if (General_Info.Break == 1)
    {
     Port_FX.R.F0  = 0x80;
     Port_FX.R.F1  = 0x05;
     Port_FX.R.F2 |= 0x01;
     IT_T6834      = 0;
     General_Info.Break=0;
     fprintf (stderr,"Break\n");
     return (IT_RST_A);
    }
    
   if ( IT_T6834 )
    {
     switch (IT_T6834)
      {
       case 1:
#if AFF_IT
               fprintf (stderr,"It1_6834 0X020F = %d   0X026D: %d\n",RAM[0x020f],RAM[0x026d]);
#endif
               Receive_from_T6834 (LEC_T6834, &Port_FX);
               IT_T6834 = 0;
               return (IT_RST_A);
               break;
       case 2:
//#if AFF_IT
//               fprintf (stderr,"It2_6834 0X020F = %d   0X026D: %d\n",RAM[0x020f],RAM[0x026d]);
               fprintf (stderr,"It2_6834 Nb= %d\n",Nb);
//#endif
                          Port_FX.R.F0  = 0x80;
                          /* Port_FX.R.F1  = 0x04;  Pour 4: demande de l'heure Cmd=1    */
                          /* Port_FX.R.F1  = 0x05;  Pour 5: UDKoff SPoff, Lect @C00E    */
                          /* Port_FX.R.F1  = 0x06;  Pour 6: UDKon Affiche 'Low battery' */
                          /* Port_FX.R.F1  = 0x07;  Pour 7: demande de l'heure Cmd=1    */
                          /* Port_FX.R.F1  = 0x08;  Pour 8: demande de l'heure Cmd=1    */
                          Port_FX.R.F1  = Nb;
                          Port_FX.R.F2 |= 0x01;
                          Nb++; 
                          if ((Nb==4) || (Nb==7)) Nb++;
                          if (Nb>255) Nb=0;
               IT_T6834 = 0;
               return (IT_RST_A);
               break;
       case 3:
               if (Lec_K7 >= 100)
                {
//                 fprintf (stderr,"It3_6834\n");
                 IT_T6834 = 0;
                 Lec_K7 = 0;
                 return (IT_RST_B);
                }
               else
                {
                 Lec_K7 ++;
                }
               break;
      }
    }
   return (INT_NONE);
  }
 else
  return (INT_NONE);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Load_Rom (void)
{
 int  i;
 int  Adresse;
 int  NbResult;
 char Data[10][30];
 char Variable[10];
 FILE *Fichier;
 
 for (i=0; i<10; i++)
  {
   sprintf (Variable,"ROM_%d",i);
   GetProfile_MultiString ("ROM",Variable,&NbResult,Data,INI_FILE);

   if (NbResult > 1)
    {
     Adresse = strtol (Data[1],NULL,0);
     if (!(Fichier=fopen(Data[0],"rb")))
      {
       fprintf (stderr,"Ouverture du fichier ROM impossible (%s).\n",Data[0]);
      }
     else
      {
       fprintf (stderr,"telechargement de la ROM : %s a l'adresse %04X\n",Data[0],Adresse);
       while ((Adresse != 0x10000) && !feof(Fichier))
        {
         RAM [Adresse++] = (byte) fgetc (Fichier);
        }
       fclose (Fichier);
      }
    }
  }
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Active_Debugger (int a)
{
#if AFF_IT
 fprintf (stderr,"Interruption!!!\n");
#endif
 Reg_Xo7.Trace=1;
 signal (a,Active_Debugger);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Active_Int (int a)
{
 static int Type = 2;
// char c;
// ssize_t Res;
#if AFF_IT
 fprintf (stderr,"Int T6834!!! 0X020F = %d   0X026D: %d %d\n",RAM[0x020f],RAM[0x026d],Type);
#endif
 signal (a,Active_Int);
 IT_T6834 = Type;
//   Port_FX.R.F7  = 0xd3;
//   Port_FX.R.F6 |= 0x02;
// Type = (Type==2)?1:2;
//fprintf (stderr,"Fgetc\n");
// Res=read (stdin,(void*)&c,(size_t)1);
//fprintf (stderr,"Fin fgetc %d %d\n",Res,c);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Print (byte Cmd, PorT_FX *Port)
{
 static byte Cpt_Bit=0;
 static byte Data=0;
 
 switch (Cmd)
  {
   case PRT_DATA: if (Port->R.F4 & MASK_PRT_DATA) 
                   Data |= 1;
                  Data <<= 1;
                  Cpt_Bit ++;
                  if (Cpt_Bit == 8)
                   {
                    fprintf (stderr,"Prt_data=0x%02X\n",Data);
                    Cpt_Bit = 0;
                    Data = 0;
                   }
                  break;
   case PRT_ACK: 
                  break;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Send_to_K7 (PorT_FX *Port)
{
 if ((Port->R.F4 & 0x09) == 0x09)
  {
   fprintf (stderr,"%02X ",Port->W.F7);
  }
}

void init_k7 (void)
{
 char Name_k7[80];

 close_k7();
 GetProfile_String ("K7","K7","nul",Name_k7,INI_FILE);
 open_k7 (Name_k7);
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
byte Pgm[60]={
0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,
0x6A,0x62,0x00,0x00,0x00,0x00,0x63,0x05,0x0A,0x00,
0x81,0x20,0x49,0xDD,0x31,0x20,0xBB,0x20,0x31,0x30,
0x30,0x00,0x6B,0x05,0x14,0x00,0x9F,0x20,0x49,0x00,
0x73,0x05,0x1E,0x00,0x82,0x20,0x49,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void display_info_k7 (void)
{
 if (Presence_k7)
  {
   fprintf (stderr,"K7 name : %s\n",K7_Name);
   fprintf (stderr,"File index : %ld\n",ftell (Fichier_k7));
  }
 else
  {
   fprintf (stderr,"Pas de K7 presente!\n");
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int open_k7 (char *K7_Name_i)
{
 int Result_v = RC_KO;
 FILE *Fichier;
 
 /* Ouverture de la mouvelle K7 */
 /*-----------------------------*/
 if ((Fichier = fopen (K7_Name_i,"rb")) != NULL )
  {
   close_k7();
   Fichier_k7 = Fichier;
   Presence_k7 = 1;
   strcpy (K7_Name,K7_Name_i);
   Result_v = RC_OK;
  }
 else
  {
   fprintf (stderr,"Ouverture du fichier k7:%s impossible.\n",K7_Name_i);
  }

 return (Result_v);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void close_k7 (void)
{
 /* Verifier si une k7 est presente */
 /* si c'est le cas, faut la fermer */
 /*---------------------------------*/
 if (Fichier_k7)
  fclose (Fichier_k7);

 memset ((void*)K7_Name,0,sizeof(K7_Name));
 Presence_k7 = 0;
 Fichier_k7  = 0;
 
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Receive_from_K7 (PorT_FX *Port)
{
// static int Cpt=0;
 
// if (!Presence_k7)
//  {
//   Fichier = fopen ("fichiers.xo7/DOMINO.LST","r");
//   Fichier = fopen ("fichiers.xo7/BURGER.LST","r");
//   Fichier = fopen ("fichiers.xo7/KARATE.LST","r");
//   Fichier_k7 = fopen ("fichiers.xo7/AVENTUR.LST","r");
//   Presence_k7 = 1;
//  }
 
 if (Presence_k7 && ((Port->R.F4 & 0x09) == 0x09))
  {
   if (feof(Fichier_k7))
    {
     rewind (Fichier_k7);
    }
   Port->R.F7  = fgetc (Fichier_k7);
   Port->R.F6 |= 0x02;
   IT_T6834 = 3;
//   fprintf (stderr,"Receive from K7 %02X ",Port->R.F7);
//   if (Cpt>=60)
//    Cpt=0;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Init_Etat_Xo7 (void)
{
  GetProfile_Int    ("windows", "size_point_x",3,        &General_Info.size_point_x,INI_FILE);
  GetProfile_Int    ("windows", "size_point_y",3,        &General_Info.size_point_y,INI_FILE);
  GetProfile_Char   ("windows", "Scroll_Min_Y",0,(char*) &General_Info.Scroll_Min_Y,INI_FILE);
  GetProfile_Char   ("windows", "Scroll_Max_Y",4,(char*) &General_Info.Scroll_Max_Y,INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_1" ,"tim?TIME$\r", (char*)General_Info.F_Key[0],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_2" ,"cldCLOAD",    (char*)General_Info.F_Key[1],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_3" ,"locLOCATE",   (char*)General_Info.F_Key[2],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_4" ,"lstLIST",     (char*)General_Info.F_Key[3],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_5" ,"runRUN\r",    (char*)General_Info.F_Key[4],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_6" ,"nul",         (char*)General_Info.F_Key[5],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_7" ,"dat?DATE$\r", (char*)General_Info.F_Key[6],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_8" ,"csaCSAVE",    (char*)General_Info.F_Key[7],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_9" ,"prtPRINT",    (char*)General_Info.F_Key[8],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_10","slpSLEEP",    (char*)General_Info.F_Key[9],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_11","cntCONT\r",   (char*)General_Info.F_Key[10],INI_FILE);
  GetProfile_String ("F_KEY",   "F_KEY_12","nul",         (char*)General_Info.F_Key[11],INI_FILE);
  
  General_Info.Curs_X       = 0;
  General_Info.Curs_Y       = 0;
  General_Info.Curseur      = 0;
  General_Info.Aff_Udk      = 0;
  General_Info.Rem_Canal    = 0;
  General_Info.Stick        = 0x30;
  General_Info.Strig        = 0xFF;
  General_Info.Strig1       = 0xFF;
  General_Info.Break        = 0;
}

