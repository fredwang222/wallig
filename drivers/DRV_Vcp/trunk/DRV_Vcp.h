/*
 * LIB_Vcp.h
 *
 *  Created on: 5 févr. 2011
 *      Author: cara
 */

#ifndef DRV_VCP_H_
#define DRV_VCP_H_

void DRV_Vcp_CallbackRegister( void (*callback)(unsigned char *, unsigned int));
unsigned int DRV_Vcp_SendBuffer( unsigned char *, unsigned int );

#endif /* LIB_VCP_H_ */
