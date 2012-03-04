#!/bin/bash
echo "genrate Lib.mk && Makefile"
grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// | grep LIB_ > conf.txt
########### Obj and lib list ################
echo "export LIB_OBJ :=" > Lib.mk
echo "export LIB_INC :=" >> Lib.mk
#fifo
if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
then
echo "LIB_OBJ += Lib/Fifo/LIB_Fifo.o" >> Lib.mk
echo "LIB_INC += -I Lib/Fifo/" >> Lib.mk
fi
#Mp3
if grep -q "LIB_MP3=y" $PRJ_CONFIG_FILE
then
echo "LIB_OBJ += Lib/Mp3/libmp3.a" >> Lib.mk
echo "LIB_INC += -I Lib/Mp3/" >> Lib.mk
fi
echo "" >> Makefile
######### all target ###########
echo "all:" > Makefile
#fifo
if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Fifo && $(MAKE))' >> Makefile
fi
#Mp3
if grep -q "LIB_MP3=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Mp3 && $(MAKE))' >> Makefile
fi
echo "" >> Makefile
######### clean target ###########
echo "clean:" >> Makefile
if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Fifo && $(MAKE) clean)' >> Makefile
fi
if grep -q "LIB_MP3=y" $PRJ_CONFIG_FILE
then
echo '	@(cd Mp3 && $(MAKE) clean)' >> Makefile
fi

###### Mp3 Lib configuration ######
if grep -q "LIB_MP3=y" $PRJ_CONFIG_FILE
then
cd Mp3
./SetConfig.sh
fi

#if grep -q "LIB_FIFO=y" $PRJ_CONFIG_FILE
#then
#cd Fifo
#./SetConfig.sh
#fi

touch .Lib
