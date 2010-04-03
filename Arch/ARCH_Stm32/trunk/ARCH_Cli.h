char ARCH_Cli_Reset( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define ARCH_CLI_RESET_COMMAND { "reset" , "Perform a soft reset" , ARCH_Cli_Reset , 0 }
