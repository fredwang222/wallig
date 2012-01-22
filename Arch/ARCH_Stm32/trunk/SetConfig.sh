grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// > conf.txt
#get CPU
#echo CPU_ID
CPU_ID=`grep "CPU_" conf.txt | sed s/CPU_// | sed s/"=y"// `
echo $CPU_ID
#set cpu RAM and ROM size
INTERNAL_RAM_SIZE=`grep $CPU_ID cpu.list | grep RAM | sed s/.*"="' '*//`
#echo  INTERNAL_RAM_SIZE = $INTERNAL_RAM_SIZE
INTERNAL_ROM_SIZE=`grep $CPU_ID cpu.list | grep ROM | sed s/.*"="' '*//`
#echo  INTERNAL_ROM_SIZE = $INTERNAL_ROM_SIZE
if [ $SUB_ARCH != "SUB_STM32F2XX" ]
then
echo -n "LIB_SOURCE_ARCHIVE_NAME :=" >arch.mk
grep LIB_SOURCE_ARCHIVE_NAME_F1 conf.txt | sed s/LIB_SOURCE_ARCHIVE_NAME_F1=// | sed s/\"//g >>arch.mk
else
#echo LIB_SOURCE_ARCHIVE_NAME_F2
echo -n "LIB_SOURCE_ARCHIVE_NAME :=" >arch.mk
grep LIB_SOURCE_ARCHIVE_NAME_F2 conf.txt | sed s/LIB_SOURCE_ARCHIVE_NAME_F2=// | sed s/\"//g >>arch.mk
fi
#echo LIB_SOURCE_PATH conf.txt
grep LIB_SOURCE_PATH conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
echo "INTERNAL_RAM := "$INTERNAL_RAM_SIZE>>arch.mk
echo "INTERNAL_ROM := "$INTERNAL_ROM_SIZE>>arch.mk
sed -i s/^/"export "/ arch.mk
echo 'ifeq ($(SUB_ARCH),SUB_STM32F2XX)' >>arch.mk
echo 'include $(PROJECT_PATH)/System/$(ARCH)/arch_f2.mk' >>arch.mk
echo 'endif' >>arch.mk
echo "" >>arch.mk
