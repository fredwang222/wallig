/*
 * DRV_MS.h
 *
 *  Created on: 26 févr. 2011
 *      Author: cara
 */

#ifndef DRV_MS_H_
#define DRV_MS_H_


typedef struct
{
	uint16_t (*Init)( void );
	uint16_t (*Write)(uint8_t , uint32_t , uint32_t *, uint16_t );
	uint16_t (*Read)(uint8_t , uint32_t , uint32_t *, uint16_t );
	uint16_t (*GetStatus) (uint8_t lun , uint32_t *, uint32_t *, uint32_t *);
} tDRV_MS_DeviceIO;


uint16_t DRV_Ms_Init( uint8_t lun , tDRV_MS_DeviceIO *ptInitParam );

#endif /* DRV_MS_H_ */
