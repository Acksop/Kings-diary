
/**************************************************************************
 *
 *  Copyright (C) 2000-2004 François LEIBER <http://leiber.free.fr>
 *
 *	This file is part of Flib.
 *
 *	Flib is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Flib is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *************************************************************************/


#define OPTIMIZE_ROM_CALLS
#define NO_EXIT_SUPPORT
#define NO_CALC_DETECT
#define NO_AMS_CHECK

#define _89						// Remplacer par _92 pour avoir une version 92+

#ifdef _89
	#define USE_TI89
	#define _89_92(x, y) (x)
#else
	#define USE_TI92PLUS
	#define USE_V200
	#define _89_92(x, y) (y)
#endif

#include <tigcclib.h>


short           Temp = 0, Num = 0;
char           *Retour = NULL;

/////////////////////////////////////////////////////////////////////////////////////////
//  Ajoute à 'retour' la chaîne pointée par 'str' en respectant les conventions du TIOS,
//  'retour' sera ensuite stocké dans 'fl'.
//
//  Adds to 'retour' the string pointed by 'str', respecting the TIOS conventions,
//  'retour' will be copied in 'fl' at the end.
/////////////////////////////////////////////////////////////////////////////////////////

void
str_add(char *str)
{
	short           len = strlen(str) + 3;

	if (Num < len)
		return;

	Retour[Num] = 0x2D;
	Retour[Num - 1] = 0;
	Num -= len;
	memcpy(Retour + Num + 2, str, len - 3);
	Retour[Num + 1] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Ajoute à 'retour' l'entier n, 'retour' sera ensuite stocké dans 'fl'.
//
//  Adds to 'retour' the integer n, 'retour' will be copied in 'fl' at the end.
/////////////////////////////////////////////////////////////////////////////////////////

void
int_add(unsigned short n)
{
	short           taille = (n >= 256) ? 4 : (n != 0 ? 3 : 2);

	Retour[Num] = 0x1F;
	Retour[Num - 1] = taille - 2;
	Retour[Num - 2] = n;
	Num -= taille;
	if (taille == 4)
	{
		Retour[Num + 1] = n;
		Retour[Num + 2] = n >> 8;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie la taille de la variable pointée par 'var'.
//
//  Returns the size of the variable pointed by 'var'.
/////////////////////////////////////////////////////////////////////////////////////////

static inline unsigned short
var_len(char *var)
{
	return (2 + *(unsigned short *)var);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un nom formaté à partir du nom brut enregistré dans 'str'.
//
//  Returns a formated name made with the raw name 'str'.
/////////////////////////////////////////////////////////////////////////////////////////

char           *
mk_nam(char *str)
{
	static char     name[19] = { 0 };
	short           len = strlen(str);

	if (len > 17)
		return NULL;

	return strcpy(name + 1, str) + len;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un SYM_ENTRY vers la variable locale de nom formaté 'nom'.
//
//  Returns a SYM_ENTRY pointing to the local variable named 'nom'.
/////////////////////////////////////////////////////////////////////////////////////////

SYM_ENTRY      *
get_temp_entry(char *nom)
{
	SYM_ENTRY      *entry = DerefSym(FindSymInFolder(nom, TempFolderName(Temp)));

	while (*(--nom) != 0 && *nom != 92) ;
	return (entry != NULL && *nom != 92 ? entry : NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un SYM_ENTRY vers la variable dont le nom est stocké dans 'str'.
//
//  Returns a SYM_ENTRY pointing to the symbol whose name is stocked in 'str'.
/////////////////////////////////////////////////////////////////////////////////////////

SYM_ENTRY      *
get_entry(char *str)
{
	char           *nom = mk_nam(str);
	SYM_ENTRY      *entry = get_temp_entry(nom);

	if (entry != NULL)
		return entry;
	return SymFindPtr(nom, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un pointeur vers la variable 'entry'.
//
//  Returns a pointer to variable 'entry'.
/////////////////////////////////////////////////////////////////////////////////////////

char           *
get_ptr(SYM_ENTRY * entry)
{
	if (entry == NULL || entry->handle == H_NULL)
		return NULL;
	return (char *)HeapDeref(entry->handle);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Vérifie que la variable pointée par 'entry' est bien désarchivée.
//
//  Checks that the variable pointed by 'entry' is unarchived.
/////////////////////////////////////////////////////////////////////////////////////////

short
check(char *str)
{
	SYM_ENTRY      *entry = get_entry(str);

	if (entry != NULL && entry->flags.bits.archived != 0)
		return EM_moveSymFromExtMem(mk_nam(str), HS_NULL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Crée un pointeur vers une variable dont le nom est stocké dans 'str',
//  et alloue à la variable une place en mémoire 'taille'.
//
//  Creates a pointer to a variable whose name is stocked in 'str',
//  and allocates for the variable a place 'taille' bytes long in memory.
/////////////////////////////////////////////////////////////////////////////////////////

char           *
mk_ptr(char *str, unsigned short taille)
{
	char           *nom = mk_nam(str);
	SYM_ENTRY      *entry = get_temp_entry(nom);

	if (check(str) == FALSE)
		return NULL;

	entry = DerefSym(entry != NULL ? AddSymToFolder(nom, TempFolderName(Temp)) : SymAdd(nom));
	if (entry == NULL)
		return NULL;

	nom = (char *)HeapDeref(entry->handle = HeapAlloc(taille));
	if (entry->handle == H_NULL)
		return NULL;

	*(unsigned short *)nom = taille - 2;

	return (nom + 2);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Place dans 'tableau' un maximum de 'max_arg' arguments numériques contenus dans 'rang'.
//  Les expressions sont traitées par la TI avec NG_rationalESI.
//
//  Places in 'tableau' a maximum of 'max_arg' numerical arguments stocked in 'rang'.
//  Expressions are calculated by the TI using NG_rationalESI.
/////////////////////////////////////////////////////////////////////////////////////////

char           *
arguments(char *rang, unsigned short *tableau, short max_arg)
{
	short           nbre, puiss10, numero = 0;
	char           *expr, *virgule;

	while ((*rang == ',' || *rang == ' ' || (*rang >= '0' && *rang <= '9')) && numero < max_arg)
	{
		if (*rang != ',')
		{
			nbre = TRUE;
			virgule = NULL;
			if (*rang == ' ')
				rang++;
			expr = rang;
			while (*rang != ',' && *rang != 0)
			{
				if (*rang == '.')
					virgule = rang;
				else if (*rang < '0' || *rang > '9')
					nbre = FALSE;
				rang++;
			}
			if (nbre != FALSE)
			{
				if (virgule == NULL)
					virgule = rang;
				for (tableau[numero] = 0, puiss10 = 1, virgule--; virgule >= expr; virgule--, puiss10 *= 10)
					tableau[numero] += (*virgule - '0') * puiss10;
			}
			else
			{
				*rang = 0;
				push_parse_text(expr);
				if (get_temp_entry(mk_nam(expr)) != NULL)
					push_quantum(LOCALVAR_TAG);
				push_quantum(INT_TAG);
				NG_rationalESI(top_estack);
				estack_to_ushort(top_estack, tableau + numero);
			}
		}
		numero++;
		rang++;
	}
	return rang;
}
