
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


struct hArbre
{
	struct hArbre  *gauche;
	struct hArbre  *droite;
	unsigned short  valeur;
	unsigned short  priorite;
};

/////////////////////////////////////////////////////////////////////////////////////////
//  Ajoute à 'retour' la chaîne comprise entre 'str1' et 'str2',
//  'retour' sera ensuite stockée dans 'fl'.
//
//  Adds to 'retour' the string between 'str1' and 'str2',
//  'retour' will be copied in 'fl' at the end.
/////////////////////////////////////////////////////////////////////////////////////////

void
str_add2(char *str1, char *str2)
{
	short           len = str2 - str1 + 3;

	if (Num < len)
		return;

	Retour[Num] = 0x2D;
	Retour[Num - 1] = 0;
	Num -= len;
	memcpy(Retour + Num + 2, str1, len - 3);
	Retour[Num + 1] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un pointeur vers le début de la n-ième ligne du texte pointé par 'var'.
//
//  Returns a pointer to the beginning of the n-th line of the texte pointed by 'var'.
/////////////////////////////////////////////////////////////////////////////////////////

unsigned char  *
get_ligne(unsigned char *var, short num)
{
	short           i = 0;
	unsigned char  *n, *n2, *limite = var + *(unsigned short *)var;

	for (n = n2 = var + 5; n <= limite; n++)
	{
		if (*n == 13)
		{
			if (i++ == num)
				return n2;
			n2 = n + 2;
		}
	}
	if (i == num)
		return n2;
	if (i == num - 1)
		return n + 1;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Même chose que get_entry, sauf que ça renvoie un pointeur vers un dossier.
//
//  Same thing as get_entry, except it returns a pointer to a folder.
/////////////////////////////////////////////////////////////////////////////////////////

SYM_ENTRY      *
get_entry_fold(char *str)
{
	return DerefSym(SymFindHome(mk_nam(str)));
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Crée une variable avec mk_ptr, et lui affecte le type perso contenu dans 'extension'.
//
//  Creates a variable with mk_ptr, and affects to it the personnal type 'extension'.
/////////////////////////////////////////////////////////////////////////////////////////

char           *
mk_perso_ptr(char *str, char *extension, unsigned short taille)
{
	unsigned char  *var = mk_ptr(str, taille);

	if (var == NULL)
		return NULL;

	*(var + taille - strlen(extension) - 5) = 0;
	strcpy(var + taille - strlen(extension) - 4, extension);
	*(var + taille - 3) = OTH_TAG;

	return var;
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Convertit l'arbre binaire 'arbre' en un tableau indiquant la longueur et le chemin
//  pour parvenir à n'importe quel caratère.
//
//  Converts the binary tree 'arbre' to a board indicating the length and way to each
//  character.
/////////////////////////////////////////////////////////////////////////////////////////

void
arbre_to_tab(struct hArbre *arbre, short val, short nbre, unsigned long *tampon)
{
	struct hArbre  *gauche = arbre->gauche;
	struct hArbre  *droite = arbre->droite;

	if (gauche == NULL && droite == NULL)
	{
		tampon[arbre->valeur] = ((unsigned long)nbre << 16) + val;
		return;
	}
	if (gauche != NULL)
		arbre_to_tab(gauche, val, nbre + 1, tampon);
	if (droite != NULL)
		arbre_to_tab(droite, val + (1 << nbre), nbre + 1, tampon);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Crée un arbre binaire pour la compression de Huffman, à partir du dictionnaire
//  enregistré dans 'stat' d'après un format personnel.
//
//  Creates a binary tree for the Huffman compression, using the dictionnary stored
//  in 'stat' with personnal conventions.
/////////////////////////////////////////////////////////////////////////////////////////

struct hArbre  *
mk_arbre(struct hArbre *arbre, unsigned char *stat)
{
	short           i, j = 0, place, rang = 256;
	unsigned short  val, min1, min2;
	unsigned char  *n, *limite;

	memset(arbre, 0, 512 * sizeof(struct hArbre));
	for (i = 0; i < 256; i++)
		arbre[i].valeur = i;

	if (*stat != 0)
	{
		for (i = 0; i < 256; i++)
			arbre[i].priorite = stat[(*stat) * i + 1] + (*stat == 2 ? 256 * stat[(*stat) * i + 2] : 0);
	}
	else
	{
		limite = stat + 3 + 3 * stat[1];
		for (n = stat + 3; n < limite; n += 3)
			arbre[*n].priorite = 256 ** (n + 1) + *(n + 2);
		for (n = limite; n < limite + 2 * stat[2]; n += 2)
			arbre[*n].priorite = *(n + 1);
	}

	do
	{
		min1 = min2 = 65535;
		for (place = 0; place < rang; place++)
		{
			val = arbre[place].priorite;
			if (val != 0 && val < min2)
			{
				if (val < min1)
				{
					min2 = min1;
					min1 = val;
					j = i;
					i = place;
				}
				else
				{
					min2 = val;
					j = place;
				}
			}
		}
		if (min2 == 65535)
			break;
		arbre[rang].gauche = &arbre[i];
		arbre[rang].droite = &arbre[j];
		arbre[rang].priorite = min1 + min2;
		rang++;
		arbre[i].priorite = 0;
		arbre[j].priorite = 0;
	}
	while (1);

	return &arbre[i];
}
