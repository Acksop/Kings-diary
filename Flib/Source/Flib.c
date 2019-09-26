
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
#include "Flib.h"


/////////////////////////////////////////////////////////////////////////////////////////
//  Procédure de départ.
//
//  Main entry point.
/////////////////////////////////////////////////////////////////////////////////////////

void
_main()
{
	short           i, mode_gray = -1;
	unsigned short  j;
	unsigned short  tableau[9];
	char           *str, *nam;
	char            retour[1500];
	SCR_RECT        rect;
	LCD_BUFFER      lcd;
	CESI            arg;

	Retour = retour;
	Num = 1499;
	Temp = 0;
	while (FolderFind(TempFolderName(++Temp)) == 3) ;
	Temp--;

	InitArgPtr(arg);
	while (GetArgType(arg) == STR_TAG)
	{
		str = (char *)GetStrnArg(arg);

/*------------------------------		Dû à un bug de l'optimisation de ti-gcc (au moins
    FONCTIONS SANS ARGUMENTS			dans les anciennes versions), j'ai dû
--------------------------------		rassembler les fonctions sans arguments au début...*/

		if (strcmp(str, "version") == 0)
			str_add("Flib 3.2 by François Leiber");

		else if (strcmp(str, "off") == 0)
			off();

		else if (strcmp(str, "clrscr") == 0)
			ClrScr();

		else if (strcmp(str, "+") == 0)
			OSContrastUp();

		else if (strcmp(str, "-") == 0)
			OSContrastDn();

		else if (strcmp(str, "breakoff") == 0)
		{
			OSClearBreak();
			OSDisableBreak();
			OSSetSR(0x0700);
		}

		else if (strcmp(str, "slclr") == 0)
			DrawClipRect(MakeWinRect(0, _89_92(94, 122), 239, _89_92(99, 127)), &ECRAN, 0);

		else if (strcmp(str, "slrcl") == 0)
			DrawLine(0, _89_92(93, 121), _89_92(159, 239), _89_92(93, 121), 1);

		else if (strcmp(str, "getcont") == 0)
			int_add(*contraste());

		else if (strcmp(str, "keywait") == 0)
			int_add(ngetchx());

		else if (strcmp(str, "keylow") == 0)
		{
			unsigned char   touches[9];
#ifdef _89
			char            ordre[] = { 16, 32, 64, 128, 2, 8, 1, 4 };
#else
			char            ordre[] = { 1, 4, 2, 8, 16, 64, 32, 128 };
#endif

			j = _rowread(0xfffe);
			for (i = 0; i < 8; i++)
				touches[i] = j & ordre[i] ? '1' : '0';
			touches[8] = 0;
			str_add(touches);
		}


/*-------------------------
    FONCTIONS GENERALES
---------------------------*/

		else if (strncmp(str, "setcont:", 8) == 0)
		{
			arguments(str + 8, tableau, 1);
			*contraste() = tableau[0];
			OSContrastDn();
			OSContrastUp();
		}

		else if (strncmp(str, "wait:", 5) == 0)
		{
			tableau[0] = 0;
			arguments(str + 5, tableau, 1);
			attend(10 * tableau[0]);
		}

		else if (strncmp(str, "beep:", 5) == 0)
		{
			tableau[1] = 4;
			arguments(str + 5, tableau, 2);
			beep(tableau[0] + 1, tableau[1]);
		}

		else if (strncmp(str, "beep2:", 6) == 0)
		{
			short           freqs[] = { 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
				262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
				523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
				1046, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
				2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
			};
			short           durees[] = { 24000, 12000, 9000, 6000, 4500, 3000, 1500, 750, 2000 };

			i = 2;
			j = 3;
			tableau[0] = 0;
			nam = arguments(str + 6, tableau, 1);
			if (tableau[0] == 0)
				tableau[0] = 120;
			for (; *nam != 0; nam++)
			{
				if (*nam < 'A')
					j = *nam - '1';
				else if (*nam <= 'L')
					beep(freqs[12 * i + *nam - 'A'], durees[j] / tableau[0]);
				else if (*nam == 'O')
					i = min(4, max(0, *(++nam) - '0'));
				else if (*nam == 'P')
					attend(durees[j] / tableau[0]);
				if (OSdequeue(&j, kbd_queue()) == FALSE)
					break;
			}
		}

/*--------------------------
    FONCTIONS GRAPHIQUES
----------------------------*/

		else if (strncmp(str, "gray", 4) == 0)
		{
			if (mode_gray == -1)
			{
				LCD_save(lcd);
				mode_gray = str[4];
				GrayOn();
				SetPlane(0);
				ClrScr();
				SetPlane(1);
				ClrScr();
			}
			else if (mode_gray == '2')
			{
				GrayOff();
				LCD_restore(lcd);
				mode_gray = -1;
			}
		}

		else if (strncmp(str, "plane:", 6) == 0)
		{
			SetPlane(str[6] - '0');
		}

		else if (strncmp(str, "sprite:", 7) == 0)
		{
			*(long *)tableau = 0;
			tableau[2] = 1;
			nam = arguments(str + 7, tableau, 3);
			bitmap_put(tableau[0], tableau[1], get_ptr(get_entry(nam)), tableau[2]);
		}

		else if (strncmp(str, "map:", 4) == 0)
		{
			short           numero, rang, init[7] = { 999, 0, 0, 999, 999, 1, 999 };

			memcpy(tableau, init, 14);
			rang = arguments(str + 4, tableau, 7) - str;
			if (tableau[6] != 999)
				OSSetSR(0x0000);
			if (tableau[0] != 0)
			{
				i = rang + 1;
				while (str[i++] != ',') ;
				numero = str[i];
				str[i] = 0;
				do
				{
					str[i - 1] = numero;
					j = 0;
					while (str[i - 1] != 0)
					{
						nam = get_ptr(get_entry(str + rang));
						bitmap_put(tableau[1] + (j % tableau[0]) * (tableau[3] != 999 ? tableau[3] : *(nam + 5)),
								   tableau[2] + (j / tableau[0]) * (tableau[4] != 999 ? tableau[4] : *(nam + 3)), nam, tableau[5]);
						str[i - 1] = str[++j + i];
						if (tableau[6] != 999)
							attend(tableau[6]);
					}
				}
				while (tableau[6] != 999 && OSdequeue(&j, kbd_queue()) != FALSE);
				if (tableau[6] != 999)
					int_add(j);
			}
		}

		else if (strncmp(str, "savpic:", 7) == 0)
		{
			nam = arguments(str + 7, tableau, 4);
			if (tableau[0] <= tableau[2] && tableau[1] <= tableau[3])
			{
				rect = (SCR_RECT){{tableau[0], tableau[1], tableau[2], tableau[3]}};
				j = BitmapSize(&rect);
				nam = mk_ptr(nam, j + 3);
				if (nam != NULL)
				{
					BitmapGet(&rect, nam);
					nam[j] = 0xDF;
				}
			}
		}

		else if (strncmp(str, "mkpic:", 6) == 0)
		{
			char           *ptr, *limite;

			ptr = nam = arguments(str + 6, tableau, 2);
			while (*(ptr++) != ',') ;
			*(ptr - 1) = 0;
			j = ((tableau[0] + 7) >> 3) * tableau[1] + 7;
			nam = mk_ptr(nam, j);
			if (nam != NULL)
			{
				*(unsigned short *)nam = tableau[1];
				*(unsigned short *)(nam + 2) = tableau[0];
				limite = nam + j - 3;
				*limite = 0xDF;
				for (nam += 4; nam < limite; ptr++, nam++)
				{
					j = 256 ** ptr + *(ptr + 1);
					if (j == 12336)
					{
						*nam = 0;
						ptr++;
					}
					else if (j == 12338)
					{
						*nam = 2;
						ptr++;
					}
					else if (j == 12595)
					{
						*nam = 13;
						ptr++;
					}
					else
						*nam = *ptr;
				}
			}
		}

		else if (strncmp(str, "savscr:", 7) == 0)
		{
			nam = mk_ptr(str + 7, _89_92(2007, 3847));
			if (nam != NULL)
			{
				BitmapGet(&ECRAN, nam);
				nam[_89_92(2004, 3844)] = 0xDF;
			}
		}

		else if (strncmp(str, "rclscr:", 7) == 0)
		{
			bitmap_put(0, 0, get_ptr(get_entry(str + 7)), 4);
		}

		else if (strncmp(str, "picsize:", 8) == 0)
		{
			nam = get_ptr(get_entry(str + 8));
			int_add(nam != NULL ? *(unsigned short *)(nam + 4) : 0);
			int_add(nam != NULL ? *(unsigned short *)(nam + 2) : 0);
		}

		else if (strncmp(str, "rect:", 5) == 0)
		{
			tableau[4] = 1;
			arguments(str + 5, tableau, 5);
			DrawClipRect(MakeWinRect(tableau[0], tableau[1], tableau[2], tableau[3]), &ECRAN, tableau[4]);
		}

		else if (strncmp(str, "fillrect:", 9) == 0)
		{
			tableau[4] = 1;
			arguments(str + 9, tableau, 5);
			ScrRectFill(&(SCR_RECT){{tableau[0], tableau[1], tableau[2], tableau[3]}}, &ECRAN, tableau[4]);
		}

		else if (strncmp(str, "filltri:", 8) == 0)
		{
			tableau[6] = 1;
			arguments(str + 8, tableau, 7);
			FillTriangle(tableau[0], tableau[1], tableau[2], tableau[3], tableau[4], tableau[5], &ECRAN, tableau[6]);
		}

		else if (strncmp(str, "fillpoly:", 9) == 0)
		{
			WIN_RECT        wrect;

			tableau[8] = 1;
			arguments(str + 9, tableau, 9);
			wrect = (WIN_RECT){tableau[0], tableau[1], tableau[2], tableau[3]};
			FillLines2(&wrect, MakeWinRect(tableau[4], tableau[5], tableau[6], tableau[7]), &ECRAN, tableau[8]);
		}

		else if (strncmp(str, "ellipse:", 8) == 0)
		{
			tableau[4] = 1;
			arguments(str + 8, tableau, 5);
			DrawClipEllipse(tableau[0], tableau[1], tableau[2], tableau[3], &ECRAN, tableau[4]);
		}

		else if (strncmp(str, "line:", 5) == 0)
		{
			tableau[4] = 1;
			arguments(str + 5, tableau, 5);
			DrawClipLine(MakeWinRect(tableau[0], tableau[1], tableau[2], tableau[3]), &ECRAN, tableau[4]);
		}

		else if (strncmp(str, "pix:", 4) == 0)
		{
			tableau[2] = 1;
			arguments(str + 4, tableau, 3);
			if (tableau[0] < 240 && tableau[1] < 128)
				DrawPix(tableau[0], tableau[1], tableau[2]);
		}

		else if (strncmp(str, "pixtest:", 8) == 0)
		{
			arguments(str + 8, tableau, 2);
			int_add(GetPix(tableau[0], tableau[1]));
		}

/*---------------------
    FONCTIONS TEXTE
-----------------------*/

		else if (strncmp(str, "font:", 5) == 0)
			FontSetSys(str[5] - '0');

		else if (strncmp(str, "drawstr:", 8) == 0)
		{
			tableau[2] = 1;
			nam = arguments(str + 8, tableau, 3);
			if ((tableau[0] < 240 || tableau[0] == 999) && tableau[1] < 128)
				DrawStr(tableau[0] == 999 ? (_89_92(160, 240) - DrawStrWidth(nam, FontGetSys())) / 2 : tableau[0], tableau[1], nam,
						tableau[2]);
		}

		else if (strncmp(str, "drawstrv:", 9) == 0)
		{
			char           *ptr;

			tableau[2] = 1;
			i = FontGetSys();
			nam = ptr = arguments(str + 9, tableau, 3);
			while (*ptr != 0)
			{
				DrawClipChar(tableau[0], tableau[1] + (ptr - nam) * (i == 0 ? 6 : (i == 1 ? 9 : 11)), *ptr, &ECRAN, tableau[2]);
				ptr++;
			}
		}

		else if (strncmp(str, "width:", 6) == 0)
		{
			nam = arguments(str + 6, tableau, 1);
			int_add(DrawStrWidth(nam, tableau[0]));
		}

		else if (strncmp(str, "msg:", 4) == 0)
		{
			short           max = 0, rang = 1, numero = 4, xmin, ymin, debut[13];

			LCD_save(lcd);
			i = FontSetSys(1);
			for (debut[0] = 4; rang < _89_92(10, 14); numero++)
			{
				max = max(max, numero - debut[rang - 1]);
				if (numero - debut[rang - 1] > _89_92(25, 38))
				{
					str[debut[rang - 1] + _89_92(25, 38)] = 0;
					max = _89_92(25, 38);
				}
				if (str[numero] == 0)
					break;
				else if (str[numero] == '|')
				{
					str[numero] = 0;
					debut[rang++] = numero + 1;
				}
			}
			if (rang == _89_92(10, 14))
				rang--;
			ymin = _89_92(47, 61) - (9 * rang) / 2;
			xmin = (_89_92(160, 240) - 6 * max) / 2;
			rect = (SCR_RECT){{xmin - 5, ymin - 3, _89_92(165, 245) - xmin, _89_92(103, 131) - ymin}};
			ScrRectFill(&rect, &ECRAN, 0);
			rect = (SCR_RECT){{xmin - 4, ymin - 1, _89_92(164, 244) - xmin, _89_92(101, 129) - ymin}};
			ScrRectFill(&rect, &ECRAN, 1);
			DrawClipRect(MakeWinRect(xmin - 3, ymin - 2, _89_92(163, 243) - xmin, _89_92(102, 130) - ymin), &ECRAN, 1);
			DrawClipRect(MakeWinRect(xmin - 2, ymin, _89_92(162, 242) - xmin, _89_92(100, 128) - ymin), &ECRAN, 0);
			for (numero = 0; numero < rang; numero++)
				DrawStr((_89_92(160, 240) - DrawStrWidth((str + debut[numero]), 1)) / 2, ymin + 4 + 9 * numero, str + debut[numero], 2);

			FontSetSys(i);
			int_add(ngetchx());
			LCD_restore(lcd);
		}

		else if (strncmp(str, "pretty:", 7) == 0)
		{
			*(long *)tableau = 0;
			tableau[2] = 1;
			nam = arguments(str + 7, tableau, 3);
			DeskTop->Clip.xy.y0 = 0;
			DeskTop->Clip.xy.y1 = _89_92(99, 127);
			WinAttr(DeskTop, tableau[2]);
			Print2DExpr(parse_2d_expr(nam), DeskTop, tableau[0], tableau[1]);
		}

		else if (strncmp(str, "pinfo:", 6) == 0)
		{
			short           bas;
			Parms2D(parse_2d_expr(str + 6), &i, &j, &bas);
			int_add(i);
			int_add(j);
			int_add(bas);
		}

/*--------------------------------
    FONCTIONS DE LA STATUS LINE
----------------------------------*/

		else if (strncmp(str, "slmsg:", 6) == 0)
			ST_helpMsg(str + 6);

		else if (strncmp(str, "busy:", 5) == 0)
			ST_busy(str[5] - '0');

/*-----------------------
    FONCTIONS TOUCHES
-------------------------*/

		else if (strncmp(str, "initdelay:", 10) == 0)
		{
			arguments(str + 10, tableau, 1);
			int_add(OSInitKeyInitDelay(tableau[0]));
		}

		else if (strncmp(str, "delay:", 6) == 0)
		{
			arguments(str + 6, tableau, 1);
			int_add(OSInitBetweenKeyDelay(tableau[0]));
		}

	}

	if (mode_gray != -1)
	{
		if (mode_gray == 0)
			int_add(ngetchx());
		GrayMode(GRAY_OFF);
		LCD_restore(lcd);
	}

	nam = mk_ptr("fl", 1503 - Num);

	*nam = 0xE5;
	memcpy(nam + 1, Retour + Num + 1, 1499 - Num);
	nam[1500 - Num] = 0xD9;

	return;
}
