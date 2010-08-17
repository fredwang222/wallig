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
#include <stdlib.h>
#include "tft_lcd.h"
#include "LIB_Graph_Base.h"

#define GET_R(color) ( (((color)&0x1f)<<3)+3)
#define GET_G(color) (((((color)>>5)&0x3f)<<2)+1)
#define GET_B(color) (((((color)>>11)&0x1f)<<3)+3)
#define SET_RGB( R,G,B) (((R)>>3)|(((G)>>2)<<5)|(((B)>>3)<<11))


void Draw_Simple_Line( int x0 , int x1 , int y0 , int y1 , uint16_t Color );
void DrawPixel( short x ,short y , uint16_t Color );

void LIB_Graph_Base_Draw_Point( Point_t tPoint ,  uint16_t Color  )
{
	LCD_SetCursor(tPoint.X, tPoint.Y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(Color);
}

void LIB_Graph_Base_Draw_Alpha_Point( Point_t tPoint ,  uint16_t FGColor  ,  uint16_t BGColor ,  uint8_t ucAlpha)
{

	uint16_t ucRBg,ucGBg,ucBBg;
	uint16_t ucRFg,ucGFg,ucBFg;
	uint16_t uiRout,uiGout,uiBout;

	ucRBg = GET_R(BGColor);
	ucGBg = GET_G(BGColor);
	ucBBg = GET_B(BGColor);
	ucRFg = GET_R(FGColor);
	ucGFg = GET_G(FGColor);
	ucBFg = GET_B(FGColor);

	uiRout=((uint16_t)ucRBg*(uint16_t)ucAlpha+ucRFg*(uint16_t)(255-ucAlpha))>>8;
	uiGout=((uint16_t)ucGBg*(uint16_t)ucAlpha+ucGFg*(uint16_t)(255-ucAlpha))>>8;
	uiBout=((uint16_t)ucBBg*(uint16_t)ucAlpha+ucBFg*(uint16_t)(255-ucAlpha))>>8;
	LCD_SetCursor(tPoint.X, tPoint.Y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(SET_RGB(uiRout,uiGout,uiBout));
}


void LIB_Graph_Base_Draw_Line( Point_t tP1 , Point_t tP2 , uint16_t Color  )
{
	int iXIndex , iXMax, iXMin;
	int iYIndex , iYMax,iYMin;

	if( tP1.X == tP2.X )
	{
		if ( tP1.Y < tP2.Y )
		{
			LCD_SetCursor(tP1.X, tP1.Y);
			iYMax = tP2.Y;
			iYMin = tP1.Y;
		}
		else
		{
			LCD_SetCursor(tP2.X, tP2.Y);
			iYMax = tP1.Y;
			iYMin = tP2.Y;
		}
		for( iYIndex=iYMin;iYIndex<iYMax;iYIndex++)
		{
			DrawPixel(tP1.X, iYIndex,Color);
		}

	}
	else if( tP1.Y == tP2.Y )
	{
		if ( tP1.X < tP2.X )
		{
			LCD_SetCursor(tP1.X, tP1.Y);
			iXMax = tP2.X-tP1.X;
		}
		else
		{
			LCD_SetCursor(tP2.X, tP2.Y);
			iXMax = tP1.X-tP2.X;
		}
		LCD_WriteRAM_Prepare();
		for( iXIndex=0;iXIndex<iXMax;iXIndex++)
		{
			LCD_WriteRAM(Color);
		}
	} else
	{
		Draw_Simple_Line(tP1.X,tP2.X,tP1.Y,tP2.Y, Color);
		//Draw_Antialiased_Line(tP1.X,tP2.X,tP1.Y,tP2.Y, Color);

	}
}

void LIB_Graph_Base_Draw_Rectangle( Point_t tCoord ,uint16_t Width, uint16_t Height, uint16_t Color , char bFilled )
{
	int iXIndex;
	int iYIndex;

	if( bFilled )
	{
		LCD_SetCursor(tCoord.X, tCoord.Y);
		LCD_WriteRAM_Prepare();
		for( iYIndex=0;iYIndex<Height;iYIndex++)
		{
			for( iXIndex=0;iXIndex<Width;iXIndex++)
			{
				LCD_WriteRAM(Color);
			}
			LCD_SetCursor(tCoord.X, tCoord.Y+iYIndex);
			LCD_WriteRAM_Prepare();
		}
	}
}



void LIB_Graph_Base_Draw_RGB_Bmp( Point_t tCoord , Bitmap_t *pBitmap )
{
	int iXIndex;
	int iYIndex;

	uint16_t *pWriteBuffer;

	if(  pBitmap->tType == eColor_565 )
	{
		LCD_SetCursor(tCoord.X, tCoord.Y);
		LCD_WriteRAM_Prepare();
		pWriteBuffer = (uint16_t *) pBitmap->pucBuff;
		for( iYIndex=0;iYIndex<(pBitmap->Height);iYIndex++)
		{
			for( iXIndex=0;iXIndex<(pBitmap->Width);iXIndex++)
			{
				LCD_WriteRAM(*pWriteBuffer);	
				pWriteBuffer++;
			}
			LCD_SetCursor(tCoord.X, tCoord.Y+iYIndex);
			LCD_WriteRAM_Prepare();
		}
	}
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
}


void Draw_Simple_Line( int x0 , int x1 , int y0 , int y1 , uint16_t Color )
{
	Point_t tPoint;
	uint16_t Temp;
	int deltax,deltay ,ystep,y,x;
	int error ;
	char steep = abs(y1 - y0) > abs(x1 - x0);

	 if (steep)
	 {
		 Temp=x0;
		 x0 = y0;
		 y0 = Temp;
		 Temp=x1;
		 x1 = y1;
		 y1 = Temp;
	 }
	 if (x0 > x1 )
	 {
		 Temp=x0;
		 x0=x1;
		 x1=Temp;
		 Temp=y0;
		 y0=y1;
		 y1=Temp;

	 }
	 deltax = x1 - x0;
	 deltay = abs(y1 - y0);
	 error = deltax / 2 ;
	 y = y0;
	 if ( y0 < y1)
		 ystep = 1;
	 else
		 ystep = -1;
	 for ( x = x0 ; x <= x1 ; x++ )
	 {
		 if ( steep )
		 {
			 tPoint.X=y;
			 tPoint.Y=x;
		 }
		 else
		 {
			 tPoint.X=x;
			 tPoint.Y=y;

		 }
		 LIB_Graph_Base_Draw_Point(tPoint , Color );
		 error = error - deltay;
		 if ( error < 0  )
		 {
			 y = y + ystep;
			 error = error + deltax;
		 }
	 }

}

void LIB_Grpah_Draw_Circle( Point_t tCenter , uint16_t Radius , uint16_t Color)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;
	uint32_t  CurY;

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		DrawPixel(tCenter.X + CurX, tCenter.Y + CurY , Color );
		DrawPixel(tCenter.X + CurX, tCenter.Y - CurY , Color );
		DrawPixel(tCenter.X - CurX, tCenter.Y + CurY , Color );
		DrawPixel(tCenter.X - CurX, tCenter.Y - CurY , Color );
		DrawPixel(tCenter.X + CurY, tCenter.Y + CurX , Color );
		DrawPixel(tCenter.X + CurY, tCenter.Y - CurX , Color );
		DrawPixel(tCenter.X - CurY, tCenter.Y + CurX , Color );
		DrawPixel(tCenter.X - CurY, tCenter.Y - CurX , Color );
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}

}


void DrawPixel( short x ,short y , uint16_t Color )
{
	 Point_t tPoint = { x , y  };

	LIB_Graph_Base_Draw_Point(tPoint,Color);
}
