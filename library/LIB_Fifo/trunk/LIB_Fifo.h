#ifndef LIB_FIFO_H
#define LIB_FIFO_H

typedef struct
{
	int iItemSize ;
	int iItemCount ;
	unsigned char *pucBuffer;
	int iFirstItem;
	int iNextFreeItem;
	char cFull;
	char cStringFifo;

} LIB_Fifo_Type;

void LIB_Fifo_Init( LIB_Fifo_Type *pFifo, int iItemSize ,  int iItemCount , unsigned char *pucBuffer , char cStringFifo );
void LIB_Fifo_Push( LIB_Fifo_Type *pFifo ,  unsigned char  *pucItem );
void LIB_Fifo_Pop( LIB_Fifo_Type *pFifo ,  unsigned char  *pucItem );
void LIB_Fifo_Get( LIB_Fifo_Type *pFifo ,  unsigned char  *pucItem , int iIndex);
int LIB_Fifo_ItemCount(LIB_Fifo_Type *pFifo);
int LIB_Fifo_Full( LIB_Fifo_Type *pFifo);
int LIB_Fifo_Empty( LIB_Fifo_Type *pFifo );

#endif
