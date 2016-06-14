/*
   Gamebox
    Copyright (C) 2004-2006  by Malte Marwedel
    m.marwedel AT onlinehome dot de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* Diese Funktion schreibt einzelne Buchstaben auf das LED Display
Die Schriftgr��e betr�gt 5x8 Pixel, wobei nur 5x7 auch gezeichnet werden.
Jeder Buchstabe ben�tigt 5 Byte Speicher
Insgesammt werden ungef�hr 465 Byte f�r die Buchstaben verbaucht
Der Funktion kann zus�tzlich die Font Farbe und Transparenz als
Parameter �bergeben werden
Die Schrift basiert auf einer modifizierten Schrift von:
http://overcode.yak.net/12

Zus�tzlich gibt es Funktionen zum anzeigen von Texten und Scrolltexten

Date of last changes:
  2010-05-15: Prevent overflow of chars to the left side
  2010-02-10: Add german umlauts, fix & and ~ sign, translate some comments
  2009-09-10: Use SCREENPOS datatype to support 16 bit screen resolution
  2009-05-24: Changed in black/white char renderer
  2005-07-19: Buchstabe "P" war falsch
  2005-07-23: draw_tinynumber(), draw_tinydigit() hinzugefuegt

*/

#include "menu-interpreter.h"
#include <string.h>

//make pc compatible
#ifndef AVR_BUILD
  #define STOREINRAM
  #define PROGMEM
#else
  #include <avr/pgmspace.h>
#endif

//comment out to get the old behaviour (saves code + cpu time)
#define ENABLE_GERMAN_UMLAUTS

#ifdef ENABLE_GERMAN_UMLAUTS
#define MENU_CHARACTERS 100
#else
#define MENU_CHARACTERS 95
#endif

const char characters[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00, //Space
0x00,0x00,0x5F,0x00,0x00, //Ausrufezeichen
0x00,0x03,0x00,0x03,0x00,
0x14,0x7F,0x14,0x7F,0x14,
0x24,0x2A,0x7F,0x2A,0x12,
0x26,0x16,0x08,0x34,0x32,
0x36,0x49,0x55,0x22,0x50, //&
0x00,0x00,0x03,0x00,0x00,
0x00,0x1C,0x22,0x41,0x00,
0x00,0x41,0x22,0x1C,0x00,
0x22,0x14,0x7F,0x14,0x22,
0x08,0x08,0x3E,0x08,0x08,
0x00,0x00,0x60,0x30,0x00,
0x08,0x08,0x08,0x08,0x08,
0x00,0x00,0x60,0x60,0x00,
0x20,0x10,0x08,0x04,0x02,
0x3E,0x41,0x41,0x41,0x3E,
0x00,0x42,0x7F,0x40,0x00,
0x42,0x61,0x51,0x49,0x46,
0x22,0x41,0x49,0x49,0x36,
0x0C,0x0A,0x49,0x7F,0x48,
0x2F,0x49,0x49,0x49,0x31,
0x3E,0x49,0x49,0x49,0x32,
0x01,0x71,0x09,0x05,0x03,
0x36,0x49,0x49,0x49,0x36,
0x26,0x49,0x49,0x49,0x3E,
0x00,0x00,0x36,0x36,0x00,
0x00,0x00,0x56,0x36,0x00,
0x08,0x14,0x22,0x41,0x00,
0x14,0x14,0x14,0x14,0x14,
0x41,0x22,0x14,0x08,0x00,
0x02,0x01,0x51,0x09,0x06,
0x3E,0x41,0x5D,0x55,0x1E,
0x7E,0x09,0x09,0x09,0x7E, //A
0x7F,0x49,0x49,0x49,0x36,
0x3E,0x41,0x41,0x41,0x22,
0x7F,0x41,0x41,0x41,0x3E,
0x7F,0x49,0x49,0x49,0x41,
0x7F,0x09,0x09,0x09,0x09, //F
0x3E,0x41,0x49,0x49,0x3A,
0x7F,0x08,0x08,0x08,0x7F, //H
0x00,0x41,0x7F,0x41,0x00, //I
0x30,0x40,0x41,0x3F,0x01,
0x7F,0x08,0x08,0x14,0x63,
0x00,0x7F,0x40,0x40,0x40,
0x7F,0x04,0x08,0x04,0x7F,
0x7F,0x02,0x0C,0x10,0x7F,
0x3E,0x41,0x41,0x41,0x3E,
0x7F,0x09,0x09,0x09,0x06, //P
0x3E,0x41,0x51,0x61,0x7E,
0x7F,0x09,0x19,0x29,0x46,
0x26,0x49,0x49,0x49,0x32,
0x01,0x01,0x7F,0x01,0x01,
0x3F,0x40,0x40,0x40,0x3F,
0x07,0x18,0x60,0x18,0x07,
0x1F,0x60,0x18,0x60,0x1F,
0x63,0x14,0x08,0x14,0x63,
0x03,0x04,0x78,0x04,0x03,
0x61,0x51,0x49,0x45,0x43,
0x00,0x7F,0x41,0x41,0x00,
0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x7F,0x00,
0x04,0x02,0x01,0x02,0x04,
0x40,0x40,0x40,0x40,0x40,
0x00,0x01,0x02,0x00,0x00,
0x30,0x4A,0x4A,0x4A,0x7C,
0x00,0x7F,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x00,
0x00,0x38,0x44,0x44,0x7F,
0x38,0x54,0x54,0x54,0x18,
0x08,0x7E,0x09,0x01,0x00,
0x0C,0x52,0x52,0x52,0x3C,
0x7F,0x04,0x04,0x04,0x78,
0x00,0x00,0x7A,0x00,0x00,
0x20,0x40,0x3A,0x00,0x00,
0x7F,0x10,0x28,0x44,0x00,
0x00,0x00,0x7F,0x00,0x00,
0x7C,0x04,0x18,0x04,0x78,
0x00,0x78,0x04,0x04,0x78,
0x38,0x44,0x44,0x44,0x38,
0x7C,0x14,0x24,0x24,0x18,
0x18,0x24,0x24,0x14,0x7C,
0x7C,0x08,0x04,0x04,0x00,
0x48,0x54,0x54,0x54,0x24,
0x00,0x02,0x3F,0x42,0x00,
0x3C,0x40,0x40,0x40,0x3C,
0x1C,0x20,0x40,0x20,0x1C,
0x3C,0x40,0x30,0x40,0x3C,
0x44,0x28,0x10,0x28,0x44,
0x0C,0x50,0x50,0x50,0x3C,
0x44,0x64,0x54,0x4C,0x00,
0x08,0x36,0x41,0x41,0x00,
0x00,0x00,0x7F,0x00,0x00,
0x41,0x41,0x36,0x08,0x00,
0x08,0x04,0x08,0x10,0x08 //~
#ifdef ENABLE_GERMAN_UMLAUTS
,0x79,0x14,0x12,0x14,0x79, //A with dots
0x39,0x44,0x44,0x44,0x39, //O with dots
0x3D,0x40,0x40,0x40,0x3D, //U with dots
0x31,0x48,0x48,0x48,0x7D, //a with dots (o and u with dots are the same on a 5x7 font)
0x7E,0x01,0x49,0x49,0x36, //sz (spoken as 'sharp s')
#endif
};

