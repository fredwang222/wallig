/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
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
