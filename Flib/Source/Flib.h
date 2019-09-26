
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


SCR_RECT        ECRAN = {{0, 0, _89_92(159, 239), _89_92(99, 127)}};

/////////////////////////////////////////////////////////////////////////////////////////

unsigned char  *contraste(void);

asm("	contraste:
		move.w	%d0,-(%a7)
		move.w  #4,%d0
		trap    #9
		move.w (%a7)+,%d0
		rts");

void
attend(unsigned short n)
{
  asm("
		boucle:
		move.w	#10200,%%d0
		boucle2:
		dbra	%%d0,boucle2
		dbra	%0,boucle
	"::"d"(n):"d0");
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Dessine une image en vérifiant que c'en est une et qu'elle n'est pas dessinée en
//  dehors de l'écran.
//
//  Puts a picture after checking that it is one and that it is not drawn outside the
//  screen.
/////////////////////////////////////////////////////////////////////////////////////////

void
bitmap_put(short x, short y, unsigned char *pic, short option)
{
	if (*(pic + 256 ** pic + *(pic + 1) + 1) == 0xDF)
		if (x < 240 && y < 128)
			BitmapPut(x, y, pic + 2, &ECRAN, option);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Joue une note de fréquence 'freq' pour une durée 'duree'.
//
//  Plays a note at frequency 'freq' for a time 'duree'.
/////////////////////////////////////////////////////////////////////////////////////////

void
beep(short freq, long duree)
{
	unsigned long  *rombase = (unsigned long *)((*(unsigned long *)0xC8) & 0x600000), hwpb = *(rombase + 65);	// Pour détecter le hardware,
	long            loop, maxloop = freq * duree / 50;	// adaptation de la méthode de JM.
	short           i, j = peekIO(0x60000E);

	OSSetSR(0x0700);
	freq = (hwpb - (unsigned long)rombase < 0x10000
			&& *(unsigned short *)hwpb > 22 ? (*(unsigned long *)(hwpb + 22) == 1 ? 278000 : 364000) : 278000) / freq;
	pokeIO(0x60000C, 6);
	for (loop = 0; loop < maxloop; loop++)
	{
		pokeIO(0x60000E, j ^= 0x03);
		for (i = 0; i <= freq; i--)
			i += 2;
	}
	pokeIO(0x60000E, j &= 0xFC);
	pokeIO(0x60000C, 0x8D);
	OSSetSR(0x0000);
}

/////////////////////////////////////////////////////////////////////////////////////////
//  Renvoie un pointeur vers le tas ou a été placé l'expression 'expr' formatée en 2D.
//
//  Returns a pointeur to the stack where 'expr' has been parsed to 2D.
/////////////////////////////////////////////////////////////////////////////////////////

ESI
parse_2d_expr(char *expr)
{
	push_parse_text(expr);
	return Parse2DExpr(top_estack, FALSE);
}
