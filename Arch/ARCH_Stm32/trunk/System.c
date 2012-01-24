#include "stdlib.h"
#include "System.h"

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void* _sbrk (int incr)
{
#ifdef DATA_IN_ExtSRAM
	static char *heap_end = (char*)0x68000000;
#else
    static char *heap_end=&end;		/* Defined by the linker */
#endif
    char *prev_heap_end;

  prev_heap_end = heap_end;
#ifdef DATA_IN_ExtSRAM
	if ( incr >= (EXTERNAL_RAM_SIZE*1024))
#else
  if (heap_end + incr > __get_MSP())
#endif
    {
      assert_failed(__FILE__,__LINE__);
    }
  heap_end += incr;
  return (void *) prev_heap_end;
}


int  _kill( int b, int c)
{
	return 0;
}

int _getpid( void )
{
	return 1;
}

void _exit( void )
{

}

