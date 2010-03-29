#include <string.h>
#include "LIB_Cli.h"
#include "LIB_CLI_CommandCfg.h"


#define COMMAND_NOT_FOUND "Command not found, try \"Help\""EOL_STRING

struct
{
	int iState;
#ifdef INPUT_TYPE_CHAR
	LIB_Cli_GetChar GetChar;
	 LIB_Cli_PutChar PutChar;
#else
	LIB_Cli_GetString GetString;
#endif
	LIB_Cli_PutString PutString;
	char tcInput[INPUT_LINE_MAX_LEN];
	char tcOutput[OUTPUT_LINE_MAX_LEN];
	char tcCommand[INPUT_LINE_MAX_LEN];
	int iInputindex;
	LIB_CLI_Function *pCurrenCommand;

} LIB_Cli;

#define LIB_CLI_COMMAND_TABLE_END { NULL , NULL , NULL , 0 }

LIB_CLI_Function tCommandTable[]={ LIB_CLI_COMMAND_LIST_INIT , LIB_CLI_COMMAND_TABLE_END};

char LIB_Cli_LookforComand( char *pcinputString , int *piIndex);

#ifdef INPUT_TYPE_CHAR
void LIB_Cli_init(LIB_Cli_GetChar GetChar , LIB_Cli_PutChar PutChar , LIB_Cli_PutString PutString)
#else
void LIB_Cli_init(LIB_Cli_GetString GetString , LIB_Cli_PutString PutString)
#endif

{
	LIB_Cli.iState=0;
	LIB_Cli.iInputindex=0;
#ifdef INPUT_TYPE_CHAR
	LIB_Cli.GetChar = GetChar;
#else
	LIB_Cli.GetString = GetString;
#endif
	LIB_Cli.PutString =PutString;

}

char LIB_Cli_Thread( void )
{
	char cInput;
	char *pChar;
	int iIndex;

	switch( LIB_Cli.iState )
	{
		default:
			LIB_Cli.iState=0;
			LIB_Cli.iInputindex=0;
		case 0:
#ifdef INPUT_TYPE_CHAR
			if( LIB_Cli.GetChar(&cInput) )
			{
				switch(cInput)
				{
					case '\b': //BackSpace
						if(LIB_Cli.iInputindex)
						{
							LIB_Cli.iInputindex--;
							LIB_Cli.PutString(ERASE_CHAR_SEQ);
						}
						break;
					case EOL_CHAR:
						if((LIB_Cli.iInputindex+1)<INPUT_LINE_MAX_LEN)
						{
							LIB_Cli.iInputindex++;
							LIB_Cli.tcInput[LIB_Cli.iInputindex]=0;
							LIB_Cli.PutChar(EOL_CHAR); //echo
						}
						LIB_Cli.iState++;
						break;
					default:
						if((LIB_Cli.iInputindex+1)<INPUT_LINE_MAX_LEN)
						{
							LIB_Cli.iInputindex++;
							LIB_Cli.tcInput[LIB_Cli.iInputindex]=cInput;
							LIB_Cli.PutChar(cInput); //echo
						}
						break;
				}
			}
#else
			if( LIB_Cli.GetString(LIB_Cli.tcInput) )
			{
				LIB_Cli.iState++;
			}
#endif
			else
			 break;
		case 1:
			//remove end chars
			pChar = strchr(LIB_Cli.tcInput,'\r');
			if(pChar != NULL)
				*pChar=0;
			pChar = strchr(LIB_Cli.tcInput,'\n');
			if(pChar != NULL)
				*pChar=0;
			//Get command
			strncpy( LIB_Cli.tcCommand ,  LIB_Cli.tcInput ,INPUT_LINE_MAX_LEN);
			LIB_Cli.tcCommand[INPUT_LINE_MAX_LEN-1]=0;
			pChar = strchr(LIB_Cli.tcCommand,' ');
			if(pChar != NULL)
				*pChar=0;
			iIndex=0;
			if( LIB_Cli_LookforComand( LIB_Cli.tcCommand , &iIndex) )
			{
				LIB_Cli.pCurrenCommand = &tCommandTable[iIndex];
				LIB_Cli.pCurrenCommand->iState=0;
				LIB_Cli.iState++;
				break;
			}
			else
			{
				LIB_Cli.PutString(COMMAND_NOT_FOUND);
			}
			LIB_Cli.PutString(PROMPT);
			LIB_Cli.iState=0;
			LIB_Cli.iInputindex=0;
			break;
		//Execute Command
		case 2:
			LIB_Cli.tcOutput[0]=0;
			if( !LIB_Cli.pCurrenCommand->Execution(&LIB_Cli.pCurrenCommand->iState ,LIB_Cli.tcInput , LIB_Cli.tcOutput, OUTPUT_LINE_MAX_LEN ))
			{
				//end of execution
				LIB_Cli.iState=0;
				LIB_Cli.iInputindex=0;

			}
			if( LIB_Cli.tcOutput[0] )
				LIB_Cli.PutString(LIB_Cli.tcOutput);
			if(LIB_Cli.iState==0)
				LIB_Cli.PutString(PROMPT);
			break;
	}
	return 1;
}

char LIB_Cli_LookforComand( char *pcinputString , int *piIndex)
{
	while(tCommandTable[*piIndex].pcCommandName!= NULL)
	{
		if( !strcmp(tCommandTable[*piIndex].pcCommandName , pcinputString) )
		{
			//Command found
			return 1;
		}
		(*piIndex)++;

	}
	return 0; //command not found
}

