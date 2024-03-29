#!/bin/bash
echo "genrate Drv.mk && Makefile"
grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// | grep DRV_ > conf.txt
echo "export DRV_OBJ :=" > Drv.mk
echo "export DRV_INC :=" >> Drv.mk
echo "all:" > Makefile
if grep -q "DRV_UART=y" $PRJ_CONFIG_FILE
then
echo "DRV_OBJ += Drv/Uart/obj/DRV_Uart.a" >> Drv.mk
echo 'DRV_INC += -I $(PROJECT_PATH)/Drv/Uart/' >> Drv.mk
echo '	@(cd Uart && $(MAKE))' >> Makefile
fi
if grep -q "DRV_GPIO=y" $PRJ_CONFIG_FILE
then
echo "DRV_OBJ += Drv/Gpio/obj/DRV_Gpio.a" >> Drv.mk
echo 'DRV_INC += -I $(PROJECT_PATH)/Drv/Gpio/' >> Drv.mk
echo '	@(cd Gpio && $(MAKE))' >> Makefile
fi

if grep -q "DRV_NAND=y" $PRJ_CONFIG_FILE
then
echo "DRV_OBJ += Drv/Nand/obj/DRV_Nand.o" >> Drv.mk
echo 'DRV_INC += -I $(PROJECT_PATH)/Drv/Nand/' >> Drv.mk
echo '	@(cd Nand && $(MAKE))' >> Makefile
fi


echo "" >> Makefile
echo "clean:" >> Makefile
if grep -q "DRV_UART=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Uart && $(MAKE) clean)' >> Makefile
fi
if grep -q "DRV_GPIO=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Gpio && $(MAKE) clean)' >> Makefile
fi
if grep -q "DRV_NAND=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Nand && $(MAKE) clean)' >> Makefile
fi


if grep -q "DRV_UART=y" $PRJ_CONFIG_FILE
then
cd Uart
./SetConfig.sh
fi

if grep -q "DRV_NAND=y" $PRJ_CONFIG_FILE
then
cd Nand
./SetConfig.sh
fi
