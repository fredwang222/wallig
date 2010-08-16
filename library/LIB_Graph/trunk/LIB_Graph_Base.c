 /*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 * LIB_Graph_Base.c
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
#include "tft_lcd.h"
#include "LIB_Graph_Base.h"

#define GET_R(color) ((color)&0x1f)
#define GET_G(color) (((color)>>5)&0x3f)
#define GET_B(color) (((color)>>11)&0x1f)
#define SET_RGB( R,G,B) ((R)|((G)<<5)|((B)<<11))


void LIB_Graph_Base_Draw_RGB_Bmp( Point_t tCoord , Bitmap_t *pBitmap )
{
	int iIndex;
	uint16_t *pWriteBuffer;

	if(  pBitmap->tType == eColor_565 )
	{
		//LCD_SetCursor(0, 0);
		LCD_SetDisplayWindow(tCoord.X,tCoord.Y,pBitmap->Height,pBitmap->Width);
		LCD_WriteRAM_Prepare();


		pWriteBuffer = (uint16_t *) pBitmap->pucBuff;
		for( iIndex=0;iIndex<(pBitmap->Height*pBitmap->Width);iIndex++,pWriteBuffer++)
		{
			LCD_WriteRAM(*pWriteBuffer);
		}
	}
	LCD_WindowModeDisable();

}

void LIB_Graph_Base_Draw_Mono_Bmp( Point_t tCoord , Bitmap_t *pBitmap , uint16_t BGColor , uint16_t FGColor )
{
	int iXIndex;
	int iYIndex;
	unsigned char *pWriteBuffer;

	LCD_SetCursor(tCoord.X, tCoord.Y);
	LCD_WriteRAM_Prepare();
	pWriteBuffer = (unsigned char *) pBitmap->pucBuff;


	if(  pBitmap->tType == eColor_8bpp)
	{
		uint16_t ucRBg,ucGBg,ucBBg;
		uint16_t ucRFg,ucGFg,ucBFg;
		uint16_t uiRout,uiGout,uiBout;
		uint16_t uiAlpha;

		ucRBg = GET_R(BGColor);
		ucGBg = GET_G(BGColor);
		ucBBg = GET_B(BGColor);
		ucRFg = GET_R(FGColor);
		ucGFg = GET_G(FGColor);
		ucBFg = GET_B(FGColor);

		for( iYIndex=0;iYIndex<(pBitmap->Height);iYIndex++)
		{
			for( iXIndex=0;iXIndex<(pBitmap->Width);iXIndex++)
			{
				uiAlpha = *pWriteBuffer;
				uiRout=(ucRBg*uiAlpha+ucRFg*(255-uiAlpha))>>8;
				uiGout=(ucGBg*uiAlpha+ucGFg*(255-uiAlpha))>>8;
				uiBout=(ucBBg*uiAlpha+ucBFg*(255-uiAlpha))>>8;
				LCD_WriteRAM(SET_RGB(uiRout,uiGout,uiBout));

				pWriteBuffer++;
			}
			LCD_SetCursor(tCoord.X, tCoord.Y+iYIndex);
			LCD_WriteRAM_Prepare();
		}
	}
	else if(  pBitmap->tType == eColor_1bpp)
	{
		unsigned char ucBitIndex,ucTocalBitCount , ucByte;

		for( iYIndex=0;iYIndex<(pBitmap->Height);iYIndex++)
		{
			ucTocalBitCount=0;
			for( iXIndex=0;iXIndex<((pBitmap->Width-1)/8)+1;iXIndex++)
			{
				ucByte=*pWriteBuffer++;
				for( ucBitIndex=0;ucBitIndex< 8 && ucTocalBitCount<pBitmap->Width;ucBitIndex++,ucTocalBitCount++,ucByte>>=1)
				{
					if( ucByte & 0x01 )
						LCD_WriteRAM(FGColor);
					else
						LCD_WriteRAM(BGColor);
				}

			}
			LCD_SetCursor(tCoord.X, tCoord.Y+iYIndex);
			LCD_WriteRAM_Prepare();
		}

	}
//	LCD_WindowModeDisable();
}
