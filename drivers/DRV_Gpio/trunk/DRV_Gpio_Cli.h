char DRV_Gpio_Cli_open( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_OPEN_COMMAND { "pio_open" , "Open a pio device" , DRV_Gpio_Cli_open , 0 }

char DRV_Gpio_Cli_Set( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_SET_COMMAND { "pio_set" , "Set a pio Value: 0 or 1" , DRV_Gpio_Cli_Set , 0 }

char DRV_Gpio_Cli_Get( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_GET_COMMAND { "pio_get" , "Get pio Value: 0 or 1" , DRV_Gpio_Cli_Get , 0 }

#define GPIO_COMMAND_LIST GPIO_CLI_OPEN_COMMAND , GPIO_CLI_SET_COMMAND , GPIO_CLI_GET_COMMAND