unsigned char menu_font_heigth(unsigned char font) {
	if (font < 2)
		return 7; //normal font
	return 8; //font with undlerline
}


unsigned char menu_char_draw(SCREENPOS posx, SCREENPOS posy, unsigned char font, unsigned char cdraw) {
	unsigned char nunbyte, charwidth, nunbit;
	unsigned char copyedbytes[5];
	SCREENPOS tempx, tempy;
	unsigned char byte_eq_count,nun;
	unsigned char shrink;
	if ((font & 1)) {
		shrink = 0;
	} else
		shrink = 1;
	charwidth = 0;
#ifdef ENABLE_GERMAN_UMLAUTS
	if (cdraw == 196) cdraw = 127;
	if ((cdraw == 214) || (cdraw == 246)) cdraw = 128;
	if ((cdraw == 220) || (cdraw == 252)) cdraw = 129;
	if (cdraw == 228) cdraw = 130;
	if (cdraw == 223) cdraw = 131;
#endif
	cdraw -= 32;
	if (cdraw < MENU_CHARACTERS) { //valid character
#ifdef STOREINRAM
		memcpy(copyedbytes,characters+cdraw*5,5);
#else
		memcpy_P(copyedbytes,characters+cdraw*5,5);
#endif
		//automated shortening of chars
		if (shrink != 0) {
			byte_eq_count = 0;
			for (nun = 0; nun < 4; nun++) {
				byte_eq_count++;
				if (copyedbytes[nun] != copyedbytes[nun+1]) {
					byte_eq_count = 0;
				}
				if (byte_eq_count == 2) {
					//remove one byte, if it repeated three times
					copyedbytes[nun] = 0;
				}
				if (byte_eq_count == 4) {
					//if there are 5 bytes equal, we remove a second one
					copyedbytes[nun] = 0;
				}
			} //end of loop
		} //end if shrink != 0
		for (nunbyte = 0; nunbyte < 5;nunbyte++) {
			tempx = posx+charwidth;
			if (tempx < posx+charwidth) {
				break; //prevent overflow to the left side of the screen
			}
			for (nunbit = 0; nunbit < 7;nunbit++) {
				tempy = posy+nunbit;
				if ((copyedbytes[nunbyte] & (0x01<<nunbit)) != 0) {
					menu_screen_set(tempx, tempy, 1);
				} else
					menu_screen_set(tempx, tempy, 0);
			}       //end: inner loop
			if (font & 2)
				menu_screen_set(tempx, posy+7, 1);
			if ((copyedbytes[nunbyte] != 0) || (shrink == 0)) {
				charwidth++;
			}       //end: charwidth++
		}         //end: outer loop
		if (font & 2) { //underline empty part between chars (this is not perfect on word ending)
			//printf("%i, %i\n", posx, charwidth);
			menu_screen_set(posx+charwidth, posy+7, 1);
		}
	}          //end: valid char
	return charwidth++;
}          //end: function