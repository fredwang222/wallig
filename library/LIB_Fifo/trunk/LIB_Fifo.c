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
#include <string.h>
#include "LIB_Fifo.h"

void LIB_Fifo_Init( LIB_Fifo_Type *pFifo, int iItemSize ,  int iItemCount , unsigned char *pucBuffer , char cStringFifo)
{
	pFifo->iItemSize=iItemSize;
	pFifo->iItemCount=iItemCount;
	pFifo->pucBuffer=pucBuffer;
	pFifo->iFirstItem=0;
	pFifo->iNextFreeItem=0;
	pFifo->cFull=0;
	pFifo->cStringFifo = cStringFifo;
}
void LIB_Fifo_Push( LIB_Fifo_Type *pFifo , unsigned char  *pucItem )
{
	if(pucItem == NULL )
		return;
	if( pFifo->cFull )
		return;
	//Copy data to buffer
	switch(pFifo->iItemSize )
	{
		case sizeof(char):
			 pFifo->pucBuffer[pFifo->iNextFreeItem]=*pucItem;
			break;
		case sizeof(short):
			((short*)pFifo->pucBuffer)[pFifo->iNextFreeItem]=*((short*)pucItem);
			break;
		case sizeof(long):
			((long*)pFifo->pucBuffer)[pFifo->iNextFreeItem]=*((long*)pucItem);
			break;
		default:
			if(pFifo->cStringFifo )
			{
				strncpy( (char*)&pFifo->pucBuffer[pFifo->iNextFreeItem*pFifo->iItemSize],(char*)pucItem , pFifo->iItemSize);
				(pFifo->pucBuffer +pFifo->iNextFreeItem*pFifo->iItemSize)[pFifo->iItemSize-1]=0;
			}
			else
				memcpy( pFifo->pucBuffer +pFifo->iNextFreeItem*pFifo->iItemSize, pucItem , pFifo->iItemSize);
			break;
	}

	//Update next index
	pFifo->iNextFreeItem++;
	if( pFifo->iNextFreeItem >= pFifo->iItemCount )
		pFifo->iNextFreeItem=0;
	if( pFifo->iNextFreeItem == pFifo->iFirstItem )
		pFifo->cFull = 1;
}
void LIB_Fifo_Pop( LIB_Fifo_Type *pFifo ,  unsigned char *pucItem )
{
	if(pucItem == NULL )
		return;
	//Copy data to buffer
	switch(pFifo->iItemSize )
	{
		case sizeof(char):
			*pucItem = pFifo->pucBuffer[pFifo->iFirstItem];
			break;
		case sizeof(short):
			*((short*)pucItem) = ((short*)pFifo->pucBuffer)[pFifo->iFirstItem];
			break;
		case sizeof(long):
			*((long*)pucItem) = ((long*)pFifo->pucBuffer)[pFifo->iFirstItem];
			break;
		default:
			if(pFifo->cStringFifo )
			{
				strncpy((char*) pucItem , (char*) &pFifo->pucBuffer[pFifo->iFirstItem*pFifo->iItemSize], pFifo->iItemSize);
				pucItem[pFifo->iItemSize-1] = 0;
			}
			else
				memcpy( pucItem , &pFifo->pucBuffer[pFifo->iFirstItem*pFifo->iItemSize], pFifo->iItemSize);
			break;
	}
	if( pFifo->iNextFreeItem == pFifo->iFirstItem )
	{
		if( pFifo->cFull)
			pFifo->cFull=0;
		else
			return; //The fifo is empty
	}
	pFifo->iFirstItem++;
	if( pFifo->iFirstItem >= pFifo->iItemCount )
		pFifo->iFirstItem=0;

}

void LIB_Fifo_Get( LIB_Fifo_Type *pFifo ,  unsigned char  *pucItem , int iIndex)
{
	if(pucItem == NULL )
		return;

	iIndex += pFifo->iFirstItem;
	if(iIndex>= pFifo->iItemCount)
		iIndex-=pFifo->iItemCount;
	//Copy data to buffer
	switch(pFifo->iItemSize )
	{
		case sizeof(char):
			*pucItem = pFifo->pucBuffer[iIndex];
			break;
		case sizeof(short):
			*((short*)pucItem) = ((short*)pFifo->pucBuffer)[iIndex];
			break;
		case sizeof(long):
			*((long*)pucItem) = ((long*)pFifo->pucBuffer)[iIndex];
			break;
		default:
			if(pFifo->cStringFifo )
			{
				strncpy(pucItem ,  pFifo->pucBuffer +iIndex*pFifo->iItemSize, pFifo->iItemSize);
				pucItem[pFifo->iItemSize-1] = 0;
			}
			else
				memcpy( pucItem ,  pFifo->pucBuffer +iIndex*pFifo->iItemSize, pFifo->iItemSize);
			break;
	}
}

int LIB_Fifo_ItemCount(LIB_Fifo_Type *pFifo)
{
	if( pFifo->iNextFreeItem == pFifo->iFirstItem )
	{
		if( pFifo->cFull)
			return pFifo->iItemCount;
		else
			return 0;
	}
	else if( pFifo->iNextFreeItem > pFifo->iFirstItem )
		return pFifo->iNextFreeItem - pFifo->iFirstItem ;
	else
		return pFifo->iNextFreeItem  + (pFifo->iItemCount -  pFifo->iFirstItem );
}

int LIB_Fifo_Full( LIB_Fifo_Type *pFifo )
{
	return pFifo->cFull;
}

int LIB_Fifo_Empty( LIB_Fifo_Type *pFifo )
{
	return !pFifo->cFull && (pFifo->iNextFreeItem == pFifo->iFirstItem);
}
