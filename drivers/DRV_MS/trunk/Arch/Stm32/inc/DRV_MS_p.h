/*
 * DRV_MS_p.h
 *
 *  Created on: 26 févr. 2011
 *      Author: cara
 */

#ifndef DRV_MS_P_H_
#define DRV_MS_P_H_

typedef struct
{
	tDRV_MS_DeviceIO tIOAccess;
	uint32_t Memory_Size;
	uint32_t Block_Size;
	uint32_t Block_Count;

} tLUN_Data;

extern tLUN_Data DRV_MS_MemDevice[kDRV_MS_MEMDDEVICEC_COUNT];

uint16_t DRV_MSp_UpdateStatus( uint8_t );

#endif /* DRV_MS_P_H_ */
