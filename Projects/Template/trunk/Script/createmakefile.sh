# !/bin/bash
MAKFILE=Makefile
echo "#$MAKFILE generated with prjmake.sh" >$MAKFILE
echo "">>$MAKFILE
echo 'export PROJECT_PATH=$(CURDIR)'>>$MAKFILE
echo 'export CONFIG_PATH=Config'>>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "# system variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "export ARCH_PATH=System/"$ARCH>>$MAKFILE
echo 'include  $(ARCH_PATH)/arch.mk'>>$MAKFILE
echo "export ARCH_DEFINE="`grep $TARGET System/System.conf | grep 'DEF=' | sed s/.*DEF=//`>>$MAKFILE
echo 'ARCH_OBJ=$(wildcard $(ARCH_PATH)/obj/*.o)'>>$MAKFILE
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# driver variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#drivers">>$MAKFILE
echo "export DRV_PATH="$DRV_PATH>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo "#"$DRV>>$MAKFILE
echo "export "$DRV'_PATH=$(DRV_PATH)/'$DRV>>$MAKFILE
echo $DRV'_OBJ=$('$DRV'_PATH)/obj/'$DRV'.a'>>$MAKFILE
done
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# Lib variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#Library">>$MAKFILE
echo "export LIB_PATH="$LIB_PATH>>$MAKFILE
for LIB in $LIB_LIST 
do 
echo "#"$LIB>>$MAKFILE
echo "export "$LIB'_PATH=$(LIB_PATH)/'$LIB>>$MAKFILE
echo $LIB'_OBJ=$('$LIB'_PATH)/obj/'$LIB'.a'>>$MAKFILE
done
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# project variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#project">>$MAKFILE
echo 'PROJECT_SRC=$(wildcard src/*.c)'>>$MAKFILE
echo 'PROJECT_OBJ:=$(patsubst %.c,%.o,$(PROJECT_SRC))'>>$MAKFILE
echo 'PROJECT_EXE='$PRJ_NAME'.elf'>>$MAKFILE
echo "">>$MAKFILE


echo "######################################################################">>$MAKFILE
echo "# inlcude dir">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo -n "export IDIR=-I. -I..  ">>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo -n '-I$(PROJECT_PATH)/$('$DRV'_PATH) '>>$MAKFILE
done
for LIB in $LIB_LIST 
do 
echo -n '-I$(PROJECT_PATH)/$('$LIB'_PATH) '>>$MAKFILE
done
echo -n '-I$(PROJECT_PATH)/$(ARCH_PATH) -I$(PROJECT_PATH)/$(ARCH_PATH)/lib/inc '>>$MAKFILE
echo " ">>$MAKFILE
echo " ">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "#all target">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo -n 'all: '>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo -n $DRV" ">>$MAKFILE
done
for LIB in $LIB_LIST 
do 
echo -n $LIB" ">>$MAKFILE
done
echo -n "Arch ">>$MAKFILE
echo -n '$(PROJECT_EXE) '>>$MAKFILE
echo " ">>$MAKFILE
echo " ">>$MAKFILE

echo -n '$(PROJECT_EXE): $(PROJECT_OBJ) $(ARCH_OBJ) '>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo -n '$('$DRV'_OBJ) '>>$MAKFILE
done
for LIB in $LIB_LIST 
do 
echo -n '$('$LIB'_OBJ) '>>$MAKFILE
done
echo " ">>$MAKFILE
echo '	$(CC) -o $@  $^ $(LDFLAGS) -lm'>>$MAKFILE
echo '	@echo Creat binary file:'>>$MAKFILE
echo '	arm-none-eabi-objcopy -O binary $@ $(patsubst %.elf,%.bin,$@) '>>$MAKFILE
echo '	@echo Creat disassembly file:'>>$MAKFILE
echo '	arm-none-eabi-objdump -h -S -C -r $@  > $(patsubst %.elf,%.lss,$@)'>>$MAKFILE
echo " ">>$MAKFILE


echo "######################################################################">>$MAKFILE
echo "#project target">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "src/%.o:src/%.c">>$MAKFILE
echo '	$(CC) -o $@ -c $< $(CFLAGS) $(IDIR)'>>$MAKFILE
echo " ">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# system targets">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "Arch:">>$MAKFILE
echo '	@(cd $(ARCH_PATH) && $(MAKE))'>>$MAKFILE
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# driver targets">>$MAKFILE
echo "######################################################################">>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo $DRV":">>$MAKFILE
echo '	@(cd $('$DRV'_PATH) && $(MAKE))'>>$MAKFILE
done
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# Lib targets">>$MAKFILE
echo "######################################################################">>$MAKFILE
for LIB in $LIB_LIST 
do 
echo $LIB":">>$MAKFILE
echo '	@(cd $('$LIB'_PATH) && $(MAKE))'>>$MAKFILE
done
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# clean target">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "clean:">>$MAKFILE
echo '	rm -rf $(PROJECT_OBJ)'>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo '	@(cd $('$DRV'_PATH) && $(MAKE) $@)'>>$MAKFILE
done
for LIB in $LIB_LIST 
do 
echo '	@(cd $('$LIB'_PATH) && $(MAKE) $@)'>>$MAKFILE
done

