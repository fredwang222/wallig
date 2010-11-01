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
#svn access type
if [[ `grep SVN_HTTP prj.tmp` = "SVN_HTTPS" ]]
then
SVN_ACCESS=https
else
SVN_ACCESS=http
fi
echo $SVN_ACCESS

#get drivers
for DRV in $DRVIER_LIST 
do 
	if grep -q $DRV Drv/Drv.conf
	then 
		if [ ! -e Drv/$DRV ]
		then
			echo "get $DRV"
			svn co $SVN_ACCESS"://"`grep $DRV Drv/Drv.conf | sed s/.*URL=//` Drv/$DRV
		fi
	else
		echo "$DRV not found"
	fi
done

#get Lib
for LIB in $LIB_LIST 
do 
	if grep -q $LIB Lib/Lib.conf
	then 
		if [ ! -e Lib/$LIB ]
		then
			echo "get $LIB"
			svn co $SVN_ACCESS"://"`grep $LIB Lib/Lib.conf | sed s/.*URL=//` Lib/$LIB
		fi
	else
		echo "$LIB not found"
	fi
done
