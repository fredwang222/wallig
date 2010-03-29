#include <string.h>
#include "LIB_Cli.h"


#define HELP_FIRST_LINE "List of commands:"EOL_STRING
#define HELP_LAST_LINE "To have help for a command: \"help command\""EOL_STRING

extern LIB_CLI_Function tCommandTable[];
extern char LIB_Cli_LookforComand( char *pcinputString , int *piIndex);

char LIB_Cli_Help_Command( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	static int iIndex;
	static char *pcFirstArg;
	char *pcEndChar;

	switch(*pState )
	{
		case 0:
			iIndex=0;
			(*pState)++;
		case 1: //without args list all commands
			//get first arg
			pcFirstArg=pcArgs;
			do
			{
				pcFirstArg = strchr(pcFirstArg,' ');
				if(pcFirstArg != NULL)
					pcFirstArg++;
			}while(pcFirstArg != NULL &&   *pcFirstArg == ' ');

			if( pcFirstArg == NULL )
			{
				strcpy( pcOutput , HELP_FIRST_LINE );
				(*pState)++;

			}
			else
			{
				pcEndChar = strchr(pcFirstArg,' ');
				if(pcEndChar != NULL)
					pcEndChar=0;
				(*pState)=3;
			}
			break;
		case 2:
			if( tCommandTable[iIndex].pcCommandName )
			{
				strncpy( pcOutput , tCommandTable[iIndex].pcCommandName , iOutputLen);
				strcat(pcOutput , EOL_STRING );
				iIndex++;
			}
			else
			{
				strcpy( pcOutput , HELP_LAST_LINE );
				(*pState) =-1;
				return 0; //end of command
			}
			break;
		case 3:
			strncpy( pcOutput , "hlep for: " , iOutputLen);
			strcat(pcOutput , pcFirstArg );
			strcat(pcOutput , EOL_STRING );
			(*pState) =4;
			break;
		case 4:
			iIndex=0;
			if( LIB_Cli_LookforComand( pcFirstArg , &iIndex ) )
				strncpy( pcOutput ,  tCommandTable[iIndex].pcHelp , iOutputLen);
			else
				strncpy( pcOutput ,  "Command not found" , iOutputLen);
			strcat( pcOutput , EOL_STRING );
			return 0; //end of command
		default:
			return 0;

	}
	return 1;
}


char LIB_Cli_HelloWorld( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	strcpy( pcOutput , "Hello World\r" );
	return 0;
}
