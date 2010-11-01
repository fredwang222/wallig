# !/bin/bash
export DRV_PATH=Drv
export LIB_PATH=Lib
#remove comment from prj.conf
grep -v ^# prj.conf | sed s/#.*$// > prj.tmp
echo "read project configuration"
echo "----------------------------------------------"
export TARGET=`grep "TARGET_" prj.tmp`
echo "Target board:" $TARGET
#svn access type
if [[ `grep SVN_HTTP prj.tmp` = "SVN_HTTPS" ]]
then
SVN_ACCESS=https
else
SVN_ACCESS=http
fi

echo "----------------------------------------------"
#get drivers
export DRVIER_LIST=`grep DRV_ prj.tmp`
echo "Driver List:" $DRVIER_LIST

echo 'get drivers:'
for DRV in $DRVIER_LIST 
do 
	if grep -q $DRV $DRV_PATH/Drv.conf
	then 
		if [ ! -e $DRV_PATH/$DRV ]
		then
			echo "get $DRV"
			svn co $SVN_ACCESS"://"`grep $DRV $DRV_PATH/Drv.conf | sed s/.*URL=//` $DRV_PATH/$DRV
		else
			echo "    $DRV already present"
		fi
	else
		echo "    $DRV not found"
	fi
done

echo "----------------------------------------------"
export LIB_LIST=`grep LIB_ prj.tmp`
echo "Lib List:" $LIB_LIST
echo "get Lib:"
for LIB in $LIB_LIST 
do 
	if grep -q $LIB Lib/Lib.conf
	then 
		if [ ! -e Lib/$LIB ]
		then
			echo "get $LIB"
			svn co $SVN_ACCESS"://"`grep $LIB Lib/Lib.conf | sed s/.*URL=//` Lib/$LIB
		else
			echo "    $LIB already present"
		fi
	else
		echo "    $LIB not found"
	fi
done

echo "----------------------------------------------"
echo "get sytem"
export ARCH=`grep $TARGET System/System.conf | sed s/.*ARCH=//`
echo "    Arch: "$ARCH
#if [ ! -e Lib/$LIB ]
#then
#fi

echo "----------------------------------------------"
echo "create Makefile"
Script/createmakefile.sh


