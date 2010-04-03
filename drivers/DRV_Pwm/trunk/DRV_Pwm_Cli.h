char DRV_Pwm_Cli_open( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define PWM_CLI_OPEN_COMMAND { "pwm_open" , "Open a pwm device" , DRV_Pwm_Cli_open , 0 }

char DRV_Pwm_Cli_Duty( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define PWM_CLI_DUTY_COMMAND { "pwm_duty" , "Set pwm duty cycle Value: 0 .. 100" , DRV_Pwm_Cli_Duty , 0 }


#define PWM_COMMAND_LIST PWM_CLI_OPEN_COMMAND , PWM_CLI_DUTY_COMMAND
