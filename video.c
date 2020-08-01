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
/*
                             EMULATEUR

                         Par Edouard FORLER
                    (edouard.forler@di.epfl.ch)
                          Par Sylvain HUET
                    (huet@poly.polytechnique.fr)
                               1997
                         Par Jacques BRIGAUD
                     adaptation UNIX, Octobre 2000

  video.c : routines video de l'emulateur (+gestion memoire)

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <signal.h>

#include <sys/time.h>

#include "Z80.h"
#include "struct.h"
#include "const.h"
#include "var.h"
#include "proto.h"
#include "xo7_car.h"

#define NB_POINT_CAR_X 6
#define NB_POINT_CAR_Y 8
#define MAX_X          120
#define MAX_Y          32

struct itimerval fifty;
struct itimerval Old_Timer;
int              stopkey = 27; // ESC
int		 black,white;
char		*nomstat=NULL;
Display	        *display;
int		 screen;
Colormap	 cmap;
Visual		*visual;
Window		 root,
                 win;
GC		 gc,
                 mygc[256];
short		 imac[400];
XImage		*ima;
XColor		 tCol,tCol_exact;
byte             Ram_Video[MAX_X][MAX_Y];
int		 col[16];
int              crayx,crayy;
int              lasttour;
int              quitf;
static char	*nom[16]=
{"black","red","green","yellow","blue","magenta","cyan","white",
 "gray","pink","pale green","light yellow","light slate blue","violet"
 ,"light cyan","orange"};
static char *Term_Title= { "X_Xo7, l'emulateur Xo7 pour Unix"};


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void iniscreen(char *name)
{
  int	i,j;
  XGCValues xgc;

  display = XOpenDisplay    (nomstat);
  screen  = DefaultScreen   (display);
  visual  = DefaultVisual   (display,screen);
  cmap    = DefaultColormap (display,screen);
  root    = RootWindow      (display,screen);
  black   = BlackPixel      (display,screen);
  white   = WhitePixel      (display,screen);
  gc      = DefaultGC       (display,screen);
  
  /* Allocation des noms de couleurs */
  /*---------------------------------*/
  for (i=0;i<16;i++) 
   {
    XAllocNamedColor(display,cmap,nom[i],&tCol,&tCol_exact);
    col[i]=tCol.pixel;
   }
      
  for (i=0;i<16;i++)
   for (j=0;j<16;j++)
    {
     mygc[i*16+j]=XCreateGC(display,root,0,&xgc);
     XSetForeground(display,mygc[i*16+j],col[i]);
     XSetBackground(display,mygc[i*16+j],col[j]);
    }

      
