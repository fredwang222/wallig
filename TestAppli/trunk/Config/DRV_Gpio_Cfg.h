#define GPIO_LEDA_INIT { "LEDA" , { GPIOB ,GPIO_Pin_8 } , { GPIO_Output , GPIO_OUPUT_PP } , { NULL , GPIO_INT_None  } }
#define GPIO_LEDB_INIT { "LEDB" , { GPIOB ,GPIO_Pin_7 } , { GPIO_Output , GPIO_OUPUT_PP } , { NULL , GPIO_INT_None  } }

#define DRV_GPIO_INIT_CFG { GPIO_LEDA_INIT , GPIO_LEDB_INIT }


