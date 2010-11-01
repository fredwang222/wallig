# !/bin/bash
export DRV_PATH=Drv
export LIB_PATH=Lib
#remove comment from prj.conf
grep -v ^# prj.conf | sed s/#.*$// > prj.tmp
echo "read project configuration"
echo "----------------------------------------------"
#project name
export PRJ_NAME=`grep "PRJ_NAME" prj.tmp | sed s/PRJ_NAME=//`
echo "Project name: "$PRJ_NAME
export TARGET=`grep "TARGET" prj.tmp | sed s/TARGET=//`
echo "Target board:" $TARGET
#svn access type
if [[ `grep SVN prj.tmp` = "SVN=HTTPS" ]]
then
SVN_ACCESS=https
else
SVN_ACCESS=http
fi
echo ""

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
			echo "    get $DRV"
			svn co $SVN_ACCESS"://"`grep ^$DRV $DRV_PATH/Drv.conf | sed s/.*URL=//` $DRV_PATH/$DRV
		else
			echo "    $DRV already present"
		fi
	else
		echo "    $DRV not found"
	fi
done
echo ""

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
			echo "    get $LIB"
			svn co $SVN_ACCESS"://"`grep ^$LIB Lib/Lib.conf | sed s/.*URL=//` Lib/$LIB
		else
			echo "    $LIB already present"
		fi
	else
		echo "    $LIB not found"
	fi
done
echo ""

echo "----------------------------------------------"
echo "get sytem"
export ARCH=`grep $TARGET  System/System.conf | grep "ARCH=" | sed s/.*ARCH=//`
echo "    Arch: "$ARCH
if grep -q $ARCH System/System.conf
then
	if [ ! -e System/$ARCH ]
	then
		svn co $SVN_ACCESS"://"`grep ^$ARCH System/System.conf | sed s/.*URL=//` System/$ARCH
	fi
else
	echo "    $ARCH not found"
fi
echo ""

echo "----------------------------------------------"
echo "create Makefile"
Script/createmakefile.sh
rm prj.tmp


