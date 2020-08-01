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
#include <stdlib.h>
#include <strings.h>


#define PROCEDURE

#define TYPE_COMMENTAIRE  3
#define TYPE_VARIABLE     2
#define TYPE_TITRE        1
#define ERREUR_TITRE     -1
#define ERREUR_VARIABLE  -2
#define FIN_FICHIER      -3
#define VRAI 1
#define FAUX 0
void   GetProfile_MultiString (char *,char *,int  *,char  [][],char *);
void   GetProfile_String      (char *,char *,char *,char  *,char *);
char   GetProfile_Char        (char *,char *,char  ,char  *,char *);
short  GetProfile_Short       (char *,char *,short ,short *,char *);
int    GetProfile_Int         (char *,char *,int   ,int   *,char *);

static int  lit_param     (FILE *, char *,int *,char [][]);
static int  fget_str      (FILE *,char *,int Max);
static int  enleve_espace (char *);
static void upper         (char *);


/*----------------------------------------------------------------------------*/
/*                      declaration des procedure publique                    */
/*----------------------------------------------------------------------------*/
PROCEDURE int GetProfile_Int (char *Section,
				 char *Variable,
				 int   Defaut,
				 int  *Resultat,
				 char *Fichier_Ini)
{
 char ResultatString[50];
 char DefautString[50];
 int  TmpResultat;

 memset (ResultatString,0,sizeof(ResultatString));
 memset (DefautString,0,sizeof(DefautString));

 snprintf (DefautString,sizeof(DefautString),"%d",Defaut);
 
 GetProfile_String (Section, Variable,DefautString,ResultatString,Fichier_Ini);

 TmpResultat = atoi (ResultatString);
 if (Resultat) *Resultat = TmpResultat;

 return (TmpResultat);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
PROCEDURE char GetProfile_Char (char *Section,
			       char *Variable,
			       char  Defaut,
			       char *Resultat,
			       char *Fichier_Ini)
{
 char ResultatString[50];
 char DefautString[50];
 char TmpResultat;

 snprintf (DefautString,sizeof(DefautString),"%d",(int) Defaut);
 
 GetProfile_String (Section, Variable,DefautString,ResultatString,Fichier_Ini);

 TmpResultat = (char) atoi (ResultatString);
 if (Resultat) *Resultat = TmpResultat;

 return (TmpResultat);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
PROCEDURE short GetProfile_Short (char  *Section,
			          char  *Variable,
			          short  Defaut,
			          short *Resultat,
			          char  *Fichier_Ini)
{
 char  ResultatString[50];
 char  DefautString[50];
 short TmpResultat;

 snprintf (DefautString,sizeof(DefautString),"%d",(int) Defaut);
 
 GetProfile_String (Section, Variable,DefautString,ResultatString,Fichier_Ini);

 TmpResultat = (short) atoi (ResultatString);
 if (Resultat) *Resultat = TmpResultat;

 return (TmpResultat);
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
PROCEDURE void GetProfile_String (char *Section,
				 char *Variable,
				 char *Defaut,
				 char *Resultat,
				 char *Fichier_Ini)
{
 char valeur [10][30];
 int Nombre=0;

 GetProfile_MultiString (Section,Variable,&Nombre, valeur,Fichier_Ini);
 if (valeur) strcpy (Resultat,valeur[0]);
 else        strcpy (Resultat,Defaut);

}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
PROCEDURE void GetProfile_MultiString (char *Section,
				       char *Variable,
                                       int  *nbResultat,
                                       char Resultat[10][30],
				       char *Fichier_Ini)
{
 FILE *fichier;
 char chaine[81],
      valeur [10][30];
 int  nombre,
      type,
      Ligne=0,
      Trouve_Section=FAUX,
      Compteur;
 char Section_p[50];
 char Variable_p[50];

 strcpy (Section_p,Section);
 strcpy (Variable_p,Variable);
 upper (Section_p);
 upper (Variable_p);
 *nbResultat = 0;

 fichier = fopen (Fichier_Ini,"r");
 if (fichier)
    {
     while ((type = lit_param (fichier,chaine,&nombre,valeur)))
      {
       upper (chaine);
       Ligne++;
       switch (type)
        {
	 case TYPE_COMMENTAIRE :
	 case ERREUR_TITRE     :
	 case ERREUR_VARIABLE  : break;

	 case TYPE_TITRE : if (Trouve_Section)
	                    {
	                     *nbResultat = 0;
	                     fclose (fichier);
	                     return;
	                    }
                           if (!strcmp (Section_p,chaine))
			    Trouve_Section = VRAI;
	                   break;

	 case TYPE_VARIABLE : if ((Trouve_Section) && (!strcmp(Variable_p,chaine)))
			       {
			        for (Compteur = 0; (Compteur < nombre) && (Compteur<10); Compteur ++)
				    strcpy (Resultat[Compteur],valeur[Compteur]);
				*nbResultat = Compteur;
				fclose (fichier);
				return;
			       }
			      break;
                              
	 case FIN_FICHIER : *nbResultat = 0;
			    fclose (fichier);
                            return;
	}
      }
     fclose (fichier);
    }
   else
    {
     printf ("Impossible d'ouvrire le fichier d'arguments %s\n",Fichier_Ini);
    }
}


/*----------------------------------------------------------------------------*/
/*                       declaration des procedure privees                    */
/*----------------------------------------------------------------------------*/

PROCEDURE static int lit_param (FILE *fichier, char *Retour,int *nb_param,char param[10][30])
{
 char chaine   [81],
      parametre[51];
 int r,i,
     longueur,
     type;

 Retour [0]='\0';
 *nb_param = 0;

 if (!fget_str (fichier,chaine,81))
  {
   longueur = enleve_espace (chaine);
   switch (chaine[0])
    {
     case '\0':
     case ';' :
     case '#' :
     case '*' : type = TYPE_COMMENTAIRE;
		break;
     case '[' : for (r=1;(r<longueur) && (chaine[r]!=']');r++);
		if (chaine[r]==']')
		 {
		  strncpy (Retour,&chaine[1],r);
		  Retour [r-1]='\0';
		  enleve_espace (Retour);
		  type = TYPE_TITRE;
		 }
		else
		  type = ERREUR_TITRE;
		break;
     default  : for (r=0;(r<longueur) && (chaine[r] != '=');r++);
		if (chaine[r] == '=')
		 {
		  strcpy (Retour,chaine);
		  if (chaine [r-1]==':') Retour[r-1]='\0';
		  else                   Retour[r]  ='\0';
		  enleve_espace (Retour);
		  strcpy (parametre,&chaine[++r]);
		  longueur = enleve_espace (parametre);
		  type = TYPE_VARIABLE;
		  for (r=0;r<longueur;r++)
		   {
		    for (i=0;
			(r<longueur) &&
			(parametre[r]!=' ') &&
			(parametre[r]!=',') &&
			(parametre[r]!=';') &&
			(parametre[r]!='\t');
                        r++)
                        if (parametre[r]=='\\')
			 {
                          switch (parametre[r+1])
			   {
			    case '\\' : param[*nb_param][i++]='\\'; r++; break;
			    case 'a'  : param[*nb_param][i++]='\a'; r++; break;
			    case 'b'  : param[*nb_param][i++]='\b'; r++; break;
			    case 'f'  : param[*nb_param][i++]='\f'; r++; break;
			    case 'n'  : param[*nb_param][i++]='\n'; r++; break;
			    case 'r'  : param[*nb_param][i++]='\r'; r++; break;
			    case 't'  : param[*nb_param][i++]='\t'; r++; break;
			    case 'v'  : param[*nb_param][i++]='\v'; r++; break;
			    default   : param[*nb_param][i++]=parametre[r]; break;
			   }
			 }
                        else
			 {
			  param[*nb_param][i++]=parametre[r];
		         }
		    param[*nb_param][i]='\0';
		    if (param[*nb_param][0]) (*nb_param)++;
		   }
		 }
		else
		 type = ERREUR_VARIABLE;
		break;
    }
  }
 else
  type = FIN_FICHIER;

 return (type);
}


/*----------------------------------------------------------------------*/
/*                    Proc‰dure FGET_STR                                */
/*                                                                      */
/* Lecture d'une ligne dans un fichier texte jusqu'a 'CR'               */
/*----------------------------------------------------------------------*/
/* ENTREE  : FICHIER : fichier € lire.                                  */
/*           MAX     : Taille maximal de la chaine € renvoyer           */
/* SORTIE  : CHAINE  : chaine lu jusqu'a 'CR' ou tronqu‰ si trop longue */
/* RETOURNE: VRAI si fin de fichier rencontr‰                           */
/*           FAUX si fin de fichier pas rencontr‰                       */
/*----------------------------------------------------------------------*/
PROCEDURE static int fget_str(FILE *fichier,char *chaine,int Max)
{
 int  r=0,fin_fichier=0;
 char c=1;

 while (c && !fin_fichier)
  {
   if (feof(fichier)) fin_fichier = 1;
   else
    {
     c = fgetc (fichier);
     if (c=='\n')    c = chaine[r]   = '\0';
     else if (r<(Max-1)) chaine[r++] = c;
          else           chaine[r]   = '\0';
    }
  }
 return (fin_fichier);
}


/*----------------------------------------------------------------------*/
/*                    Proc‰dure ENLEVE_ESPACE                           */
/*                                                                      */
/* Enleve les espace se trouvant au d‰but et € la fin d'une chaine de   */
/*  caractere                                                           */
/*----------------------------------------------------------------------*/
/* ENTREE  : CHAINE  : chaine € modifier.                               */
/* SORTIE  : CHAINE  : chaine modifier avec les espaces en moins        */
/* RETOURNE: la longueur de la chaine modifi‰.                          */
/*----------------------------------------------------------------------*/
PROCEDURE static int enleve_espace (char *chaine)
{
 int r;

 for (r=0;(chaine[r]==' ') || (chaine[r]=='\t');r++);
 if (r) strcpy (chaine,&chaine[r]);
 for (r=strlen (chaine)-1;(r) && ((chaine[r]==' ') || (chaine[r]=='\t'));r--);
 if (r) chaine[r+1]='\0';
 return (strlen(chaine));
}

/*------------------------------------------------------------------*/
/*                        Proc‰dure UPPER                           */
/*                                                                  */
/*        Convertion d'une chaine de caractˆre en majuscule         */
/*------------------------------------------------------------------*/
/* ENTREE  : CHAINE : Chaine de caractˆre € convertir.              */
/* SORTIE  : CHAINE : Chaine de caractˆre convertie.                */
/* RETOURNE: Rien                                                   */
/*------------------------------------------------------------------*/
PROCEDURE static void upper (char *chaine)
{
 int boucle,
     longueur;

  longueur=strlen(chaine);
  for (boucle=0;boucle<longueur;boucle++)
        if ((chaine[boucle] >= 'a') && (chaine[boucle] <= 'z'))
                chaine[boucle] -= 0x20;
}
