/*
 * LIB_Graph_Base.c
 *
 *  Created on: 15 août 2010
 *      Author: luten
 */
#include <stdint.h>
#include "tft_lcd.h"
#include "LIB_Graph_Base.h"

#define GET_R(color) ((color)&0x1f)
#define GET_G(color) (((color)>>5)&0x3f)
#define GET_B(color) (((color)>>11)&0x1f)
#define SET_RGB( R,G,B) ((R)|((G)<<5)|((B)<<11))
void LIB_Graph_Base_Draw_Bmp( Point_t tCoord , Bitmap_t *pBitmap )
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

void LIB_Graph_Base_Draw_8bpp_Bmp( Point_t tCoord , Bitmap_t *pBitmap , uint16_t BGColor , uint16_t FGColor )
{
	int iXIndex;
	int iYIndex;
	unsigned char *pWriteBuffer;
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

	if(  pBitmap->tType == eColor_8bpp)
	{
		LCD_SetCursor(tCoord.X, tCoord.Y);
		//LCD_SetDisplayWindow(tCoord.X,tCoord.Y,pBitmap->Height,pBitmap->Width);
		LCD_WriteRAM_Prepare();
		pWriteBuffer = (unsigned char *) pBitmap->pucBuff;
		for( iYIndex=0;iYIndex<(pBitmap->Height);iYIndex++)
		{
			for( iXIndex=0;iXIndex<(pBitmap->Width);iXIndex++)
			{
				/*if( *pWriteBuffer > 0x80 )
					LCD_WriteRAM(BGColor);
				else
					LCD_WriteRAM(FGColor);*/
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
//	LCD_WindowModeDisable();
}
