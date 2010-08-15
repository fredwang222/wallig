/*
 * LIB_GRAPH_Touc.h
 *
 *  Created on: 15 août 2010
 *      Author: luten
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