//win=XCreateSimpleWindow (display, root,0,0,MAX_X*General_Info.size_point_x,MAX_Y*General_Info.size_point_y,1,black,white);
  win=XCreateSimpleWindow (display, root,0,0,MAX_X*General_Info.size_point_x,MAX_Y*General_Info.size_point_y,1,white,black);
  XSetStandardProperties  (display, win,Term_Title,Term_Title,0,NULL,0,NULL);
  XSetWindowColormap      (display,win,cmap);
  XSelectInput            (display,win,ExposureMask|ButtonPressMask|ButtonReleaseMask
                                       |Button2MotionMask|KeyPressMask|KeyReleaseMask);
  XMapWindow              (display,win);
  
  ima=XCreateImage (display,visual,1,XYBitmap,0,(char *) imac,MAX_X*NB_POINT_CAR_X*General_Info.size_point_x,
                                                              MAX_Y*NB_POINT_CAR_Y*General_Info.size_point_y,16,2);
  XFlush(display);
  
  lasttour=15;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Refresh_Video (void)
{
 int x;
 int y;
 int ColorIndex;
 for (x=0;x<MAX_X;x++)
  for (y=0;y<MAX_Y;y++)
   {
    ColorIndex = (Ram_Video[x][y])?0:7;
    XPutImage (display,win,mygc[ColorIndex],ima,0,0,x*General_Info.size_point_x,
                                                    y*General_Info.size_point_y,
                                                      General_Info.size_point_x,
                                                      General_Info.size_point_y);
   }
 Aff_Curseur ();
 //XFlush(display);
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Pset (byte x, byte y)
{
#if AFF_CMD_T6834
 fprintf (stderr,"Pset %d,%d ",x,y);
#endif
 Ram_Video[x][y]=1;
 XPutImage (display,win,mygc[0],ima,0,0,x*General_Info.size_point_x,
                                        y*General_Info.size_point_y,
                                          General_Info.size_point_x,
                                          General_Info.size_point_y);
 XFlush(display);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Preset (byte x, byte y)
{
#if AFF_CMD_T6834
 fprintf (stderr,"Preset %d,%d ",x,y);
#endif
 Ram_Video[x][y]=0;
 XPutImage (display,win,mygc[7],ima,0,0,x*General_Info.size_point_x,
                                        y*General_Info.size_point_y,
                                          General_Info.size_point_x,
                                          General_Info.size_point_y);
 XFlush(display);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void Add_Key (byte Key)
{
 if (Clavier.Nb_Key < 20)
  {
   Clavier.Nb_Key ++;
   Clavier.Buff_Key[Clavier.Pt_Ecr] = Key & 0XFF;
   Clavier.Pt_Ecr ++;
   if (Clavier.Pt_Ecr >=20)
   Clavier.Pt_Ecr = 0;
  }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void Add_FKey (byte F_Key)
{
 byte i;
 if (F_Key < 12)
  for (i=3;(i<50) && (General_Info.F_Key [F_Key][i]);i++)
   Add_Key (General_Info.F_Key [F_Key][i]);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
int Voir_Xevent()
{
 XEvent ev;
 int key=0;
 static int Shift=0;
 static int Control=0;

// startintr();

     while(XEventsQueued(display,QueuedAfterFlush))
      {
       XNextEvent(display,&ev);
       switch(ev.type) 
        {
         case Expose :
                    while(XCheckTypedEvent(display,Expose,&ev))
                        ;
                      fprintf (stderr,"Expose\n");
                      Refresh_Video ();
                      break;

         case ButtonPress : switch(ev.xbutton.button)
                            {
                             case 1  : 
                             case 2  :
                                       crayx=(ev.xbutton.x-20)/2;
                                       crayy=(ev.xbutton.y-20)/2;
                                       break;
                             case 3  : Reg_Xo7.Trace=1;
                                       //quitf = 0;
                             default : break;
                     	    }
                           break;

         case MotionNotify : crayx=(ev.xbutton.x-20)/2;
                             crayy=(ev.xbutton.y-20)/2;
                             fprintf (stderr,"MotionNotify\n");
                             Refresh_Video ();
                             break;

         case ButtonRelease : switch(ev.xbutton.button) 
                               {
                                case 1 : 
                                case 2 : 
                                default: break;
                               }
                              break;
     
         case KeyPress : if ((ev.xkey.keycode&255)==stopkey) quitf=0; 
                         else
                          {
                           key=XKeycodeToKeysym (display,ev.xkey.keycode,Shift);
                           switch (key)
                            {
                             case XK_c :
                             case XK_C :
                                        if (Control == 1)
                                         {
                                          Add_Key (3);
//                                        General_Info.Break=1;
                                         }
                                        else
                                         Add_Key (key & 0XFF);
                                        break;

                            case XK_Shift_L :
                            case XK_Shift_R :
                                 { 
                                  Shift=1; 
                                  key=0;
                                  if (General_Info.Aff_Udk)
                                   AffUdk_ON (1);
                                 }
                            break;
                            
                            case XK_Control_L :
                            case XK_Control_R :
                                 { 
                                  Control=1; 
                                  key=0;
                                 }
                            break;
                            
                            case XK_Up    : General_Info.Stick = 0x31; key=0;break;
                            case XK_Right : General_Info.Stick = 0x32; key=0;break;
                            case XK_Down  : General_Info.Stick = 0x36; key=0;break;
                            case XK_Left  : General_Info.Stick = 0x37; key=0;break;

                            case XK_F1    : Add_FKey (0);break;
                            case XK_F2    : Add_FKey (1);break;
                            case XK_F3    : Add_FKey (2);break;
                            case XK_F4    : Add_FKey (3);break;
                            case XK_F5    : Add_FKey (4);break;
                            case XK_F6    : General_Info.Strig = 0; key=0;Add_FKey (5);break;
                            case XK_F7    : Add_FKey (6);break;
                            case XK_F8    : Add_FKey (7);break;
                            case XK_F9    : Add_FKey (8);break;
                            case XK_F10   : Add_FKey (9);break;
                            case XK_F11   : Add_FKey (10);break;
                            case XK_F12   : Add_FKey (11);break;
                            case XK_KP_Space  : General_Info.Strig1 = 0; key=0;break;

                            default : Add_Key (key & 0XFF);
                                      break;
                           }
                          if (key==0x0d) fputc ('\n',stderr);
                         }
                        break;

         case KeyRelease : key=XKeycodeToKeysym (display,ev.xkey.keycode,0);
                           switch (key)
                            {
                             case XK_Shift_L :
                             case XK_Shift_R :
                                 { 
                                  Shift=0;
                                  if (General_Info.Aff_Udk)
                                   AffUdk_ON (0);
                                 }
                            break;
                            
                            case XK_Control_L :
                            case XK_Control_R :
                                  Control=0; 
                            break;
                            
                            case XK_Up    : General_Info.Stick = 0x30; break;
                            case XK_Right : General_Info.Stick = 0x30; break;
                            case XK_Down  : General_Info.Stick = 0x30; break;
                            case XK_Left  : General_Info.Stick = 0x30; break;

                            case XK_F6    : General_Info.Strig = 0xFF; break;
                            case XK_KP_Space  : General_Info.Strig1 = 0xFF; break;
                           }
                          key=0;
                          break;
                       
         default : break;
        }/* Fin du switch (ev.type) */ 
      } /* Fin du while  (XEventsQueued(di... */
 
// stopintr();

 return (key);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void new_timer(int a)
{
//  #### FReD : ????
//  int d;
//  int b;
//  int c;
 
//   d= 1;
//   b = d+23;
//   c= (d*b)/c;

//    retvideo=128;
//    buf[0xa7c3]|=128;
//    do_irq=1;
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

void startintr()
{
 sigset_t set;
 struct sigaction a;
 sigemptyset(&set);

 a.sa_handler = new_timer;
 a.sa_mask = set;
 a.sa_flags = 0;
 sigaction (SIGALRM, &a, NULL);

 fifty.it_interval.tv_sec=0;
 fifty.it_interval.tv_usec=20000;
 fifty.it_value.tv_sec=0;
 fifty.it_value.tv_usec=20000;
 
/*  signal(SIGALRM,new_timer); */
/*    setitimer(ITIMER_REAL,&fifty,NULL);*/
 setitimer(ITIMER_REAL,&fifty,&Old_Timer);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void stopintr()
{
 fifty.it_interval.tv_sec=0;
 fifty.it_interval.tv_usec=0;
 fifty.it_value.tv_sec=0;
 fifty.it_value.tv_usec=0;

 signal(SIGALRM,NULL);
/*  setitimer(ITIMER_REAL,&fifty,NULL);*/
 setitimer(ITIMER_REAL,&Old_Timer,NULL);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void AffCar(byte x, byte y, byte Car)
{
 int P_x,P_y;
 byte Mask;
 
 /* On efface le caractere precedent */
 /*----------------------------------*/
 XPutImage (display,win,mygc[7],ima,0,0,x*NB_POINT_CAR_X*General_Info.size_point_x,
                                        y*NB_POINT_CAR_Y*General_Info.size_point_y,
                                        NB_POINT_CAR_X*General_Info.size_point_x,
                                        NB_POINT_CAR_Y*General_Info.size_point_y);
 
 /* Dessin du caractere point par point */
 /*-------------------------------------*/
 for (P_y=0;P_y<8;P_y++)
  {
   Mask=0x80;
   for (P_x=0;P_x<6;P_x++)
    {
     if (Car_Def[Car][P_y] & Mask)
      {
       /* Positionnement de la m�moire video */
       /*------------------------------------*/
       Ram_Video[(x*NB_POINT_CAR_X)+P_x][(y*NB_POINT_CAR_Y)+P_y]=1;
       
       /* Affichage d'un point noir si necessaire */
       /*-----------------------------------------*/
       //Pset ((x*MUL_X)+P_x,(y*MUL_Y)+P_y);
       XPutImage (display,win,mygc[0],ima,0,0,(x*NB_POINT_CAR_X*General_Info.size_point_x)+(P_x*General_Info.size_point_x),
                                              (y*NB_POINT_CAR_Y*General_Info.size_point_y)+(P_y*General_Info.size_point_y),
                                              General_Info.size_point_x,
                                              General_Info.size_point_y);
      }
     else
      {
       Ram_Video[(x*NB_POINT_CAR_X)+P_x][(y*NB_POINT_CAR_Y)+P_y]=0;
      }
     Mask >>=1;
    }
  }

 /* Envoi de l'image au serveur X11 */
 /*---------------------------------*/
 XFlush(display);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void LineClear (byte P_y)
{
 byte x,y;
 
 /* Effacement de la m�moire video */
 /*--------------------------------*/
 for (x=0;x<MAX_X;x++)
  for (y=P_y*NB_POINT_CAR_Y;y<(P_y+1)*NB_POINT_CAR_Y;y++)
   Ram_Video[x][y]=0;
 
 /* Effacement dans la fenetre video */
 /*----------------------------------*/  
 XPutImage (display,win,mygc[7],ima,0,0,0,
                                        P_y*(NB_POINT_CAR_Y*General_Info.size_point_y),
                                        (MAX_X*NB_POINT_CAR_X*General_Info.size_point_x),
                                        NB_POINT_CAR_Y*General_Info.size_point_y);
 
 /* Envoi de l'image au serveur X11 */
 /*---------------------------------*/
 XFlush(display);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Scroll_Video (void)
{
 byte x,y;
 
 for (x=0 ; x<MAX_X ; x++)
  for (y = (General_Info.Scroll_Min_Y * NB_POINT_CAR_Y);
       y < (General_Info.Scroll_Max_Y * NB_POINT_CAR_Y);
       y++)
  if (y<((General_Info.Scroll_Max_Y - 1)*NB_POINT_CAR_Y))
   Ram_Video [x][y] = Ram_Video[x][y+8];
  else
   Ram_Video [x][y] = 0;
 Refresh_Video ();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void ClrScr (void)
{
 byte x,y;
 
 for (x=0 ; x<MAX_X ; x++)
  for (y=0 ; y<MAX_Y ; y++)
   Ram_Video [x][y] = 0;
 Refresh_Video ();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Line (byte x1,byte y1,byte x2,byte y2)
{
 byte c;
 
 /* Remise des X et Y dans le bon ordre */
 /*-------------------------------------*/
 if (x2<x1) {c=x2;x2=x1;x1=c;};
 if (y2<y1) {c=y2;y2=y1;y1=c;};
 
 /* Trac� d'un segment horizontal */
 /*-------------------------------*/
 if ((x2-x1)==0)
  {
   for (c=y1;c<=y2;c++)
    Ram_Video[x1][c]=1;
  }
 else
  {
   /* Trac� d'un segment vertical */
   /*-----------------------------*/
   if ((y2-y1)==0)
    {
     for (c=x1;c<=x2;c++)
     Ram_Video[c][y1]=1;
    }
  }
 Refresh_Video ();
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void Aff_Curseur (void)
{
 static byte First=1;
 static byte Loc_x,Loc_y;
 byte x,y;

  
 if (!First)
  {
   if (General_Info.Curseur)
    {
     y = ((Loc_y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) -General_Info.size_point_y;
     x =   Loc_x    * General_Info.size_point_x * NB_POINT_CAR_X;
//     XPutImage (display,win,mygc[7],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,NB_POINT_CAR_Y);
     XPutImage (display,win,mygc[7],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,1);
    }
  }
 else First = 0;
 
 Loc_x = General_Info.Curs_X;
 Loc_y = General_Info.Curs_Y;
 if (General_Info.Curseur)
  {
   y = ((Loc_y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) -General_Info.size_point_y;
   x =   Loc_x    * General_Info.size_point_x * NB_POINT_CAR_X;
//   XPutImage (display,win,mygc[0],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,NB_POINT_CAR_Y);
   XPutImage (display,win,mygc[0],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,1);
   XFlush(display);
  }
}



