/*
 * DRV_Spi_CFG.h
 *
 *  Created on: 27 févr. 2011
 *      Author: cara
 */

#ifndef DRV_SPI_CFG_H_
#define DRV_SPI_CFG_H_

#define SPISD_INIT_CFG {"SPISD" , \
						SPI1 ,  \
						{ SPI_BaudRatePrescaler_256 , SPI_BaudRatePrescaler_4 }, \
						{ RCC_APB2PeriphClockCmd , RCC_APB2Periph_SPI1  } ,\
						{ GPIOA , GPIO_Pin_5 , GPIO_Pin_6 , GPIO_Pin_7 } , \
						{ { DMA1_Channel2 , DMA1_FLAG_TC2 } , { DMA1_Channel3 , DMA1_FLAG_TC3 } } }
#define SPI_INIT_CFG { SPISD_INIT_CFG }

#endif /* DRV_SPI_CFG_H_ */
