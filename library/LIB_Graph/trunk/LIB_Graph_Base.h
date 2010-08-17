 /*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    LIB_Graph_Base.h
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
#ifndef LIB_GRAPH_BASE_H_
#define LIB_GRAPH_BASE_H_

typedef struct
{
	uint16_t X;
	uint16_t Y;
} Point_t;

typedef enum
{
	eColor_1bpp,
	eColor_8bpp,
	eColor_565,
	eColor_888,
} ColorType_t;

typedef struct
{
	ColorType_t tType;
	uint16_t Width;
	uint16_t Height;
	unsigned char *pucBuff;
} Bitmap_t;

void LIB_Graph_Base_Draw_Point( Point_t tPoint ,  uint16_t Color  );
void LIB_Graph_Base_Draw_Line( Point_t tP1 , Point_t tP2 , uint16_t Color  );
void LIB_Graph_Base_Draw_Rectangle( Point_t tCoord ,uint16_t Width, uint16_t Height, uint16_t Color , char bFilled );
void LIB_Grpah_Draw_Circle( Point_t tCenter , uint16_t Radius , uint16_t Color);
void LIB_Graph_Base_Draw_Mono_Bmp( Point_t tCoord , Bitmap_t *pBitmap , uint16_t BGColor , uint16_t FGColor );
void LIB_Graph_Base_Draw_RGB_Bmp( Point_t tCoord , Bitmap_t *pBitmap );
#endif /* LIB_GRAPH_BASE_H_ */
