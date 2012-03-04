#!/bin/bash
echo "genrate Mp3 Makefile"
grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// | grep LIB_ > conf.txt
#lib mad
if grep -q "LIB_MAD=y" $PRJ_CONFIG_FILE
then
	LIB_PATH=`grep LIB_MAD_SOURCE_PATH conf.txt | sed s/LIB_MAD_SOURCE_PATH=// | sed s/\"//g`
	LIB_ARCHIVE_NAME=`grep LIB_MAD_ARCHIVE_NAME conf.txt | sed s/LIB_MAD_ARCHIVE_NAME=// | sed s/\"//g`
	LIB_DIR_NAME=`echo $LIB_ARCHIVE_NAME| sed s/\.tar\.gz//`
	echo  "LIB_SOURCE_PATH :="$LIB_PATH >Makefile
	echo  "LIB_SOURCE_ARCHIVE_NAME :="$LIB_ARCHIVE_NAME >>Makefile
	echo  "LIB_SOURCE_DIRECTORY_NAME :="$LIB_DIR_NAME >>Makefile
	echo "" >>Makefile
	echo 'include libmad.mk' >>Makefile
	if [ ! -e Archive/$LIB_ARCHIVE_NAME ] ; then
		echo "download "$LIB_ARCHIVE_NAME
		wget -q $LIB_PATH/$LIB_ARCHIVE_NAME -O Archive/$LIB_ARCHIVE_NAME
	fi
	if [ ! -e Src/$LIB_DIR_NAME ]; then
		echo "extract "$LIB_ARCHIVE_NAME
		tar -xzf Archive/$LIB_ARCHIVE_NAME -C Src/
	fi 
fi

