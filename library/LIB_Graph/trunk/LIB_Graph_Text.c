 /*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    LIB_Graph_Text.c
 *
 *    This file is part of Wallig Library and Drivers.
 *
 *    Copyright (C) 2010  Gwendal Le Gall
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdint.h>
#include <LIB_Graph_Base.h>
#include <LIB_Graph_Text.h>
#include <Fonts/LIB_Graph_Fonts.h>


void LIB_Graph_Text_Draw_Char( char ucChar , Font_t *pFont ,Point_t tCoord , uint16_t BGColor , uint16_t FGColor)
{
	unsigned char ucGlyphIndex;

	ucGlyphIndex = pFont->tucAscii2GlyphIndex[(unsigned char )ucChar];
	if( ucGlyphIndex != 0xff )
	{
		LIB_Graph_Base_Draw_Mono_Bmp(tCoord ,(Bitmap_t *) pFont->pGlyphe[ucGlyphIndex] , BGColor , FGColor);
	}
}

void LIB_Graph_Text_Draw_String( char *pcChar ,  String_Attribut_t *pAttr )
{
	unsigned char ucGlyphIndex;
	Point_t tNewCoord = pAttr->tCoord;
	uint16_t uiXsize=LIb_Graph_Get_String_XSize(pcChar,pAttr);

	switch( pAttr->Just)
	{
		default:
		case eLeft:
			break;
		case eCenter:
			tNewCoord.X -=uiXsize/2;
			break;
		case eRight:
			tNewCoord.X -=uiXsize;
			break;

	}

	while( *pcChar )
	{
		ucGlyphIndex = pAttr->pFont->tucAscii2GlyphIndex[(unsigned char ) *pcChar];
		if( ucGlyphIndex != 0xff )
		{
			LIB_Graph_Text_Draw_Char(*pcChar,pAttr->pFont,tNewCoord,pAttr->Color.BG,pAttr->Color.FG);
			tNewCoord.X +=( pAttr->pFont->pGlyphe[ucGlyphIndex]->Width + 1);
		}
		pcChar++;
	}
}

uint16_t LIb_Graph_Get_String_XSize( char *pcChar ,  String_Attribut_t *pAttr  )
{
	uint16_t uiXsize=0;
	unsigned char ucGlyphIndex;

	while( *pcChar )
	{
		ucGlyphIndex = pAttr->pFont->tucAscii2GlyphIndex[(unsigned char ) *pcChar];
		if( ucGlyphIndex != 0xff )
		{
			uiXsize+=( pAttr->pFont->pGlyphe[ucGlyphIndex]->Width + 1);
		}
		pcChar++;
	}

	return uiXsize;
}
