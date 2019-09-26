
/**************************************************************************
 *
 *  Flib 3.2 - 04/02/2004
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


#include "Commun.h"
#include "Flib2.h"


/////////////////////////////////////////////////////////////////////////////////////////
//  Procédure de départ.
//
//  Main entry point.
/////////////////////////////////////////////////////////////////////////////////////////

void
_main()
{
	char            retour[5000];
	unsigned char  *str, *nam, *n;
	short           i;
	unsigned short  tableau[9];
	unsigned short  j, rang;
	HANDLE          handle;
	SYM_ENTRY      *entry;
	CESI            arg;

	Retour = retour;
	Num = 4999;
	Temp = 0;
	while (FolderFind(TempFolderName(++Temp)) == 3) ;
	Temp--;

	InitArgPtr(arg);
	while (GetArgType(arg) == STR_TAG)
	{
		str = (unsigned char *)GetStrnArg(arg);

/*------------------------------		Dû à un bug de l'optimisation de ti-gcc (au moins
    FONCTIONS SANS ARGUMENTS			dans les anciennes versions), j'ai dû
--------------------------------		rassembler les fonctions sans arguments au début...*/

		if (strcmp(str, "version") == 0)
			str_add("Flib 3.2 by François Leiber");

		else if (strcmp(str, "reset") == 0)
			OSReset();

		else if (strcmp(str, "getfolder") == 0)
		{
			entry = SymFindFirst(NULL, 4);
			while (entry != NULL)
			{
				str_add(entry->name);
				entry = SymFindNext();
			}
		}

/*-------------------------
    FONCTIONS GENERALES
---------------------------*/

		else if (strncmp(str, "apd:", 4) == 0)
		{
			int_add((unsigned short)OSTimerCurVal(APD_TIMER) / 20);
			arguments(str + 4, tableau, 1);
			OSFreeTimer(APD_TIMER);
			OSRegisterTimer(APD_TIMER, 20 * tableau[0]);
		}

		else if (strncmp(str, "error:", 6) == 0)
		{
			arguments(str + 6, tableau, 1);
			ERD_dialog(tableau[0], FALSE);
		}

/*--------------------
    FONCTIONS MENU
----------------------*/

		else if (strncmp(str, "menu:", 5) == 0)
		{
			short           numero, handlenum;

			for (i = 0; i <= 2; i++)
				tableau[i] = CENTER;
			nam = n = arguments(str + 5, tableau, 3);
			while (*nam != 0 && *(nam++) != ',') ;
			*(nam - 1) = 0;
			handle = PopupNew(n, tableau[0]);
			n = nam;
			handlenum = -1;
			numero = 0;
			while (*n != 0)
			{
				i = *n;
				if (i == ',' || i == '|')
				{
					*n = 0;
					if (i == '|' && handlenum == -1)
						handlenum = 0;
					PopupAddText(handle, handlenum, nam, ++numero);
					nam = n + 1;
					if (i == ',')
						handlenum = -1;
					else if (handlenum == 0)
						handlenum = numero;
				}
				n++;
			}
			PopupAddText(handle, handlenum, nam, numero + 1);
			int_add(PopupDo(handle, tableau[1], tableau[2], 0));
			HeapFree(handle);
			GKeyFlush();
		}

