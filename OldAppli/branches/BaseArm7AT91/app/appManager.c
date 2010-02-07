#include "appTest/appTest.h"
#include "appIpStack/appIpStack.h"

void vFappManager_Init( void )
{
	vFappTest_init();
	//vFappIpStack_Init();
}

void vFappManager_Main( void)
{
	vFappTest();
	//vFappIpStack();
}

