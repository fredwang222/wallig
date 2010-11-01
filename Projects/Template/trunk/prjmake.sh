# !/bin/bash

#remove comment from prj.conf
grep -v ^# prj.conf | sed s/#.*$// > prj.tmp
cat prj.tmp
echo ""
echo "read project configuration"
TARGET=`grep "TARGET_" prj.tmp`
echo "Target board:" $TARGET
#get drivers
DRVIER_LIST=`grep DRV_ prj.tmp`
echo "Driver List:" $DRVIER_LIST
#get lib
LIB_LIST=`grep LIB_ prj.tmp`
echo "Lib List:" $LIB_LIST

