/*
 * DRV_USB.h
 *
 *  Created on: 5 févr. 2011
 *      Author: cara
 */

#ifndef DRV_USB_H_
#define DRV_USB_H_

uint32_t DRV_USB_Init(void);
uint32_t DRV_USB_Write(uint8_t bEpAddr, uint8_t* pBufferPointer, uint32_t wBufferSize);
uint32_t DRV_USB_Read(uint8_t bEpAddr, uint8_t* pBufferPointer);

#endif /* DRV_USB_H_ */
