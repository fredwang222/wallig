/*
 * LIB_Graph_Text.c
 *
 *  Created on: 15 août 2010
 *      Author: luten
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
		LIB_Graph_Base_Draw_8bpp_Bmp(tCoord ,(Bitmap_t *) pFont->pGlyphe[ucGlyphIndex] , BGColor , FGColor);
	}
}

void LIB_Graph_Text_Draw_String( char *pcChar , Font_t *pFont ,Point_t tCoord , uint16_t BGColor , uint16_t FGColor)
{
	unsigned char ucGlyphIndex;

	while( *pcChar )
	{
		ucGlyphIndex = pFont->tucAscii2GlyphIndex[(unsigned char ) *pcChar];
		if( ucGlyphIndex != 0xff )
		{
			LIB_Graph_Text_Draw_Char(*pcChar,pFont,tCoord,BGColor,FGColor);
			tCoord.X +=( pFont->pGlyphe[ucGlyphIndex]->Width + 1);
		}
		pcChar++;
	}
}
