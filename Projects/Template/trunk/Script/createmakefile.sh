# !/bin/bash
MAKFILE=Makefile
echo "#$MAKFILE generated with prjmake.sh" >$MAKFILE
echo "">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#system include">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "include  System/"$ARCH"/arch.mk">>$MAKFILE
echo "">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "# driver variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#drivers">>$MAKFILE
echo "export DRV_PATH="$DRV_PATH>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo "#"$DRV>>$MAKFILE
echo $DRV'_PATH=$(DRV_PATH)/'$DRV>>$MAKFILE
echo $DRV'_OBJ=$(wildcard $('$DRV'_PATH)/obj/*.o)'>>$MAKFILE
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
echo $LIB'_PATH=$(DRV_PATH)/'$LIB>>$MAKFILE
echo $LIB'_OBJ=$(wildcard $('$LIB'_PATH)/obj/*.o)'>>$MAKFILE
done
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "# project variables">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "#project">>$MAKFILE
echo 'PROJECT_SRC=$(wildcard src/*.c)'>>$MAKFILE
echo 'PROJECT_OBJ:=$(patsubst %.c,%.o,$(PROJECT_SRC))'>>$MAKFILE
echo "">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "#all target">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo -n 'all: $(PROJECT_OBJ) '>>$MAKFILE
for DRV in $DRVIER_LIST 
do 
echo -n $DRV" ">>$MAKFILE
done
for LIB in $LIB_LIST 
do 
echo -n $LIB" ">>$MAKFILE
done
echo " ">>$MAKFILE
echo " ">>$MAKFILE

echo "######################################################################">>$MAKFILE
echo "#project target">>$MAKFILE
echo "######################################################################">>$MAKFILE
echo "src/%.o:src/%.c">>$MAKFILE
echo '	$(CC) -o $@ -c $< $(CFLAGS) $(IDIR)'>>$MAKFILE
echo " ">>$MAKFILE

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
for DRV in $DRVIER_LIST 
do 
echo '	@(cd $('$DRV'_PATH) && $(MAKE) $@)'>>$MAKFILE
done

