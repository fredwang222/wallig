/*
 * DRV_Usb_CFG.h
 *
 *  Created on: 6 févr. 2011
 *      Author: cara
 */

#ifndef DRV_USB_CFG_H_
#define DRV_USB_CFG_H_

  #define USB_DISCONNECT                      GPIOD
  #define USB_DISCONNECT_PIN                  GPIO_Pin_2
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOD
  #define EVAL_COM1_IRQn                      USART1_IRQn

#endif /* DRV_USB_CFG_H_ */
