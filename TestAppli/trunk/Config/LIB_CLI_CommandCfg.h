#include "LIB_Cli.h"
#include "LIB_CLI_Help.h"
#include "ARCH_Cli.h"
#include "DRV_SoftTimer_Cli.h"




#define LIB_CLI_COMMAND_LIST_INIT  LIB_CLI_HELP_COMMAND ,\
								   LIB_CLI_HISTORY ,\
								   LIB_CLI_HELLO_COMMAND ,\
								   ARCH_CLI_RESET_COMMAND ,\
								   DRV_SOFTTIMER__CLI_UPTIME_COMMAND
