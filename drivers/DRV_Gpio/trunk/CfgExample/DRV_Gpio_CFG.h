/*
 * DRV_Gpio_CFG.h
 *
 *  Created on: 27 févr. 2011
 *      Author: cara
 */

#ifndef DRV_GPIO_CFG_H_
#define DRV_GPIO_CFG_H_

#define GPIO_WKUP_INIT  { "WKUP" ,  { GPIOA ,GPIO_Pin_0 } ,  GPIO_Speed_10MHz , GPIO_INPUT    , GPIO_PortSourceGPIOA }
#define GPIO_SDCS_INIT  { "SD_CS" , { GPIOA ,GPIO_Pin_4 } ,  GPIO_Speed_10MHz , GPIO_OUPUT_PP , GPIO_PortSourceGPIOA }
 #define GPIO_SDWP_INIT { "SD_WP" , { GPIOA ,GPIO_Pin_0 } ,  GPIO_Speed_10MHz , GPIO_INPUT    , GPIO_PortSourceGPIOA }
#define GPIO_SDCCD_INIT { "SD_CD" , { GPIOA ,GPIO_Pin_8 } ,  GPIO_Speed_10MHz , GPIO_INPUT    , GPIO_PortSourceGPIOA }
#define DRV_GPIO_INIT_CFG { GPIO_WKUP_INIT , \
							GPIO_SDCS_INIT , \
							GPIO_SDWP_INIT , \
							GPIO_SDCCD_INIT}


#endif /* DRV_GPIO_CFG_H_ */
