
static  struct
{
	enum
	{
		eInit=0,
		eRead
	} tState;
} tLibMouseData ;

void vFlibMouse_Init(void )
{
	//init uart
}


void vFlibMouse_Demon( void )
{
	switch( tLibMouseData.tState )
	{
		default:
			tLibMouseData.tState = eInit;
		case eInit:
			break;
	}
}
