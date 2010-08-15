/*
 * LIB_Graph_Base.h
 *
 *  Created on: 15 août 2010
 *      Author: luten
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

void LIB_Graph_Base_Draw_8bpp_Bmp( Point_t tCoord , Bitmap_t *pBitmap , uint16_t BGColor , uint16_t FGColor );
void LIB_Graph_Base_Draw_Bmp( Point_t tCoord , Bitmap_t *pBitmap );
#endif /* LIB_GRAPH_BASE_H_ */
