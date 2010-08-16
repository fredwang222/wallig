 /*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    LIB_Graph_Touch.c
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
#ifndef LIB_GRAPH_TOUCH_H_
#define LIB_GRAPH_TOUCH_H_

typedef enum
{
	eEvent_none,
	eEvent_Pressed,
	eEvent_Releasd,
	eEvent_Click
} TouchEventType_t;

typedef struct
{
	TouchEventType_t eType;
	uint16_t         usXpos;
	uint16_t         usYpos;

} TouchEvent_t;


void LIB_Graph_Touch_Init( void );
unsigned int LIB_Graph_Calibrate( unsigned char *pucState );
void LIB_Graph_Get_Event( TouchEvent_t *ptEvent );
void LIB_Graph_Event_Thread( void );

#endif /* LIB_GRAPH_TOUCH_H_ */
