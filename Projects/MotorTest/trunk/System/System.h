#ifdef ARCH_SAM7
#include "arch/ARCH_Sam7/ARCH_SAM7_def.h"
#endif

#ifdef ARCH_LINUX
#include "arch/ARCH_Linux/ARCH_Linux_def.h"
#endif

#ifdef ARCH_STM32
#include "stm32f10x.h"
#include "arch/ARCH_Stm32/ARCH_Stm32_system.h"
#endif

void System_init( void );
