/*
 * LIB_Graph_Text.h
 *
 *  Created on: 15 août 2010
 *      Author: luten
 */

#ifndef LIB_GRAPH_TEXT_H_
#define LIB_GRAPH_TEXT_H_

typedef struct
{
	uint16_t Size;
	const unsigned char *tucAscii2GlyphIndex;
	const  Bitmap_t const *const*pGlyphe;
} Font_t;

void LIB_Graph_Text_Draw_Char(  char ucChar , Font_t *pFont ,Point_t tCoord , uint16_t BGColor , uint16_t FGColor);
void LIB_Graph_Text_Draw_String( char *pcChar , Font_t *pFont ,Point_t tCoord , uint16_t BGColor , uint16_t FGColor);

#endif /* LIB_GRAPH_TEXT_H_ */