/*---------------------------
    FONCTIONS HIGH-SCORES
-----------------------------*/

		else if (strncmp(str, "mkhs:", 5) == 0)
		{
			tableau[0] = 0;
			nam = n = arguments(str + 5, tableau, 2);
			while (*nam != 0 && *(nam++) != ',') ;
			*(nam - n <= 11 ? nam - 1 : n + 10) = 0;
			nam = mk_ptr(nam, 13 * tableau[0] + 23);
			if (nam != NULL)
			{
				for (rang = 0; rang < tableau[0]; rang++)
				{
					strcpy(nam + 13 * rang, n);
					nam[13 * rang + 11] = tableau[1] >> 8;
					nam[13 * rang + 12] = tableau[1];
				}
				nam[13 * tableau[0]] = 0;
				strcpy(nam + 13 * tableau[0] + 1, "Highscores by Flib");
				nam[13 * tableau[0] + 20] = 0x2D;
			}
		}

		else if (strncmp(str, "puths:", 6) == 0)
		{
			nam = n = arguments(str + 6, tableau, 1);
			while (*nam != 0 && *(nam++) != ',') ;
			*(nam - n <= 11 ? nam - 1 : n + 10) = 0;
			check(nam);
			nam = get_ptr(get_entry(nam));
			if (nam != NULL)
			{
				rang = var_len(nam) - 21;
				for (i = 2; (unsigned short)i < rang; i += 13)
				{
					if (tableau[0] > (unsigned short)(256 * (*(nam + i + 11)) + *(nam + i + 12)))
					{
						if (rang > 15)
							for (j = rang - 13; j >= (unsigned short)i; j -= 13)
							{
								strcpy(nam + j + 13, nam + j);
								nam[j + 24] = nam[j + 11];
								nam[j + 25] = nam[j + 12];
//                              if (j == 2) break ;
							}
						strcpy(nam + i, n);
						nam[i + 11] = tableau[0] >> 8;
						nam[i + 12] = tableau[0];
						break;
					}
				}
				int_add((unsigned short)i > rang ? 0 : i / 13 + 1);
			}
		}

		else if (strncmp(str, "geths:", 6) == 0)
		{
			nam = get_ptr(get_entry(arguments(str + 6, tableau, 1)));
			if (tableau[0] > 0 && 13 * tableau[0] + 12 < *(unsigned short *)nam)
			{
				str_add(nam + 13 * tableau[0] - 11);
				int_add(256 * (*(nam + 13 * tableau[0])) + *(nam + 13 * tableau[0] + 1));
			}
		}

/*---------------------
    FONCTIONS TEXTE
-----------------------*/

		else if (strncmp(str, "numline:", 8) == 0)
		{
			unsigned char  *limite;

			nam = get_ptr(get_entry(str + 8));
			if (*(limite = nam + *(unsigned short *)nam + 1) == 224)
			{
				i = 1;
				for (n = nam + 5; n < limite; n++)
					if (*n == 13)
						i++;
				int_add(i);
			}
		}

		else if (strncmp(str, "getline:", 8) == 0)
		{
			nam = get_ptr(get_entry(arguments(str + 8, tableau, 1)));
			if (*(nam + *(unsigned short *)nam + 1) == 224)
			{
				n = get_ligne(nam, tableau[0] + 1);
				if (n != NULL)
					str_add2(get_ligne(nam, tableau[0]), n - 2);
			}
		}

		else if (strncmp(str, "delline:", 8) == 0)
		{
			char           *tampon, *nom;
			unsigned short  taille;

			HeapCompress();
			i = 0;
			nam = get_ptr(get_entry(nom = arguments(str + 8, tableau, 1)));
			taille = var_len(nam);
			if (*(nam + taille - 1) == 224)
			{
				rang = get_ligne(nam, tableau[0]) - nam;
				n = get_ligne(nam, tableau[0] + 1);
				if (n != NULL)
				{
					j = taille - (n - nam);
					if (get_ligne(nam, tableau[0] + 2) == NULL)
					{
						j += 2;
						if (tableau[0] != 0)
							rang -= 2;
					}
					if ((tampon = malloc(rang + j)) != NULL)
					{
						memcpy(tampon, nam, rang);
						memcpy(tampon + rang, nam + taille - j, j);
						nam = mk_ptr(nom, rang + j);
						if (nam != NULL)
						{
							i = 1;
							memcpy(nam, tampon + 2, rang + j - 2);
						}
					}
					free(tampon);
				}
			}
			int_add(i);
		}

		else if (strncmp(str, "insertline:", 11) == 0)
		{
			char           *tampon, *nom, *add;
			unsigned short  taille;

			HeapCompress();
			i = 0;
			add = nom = arguments(str + 11, tableau, 1);
			while (*add != 0 && *(add++) != ',') ;
			*(add - 1) = 0;
			j = strlen(add);
			nam = get_ptr(get_entry(nom));
			taille = var_len(nam);
			if (*(nam + taille - 1) == 224)
			{
				n = get_ligne(nam, tableau[0]);
				if (n != NULL)
				{
					rang = n - nam;
					if ((tampon = malloc(taille + j + 2)) != NULL)
					{
						memcpy(tampon, nam, rang - 2);
						*(tampon + rang - 2) = 13;
						*(tampon + rang - 1) = 32;
						memcpy(tampon + rang, add, j);
						memcpy(tampon + rang + j, nam + rang - 2, taille - rang + 2);
						if (tableau[0] == 0)
							*(tampon + rang + j) = 13;
						nam = mk_ptr(nom, taille + j + 2);
						if (nam != NULL)
						{
							i = 1;
							memcpy(nam, tampon + 2, taille + j);
						}
					}
					free(tampon);
				}
			}
			int_add(i);
		}

