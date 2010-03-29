#ifndef LIB_CLI_H
#define LIB_CLI_H
#include "LIB_CLI_Cfg.h"
#ifdef INPUT_TYPE_CHAR
typedef char (*LIB_Cli_GetChar)(char *) ;
typedef char (*LIB_Cli_PutChar)(char );
#else
typedef char (*LIB_Cli_GetString)(char *) ;
#endif
typedef char (*LIB_Cli_PutString)(char *) ;

typedef struct
{
	char *pcCommandName;
	char *pcHelp;
	char (*Execution)( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
	int iState;
}LIB_CLI_Function;

#ifdef INPUT_TYPE_CHAR
void LIB_Cli_init(LIB_Cli_GetChar GetChar , LIB_Cli_PutChar PutChar ,  LIB_Cli_PutString PutString);
#else
void LIB_Cli_init(LIB_Cli_GetString GetString , LIB_Cli_PutString PutString);
#endif
char LIB_Cli_Thread( void );

#endif
