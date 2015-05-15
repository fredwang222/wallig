The original goal of this projet was to write a software of a robot based on a remote controlled Wall-e toy. (on a STM32 Cortex M3 µC ).

Now the first step is to write basic drivers and library which are able to run on linux pc or arm board.
  * UART Driver (linux , STM32 )
  * [Timer](http://code.google.com/p/wallig/source/browse/drivers/DRV_Timer/trunk/) (linux , STM32 )
  * IP Stack ( using uIP ) (linux , STM32 )
  * Trace Library ( send traces on udp, serial or in log file ) (linux , STM32 )
  * Fat ( using chan fat )  (linux , STM32 )
  * [Spi Driver](http://code.google.com/p/wallig/source/browse/drivers/DRV_Spi/trunk/) (STM32 )
  * SdCard library (STM32 )
  * [ADC driver](http://code.google.com/p/wallig/source/browse/drivers/DRV_Adc/trunk/) (STM32 )
  * [PWM driver](http://code.google.com/p/wallig/source/browse/drivers/DRV_Pwm/trunk/) (STM32 )
  * [Quadrature encoder driver](http://code.google.com/p/wallig/source/browse/drivers/DRV_Quad/trunk/) (STM32 )