/*-------------------------
    FONCTIONS VARIABLES
---------------------------*/

		else if (strncmp(str, "hide:", 5) == 0)
		{
			entry = get_entry(str + 5);
			if (entry == NULL)
				entry = get_entry_fold(str + 5);
			if (entry != NULL)
			{
				entry->flags.bits.hidden = 1;
				entry->flags.bits.collapsed = 1;
			}
		}

		else if (strncmp(str, "unhide:", 7) == 0)
		{
			entry = get_entry(str + 7);
			if (entry == NULL)
				entry = get_entry_fold(str + 7);
			if (entry != NULL)
				entry->flags.bits.hidden = 0;
		}

		else if (strncmp(str, "size:", 5) == 0)
		{
			entry = get_entry(str + 5);
			int_add(entry != NULL ? var_len(get_ptr(entry)) : 0);
		}

		else if (strncmp(str, "state:", 6) == 0)
		{
			entry = get_entry(str + 6);
			if (entry == NULL)
				entry = get_entry_fold(str + 6);
			if (entry != NULL)
				int_add(4 * entry->flags.bits.hidden + 2 * entry->flags.bits.locked + entry->flags.bits.archived);
		}

		else if (strncmp(str, "type:", 5) == 0)
		{
			n = get_ptr(get_entry(str + 5));
			nam = n + var_len(n) - 2;
			if (n != NULL && *(nam + 1) == OTH_TAG)
			{
				while (*(--nam) != 0) ;
				str_add(nam + 1);
			}
		}

		else if (strncmp(str, "folder:", 7) == 0)
		{
			char            repertoire[10];

			repertoire[0] = 0;
			entry = SymFindFirst(NULL, 4);
			while (entry != NULL)
			{
				if (DerefSym(FindSymInFolder(mk_nam(str + 7), strcpy(repertoire + 1, entry->name) + strlen(entry->name))) != NULL)
					str_add(repertoire + 1);
				entry = SymFindNext();
			}
		}

		else if (strncmp(str, "getfile:", 8) == 0)
		{
			entry = SymFindFirst(mk_nam(n = arguments(str + 8, tableau, 5)), 9);
			while (entry != NULL)
			{
				nam = get_ptr(entry);
				if (*(nam + var_len(nam) - 1) == tableau[0] || n <= str + 9)
					str_add(entry->name);
				entry = SymFindNext();
			}
		}

		else if (strncmp(str, "getbyte:", 8) == 0)
		{
			tableau[1] = 0;
			nam = get_ptr(get_entry(arguments(str + 8, tableau, 2)));
			if (nam != NULL)
			{
				if (tableau[1] != 0)
					str_add2(nam + tableau[0] + 2, nam + tableau[0] + 2 + tableau[1]);
				else
					int_add(*(unsigned char *)(nam + tableau[0] + 2));
			}
		}

		else if (strncmp(str, "setbyte:", 8) == 0)
		{
			check(n = arguments(str + 8, tableau, 2));
			nam = get_ptr(get_entry(n));
			if (nam != NULL)
				*(nam + tableau[0] + 2) = tableau[1];
		}

		else if (strncmp(str, "memcpy:", 7) == 0)
		{
			nam = n = arguments(str + 7, tableau, 3);
			while (*nam != 0 && *(nam++) != ',') ;
			*(nam - 1) = 0;
			check(n);
			n = get_ptr(get_entry(n));
			nam = get_ptr(get_entry(nam));
			if (n != NULL && nam != NULL)
				memcpy(n + tableau[0] + 2, nam + tableau[1] + 2, tableau[2]);
		}

		else if (strncmp(str, "mkvar:", 6) == 0)
		{
			tableau[0] = 0;
			tableau[1] = 224;
			nam = n = arguments(str + 6, tableau, 2);
			while (*nam != ',' && *(nam++) != 0) ;
			if (*(nam - 1) == 0)
			{
				if (tableau[0] > 5)
				{
					nam = mk_ptr(n, tableau[0]);
					if (nam != NULL)
					{
						*(nam + tableau[0] - 3) = tableau[1];
						memset(nam, 0, tableau[0] - 3);
					}
				}
			}
			else
			{
				*(nam - n < 5 ? nam : n + 4) = 0;
				if (tableau[0] > 8)
				{
					nam = mk_perso_ptr(nam + 1, n, tableau[0]);
					if (nam != NULL)
						memset(nam, 0, tableau[0] - 5 - strlen(n));
				}
			}
		}

		else if (strncmp(str, "group:", 6) == 0)
		{
			unsigned char  *var, *nom, *nom_var;
			long            taille = 8;

			tableau[0] = 0;
			i = 0;
			nom = nom_var = n = arguments(str + 6, tableau, 1);
			while (*(n++) != 0)
			{
				if (*n == ',')
				{
					*n = 0;
					nam = get_ptr(get_entry(nom_var));
					if (nam != NULL)
						taille += var_len(nam) + 18;
					*n = ',';
					nom_var = n + 1;
				}
			}

			if (taille > 8)
			{
				n = get_ptr(get_entry(nom_var));
				nam = n + var_len(n) - 8;
				if (n != NULL && memcmp(nam + 3, "GRP", 4) == 0)
				{
					var = mk_perso_ptr("fl", "GRP", var_len(n) + taille - 8);
					if (var != NULL)
					{
						*(nom_var - 1) = 0;
						memcpy(var, n + 2, var_len(n));
						nam = get_ptr(get_entry(nom));
						strcpy(var + var_len(n) - 8, nom);
						memcpy(var + var_len(n) + 10, nam, var_len(nam));
						i = 1;
						check(nom_var);
						SymDel(nom_var);
						SymMove(SYMSTR("fl"), nom_var + strlen(nom_var));
					}
				}
				else
				{
					var = mk_perso_ptr(nom_var, "GRP", taille);
					if (var != NULL)
					{
						n = nom;
						while (*(++n) != 0)
						{
							if (*n == ',')
							{
								*n = 0;
								if (strcmp(nom, nom_var) == 0)
								{
									SymDel(nom);
									break;
								}
								nam = get_ptr(get_entry(nom));
								if (nam != NULL)
								{
									strcpy(var, nom);
									memcpy(var + 18, nam, var_len(nam));
									var += var_len(nam) + 18;
									i++;
								}
								nom = n + 1;
							}
						}
					}
				}
			}
			else
			{
				n = get_ptr(get_entry(nom_var));
				var = n + var_len(n) - 8;
				if (n != NULL && memcmp(var + 3, "GRP", 4) == 0)
				{
					n += 2;
					j = 1;
					while (n < var)
					{
						rang = 256 ** (n + 18) + *(n + 19);
						if (tableau[0] == 0 || tableau[0] == j)
						{
							nam = mk_ptr(n, rang + 2);
							if (nam != NULL)
							{
								memcpy(nam, n + 20, rang);
								i++;
							}
						}
						n += rang + 20;
						j++;
					}
				}
			}
			int_add(i);
		}

		else if (strncmp(str, "ginfo:", 6) == 0)
		{
			n = get_ptr(get_entry(str + 6));
			nam = n + var_len(n) - 8;
			if (n != NULL && memcmp(nam + 3, "GRP", 4) == 0)
			{
				n += 2;
				while (n < nam)
				{
					str_add(n);
					n += 256 * (unsigned short)*(n + 18) + (unsigned short)*(n + 19) + 20;
				}
			}
		}

		else if (strncmp(str, "komp:", 5) == 0)
		{
			struct hArbre  *arbre, *debut, *lecture;
			unsigned char  *tampon, *n2, *limite, *limite2;
			short           chemin[512];
			short           nbre1 = 0, nbre2 = 0, mode;
			unsigned short  val, longueur;
			unsigned long   bit = 0;

			arbre = HeapDeref(handle = HeapAllocHigh(512 * sizeof(struct hArbre)));
			str += 5;
			nam = get_ptr(entry = get_entry(str));
			if (nam != NULL && handle != H_NULL)
			{
				longueur = var_len(nam);
				limite = nam + longueur - 1;
				if (memcmp(limite - 5, "KOMP", 5) != 0)
				{
					if ((tampon = malloc(longueur + 250)) == NULL)
					{
						int_add(3);
						goto Fini;
					}
					if (longueur < 17)
					{
						int_add(2);
						goto Fini;
					}

//                  HeapLock(*(unsigned short*)(tampon-2)) ;
					*(unsigned short *)tampon = *(unsigned short *)nam;
					tampon[2] = *limite;

					memset(chemin, 0, 1024);
					for (n = nam + 2; n < limite; n++)
						chemin[*n]++;
					for (i = 0; i < 256; i++)
					{
						val = chemin[i];
						nbre1 += (val && val < 256);
						nbre2 += (val >= 256);
					}
					mode = nbre2 != 0 ? 2 : 1;
					if (nbre1 + 2 * nbre2 < 255 * mode)
						mode = 0;

					tampon[3] = mode;
					n2 = tampon + 4;
					if (mode)
					{
						for (i = 0; i < 256; i++)
						{
							val = chemin[i];
							*n2++ = val;
							if (mode == 2)
								*n2++ = val >> 8;
						}
					}
					else
					{
						*n2++ = nbre2;
						*n2++ = nbre1;
						for (i = 0; i < 256; i++)
						{
							if ((val = chemin[i]) > 255)
							{
								*n2++ = i;
								*n2++ = chemin[i] >> 8;
								*n2++ = val;
							}
						}
						for (i = 0; i < 256; i++)
						{
							val = chemin[i];
							if (val && val < 256)
							{
								*n2++ = i;
								*n2++ = val;
							}
						}
					}

					arbre_to_tab(mk_arbre(arbre, tampon + 3), 0, 0, (unsigned long *)chemin);

					j = 0;
					limite2 = tampon + longueur - 10;
					for (n = nam + 2; n < limite; n++)
					{
						bit += (unsigned long)(chemin[2 ** n + 1]) << j;
						j += chemin[2 ** n];
						while (j >= 8)
						{
							if (n2 >= limite2)
							{
								int_add(2);
								goto Fini;
							}
							*n2++ = bit;
							bit >>= 8;
							j -= 8;
						}
					}
					if (j > 0)
						*n2++ = bit;

					longueur = n2 - tampon;
					nam = mk_perso_ptr(str, "KOMP", longueur + 9);
				}
				else
				{
					longueur = var_len(nam + 2) - 2;
					if ((tampon = malloc(longueur)) == NULL)
					{
						int_add(3);
						goto Fini;
					}
//                  HeapLock(*(unsigned short*)(tampon-2)) ;
					debut = mk_arbre(arbre, nam + 5);
					n = nam + 6 + 256 * (nam[5] == 1) + 512 * (nam[5] == 2) + (3 * nam[6] + 2 * nam[7] + 2) * (nam[5] == 0);
					n2 = tampon;
					j = 0;
					while (n2 - tampon < longueur - 1)
					{
						lecture = debut;
						do
						{
							if (j == 0)
							{
								nbre1 = *n++;
								j += 8;
							}
							val = nbre1 % 2;
							nbre1 >>= 1;
							j--;
							if (val == 0)
								lecture = lecture->gauche;
							else
								lecture = lecture->droite;
						}
						while (lecture->gauche != NULL || lecture->droite != NULL);
						*n2++ = lecture->valeur;
					}
					*n2 = nam[4];
					nam = mk_ptr(str, longueur + 2);
				}

				if (nam != NULL)
				{
					memcpy(nam, tampon, longueur);
					int_add(0);
				}
				else
					int_add(3);
			  Fini:
				if (tampon != NULL)
					free(tampon);
			}
			else
				int_add(1);
			HeapFree(handle);
		}

		else if (strncmp(str, "kinfo:", 6) == 0)
		{
			nam = get_ptr(get_entry(str + 6));
			if (nam != NULL)
			{
				if (memcmp(nam + var_len(nam) - 6, "KOMP", 5) == 0)
				{
					int_add(nam[4]);
					int_add(var_len(nam + 2));
				}
				else
					int_add(0);
			}
		}
		if (GetArgType(arg) == LIST_TAG)
			SkipArg((ESQ *) arg);
	}

	nam = mk_ptr("fl", 5003 - Num);

	*nam = 0xE5;
	memcpy(nam + 1, Retour + Num + 1, 4999 - Num);
	nam[5000 - Num] = 0xD9;

	return;
}
