#!/bin/bash
echo "genrate Lib.mk && Makefile"
grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// | grep LIB_ > conf.txt
echo "export LIB_OBJ :=" > Lib.mk
echo "export LIB_INC :=" >> Lib.mk
echo "all:" > Makefile
if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
then
echo "LIB_OBJ += Lib/Uart/obj/LIB_Fifo.a" >> Lib.mk
echo "LIB_INC += -I Lib/Uart/" >> Lib.mk
echo '	@(cd Uart && $(MAKE))' >> Makefile
fi
echo "" >> Makefile
echo "clean:" >> Makefile
if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Uart && $(MAKE) clean)' >> Makefile
fi

#if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
#then
#cd Fifo
#./SetConfig.sh
#fi

touch .Lib
