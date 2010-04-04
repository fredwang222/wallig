char DRV_Timer_Cli_uptime( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define DRV_TIMER_CLI_UPTIME_COMMAND { "uptime" , "Get uptime" , DRV_Timer_Cli_uptime , 0 }
