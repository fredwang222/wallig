/*
 * DRV_Vcp_p.h
 *
 *  Created on: 5 févr. 2011
 *      Author: cara
 */

#ifndef DRV_VCP_P_H_
#define DRV_VCP_P_H_

typedef struct
{
	void (*Rxcallback)(uint8_t *, uint8_t);
	void (*Txcallback)(void);
} tDRV_Vcp_Data;

extern tDRV_Vcp_Data DRV_Vcp_Data;

void DRV_Vcp_Init( void );

#endif /* DRV_VCP_P_H_ */
