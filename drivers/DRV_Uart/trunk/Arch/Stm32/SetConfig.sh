grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ |grep CONFIG_DRV_UART > config.txt

Get_Value()
{
	grep $1 config.txt | sed s/$1=// | sed s/\"//g	
}


echo '#define kUART1_COM_PARAM { '`Get_Value CONFIG_DRV_UART1_BAUDRATE`' , '\
								 ''`Get_Value CONFIG_DRV_UART1_WORDLENGTH`' , '\
								 ''`Get_Value CONFIG_DRV_UART1_STOPBITS`' , '\
								 ''`Get_Value CONFIG_DRV_UART1_PARITY`' , '\
								'USART_Mode_Rx | USART_Mode_Tx , USART_HardwareFlowControl_None }' > $CONFIG_PATH/DRV_Uart_CFG.h
if [ $SUB_ARCH != "SUB_STM32F2XX" ]
then
echo '#define kUART1_CLK_DEV_PARAM { RCC_APB2PeriphClockCmd , RCC_APB2Periph_USART1| RCC_APB2Periph_AFIO }' >> $CONFIG_PATH/DRV_Uart_CFG.h
echo '#define kUART1_CLK_PIO_PARAM { RCC_APB2PeriphClockCmd , RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO  }' >> $CONFIG_PATH/DRV_Uart_CFG.h
else
echo '#define kUART1_CLK_DEV_PARAM { RCC_APB2PeriphClockCmd , RCC_APB2Periph_USART1 }' >> $CONFIG_PATH/DRV_Uart_CFG.h
echo '#define kUART1_CLK_PIO_PARAM { RCC_AHB1PeriphClockCmd , RCC_AHB1Periph_GPIOA  }' >> $CONFIG_PATH/DRV_Uart_CFG.h
fi
echo '#define kUART1_PIO_PARAM { GPIOA , GPIO_Pin_9 , GPIO_Pin_10 }' >> $CONFIG_PATH/DRV_Uart_CFG.h
echo '#define kUART1_CONF { "UART1" , USART1,  kUART1_COM_PARAM , kUART1_CLK_DEV_PARAM , kUART1_CLK_PIO_PARAM , kUART1_PIO_PARAM , USART1_IRQn }' >> $CONFIG_PATH/DRV_Uart_CFG.h

echo '/*Device liste*/'  >> $CONFIG_PATH/DRV_Uart_CFG.h
echo -n '#define kDRV_Uart_DEVICE_CONF { ' >> $CONFIG_PATH/DRV_Uart_CFG.h
if grep -q CONFIG_DRV_UART1=y config.txt
then
echo -n 'kUART1_CONF '  >> $CONFIG_PATH/DRV_Uart_CFG.h
if grep -q CONFIG_DRV_UART2=y  config.txt
then
echo -n ', '  >> $CONFIG_PATH/DRV_Uart_CFG.h
fi
fi
if grep -q CONFIG_DRV_UART2=y  config.txt
then
echo -n 'kUART2_CONF '  >> $CONFIG_PATH/DRV_Uart_CFG.h
fi
echo -n '}'  >> $CONFIG_PATH/DRV_Uart_CFG.h
