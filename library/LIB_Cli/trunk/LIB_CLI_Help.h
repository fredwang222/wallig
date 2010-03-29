#define LIB_CLI_HELP_NAME "help"
#define LIB_CLI_HELP_HELP "Print list of available commands"EOL_STRING

char LIB_Cli_Help_Command( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
char LIB_Cli_HelloWorld( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);


#define LIB_CLI_HELP_COMMAND { LIB_CLI_HELP_NAME , LIB_CLI_HELP_HELP , LIB_Cli_Help_Command , 0 }
#define LIB_CLI_HELLO_COMMAND { "hello" , "hello world test" , LIB_Cli_HelloWorld , 0 }
