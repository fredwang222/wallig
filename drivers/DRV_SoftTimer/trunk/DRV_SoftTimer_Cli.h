char DRV_SoftTimer_Cli_uptime( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define DRV_SOFTTIMER__CLI_UPTIME_COMMAND { "uptime" , "Get uptime" , DRV_SoftTimer_Cli_uptime , 0 }
