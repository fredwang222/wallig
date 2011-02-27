/*
 * DRV_Ms.c



 *
 *  Created on: 26 févr. 2011
 *      Author: cara
 */
#include <stdint.h>
#include "DRV_MS.h"
#include "DRV_Usb_CFG.h"
#include "DRV_MS_p.h"


tLUN_Data DRV_MS_MemDevice[kDRV_MS_MEMDDEVICEC_COUNT];

uint16_t DRV_Ms_Init( uint8_t lun , tDRV_MS_DeviceIO *ptInitParam )
{
	if( lun >= kDRV_MS_MEMDDEVICEC_COUNT )
		return 1;

	DRV_MS_MemDevice[lun].tIOAccess=*ptInitParam;
	DRV_MS_MemDevice[lun].Block_Count=0;
	DRV_MS_MemDevice[lun].Block_Size=0;
	DRV_MS_MemDevice[lun].Memory_Size=0;

	return ptInitParam->Init();

}

uint16_t DRV_MSp_UpdateStatus( uint8_t lun )
{
	DRV_MS_MemDevice[lun].tIOAccess.GetStatus( lun ,
												&DRV_MS_MemDevice[lun].Block_Count,
												&DRV_MS_MemDevice[lun].Block_Size,
												&DRV_MS_MemDevice[lun].Memory_Size);
	return 0;
}
