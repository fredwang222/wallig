#ifdef ARCH_STM32
#include "stm32f10x.h"

typedef GPIO_TypeDef* tPIO_BANK;
typedef u16 tPIO_Pin;

#define  GPIO_INPUT (int) GPIO_Mode_IN_FLOATING
#define  GPIO_OUPUT_OD (int) GPIO_Mode_Out_OD
#define  GPIO_OUPUT_PP (int) GPIO_Mode_Out_PP
#endif
