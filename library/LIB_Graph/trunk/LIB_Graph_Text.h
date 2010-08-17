 /*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    LIB_Graph_Text.h
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
#ifndef LIB_GRAPH_TEXT_H_
#define LIB_GRAPH_TEXT_H_

typedef struct
{
	uint16_t Size;
	const unsigned char *tucAscii2GlyphIndex;
	const  Bitmap_t const *const*pGlyphe;
} Font_t;

typedef enum
{
	eLeft,
	eRight,
	eCenter

} Justification_t;

typedef struct
{
	Font_t *pFont;
	Point_t tCoord;
	struct 
	{
		uint16_t FG;
		uint16_t BG;
	} Color;
	Justification_t Just;

} String_Attribut_t;

void LIB_Graph_Text_Draw_Char(  char ucChar , Font_t *pFont ,Point_t tCoord , uint16_t BGColor , uint16_t FGColor);
void LIB_Graph_Text_Draw_String( char *pcChar , String_Attribut_t *pAttr);
uint16_t LIb_Graph_Get_String_XSize( char *pcChar ,  String_Attribut_t *pAttr  );
#endif /* LIB_GRAPH_TEXT_H_ */
