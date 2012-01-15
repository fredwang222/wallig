grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// > conf.txt
if [ $SUB_ARCH != "SUB_STM32F2XX" ]
then
echo -n "LIB_SOURCE_ARCHIVE_NAME :=" >arch.mk
grep LIB_SOURCE_ARCHIVE_NAME_F1 conf.txt | sed s/LIB_SOURCE_ARCHIVE_NAME_F1=// | sed s/\"//g >>arch.mk
echo ""  >>arch.mk
else
echo -n "LIB_SOURCE_ARCHIVE_NAME :=" >arch.mk
grep LIB_SOURCE_ARCHIVE_NAME_F2 conf.txt | sed s/LIB_SOURCE_ARCHIVE_NAME_F2=// | sed s/\"//g >>arch.mk
echo ""  >>arch.mk
fi
grep LIB_SOURCE_PATH conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
grep INTERNAL_RAM conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
grep INTERNAL_ROM conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
sed -i s/^/"export "/ arch.mk
echo 'ifeq ($(SUB_ARCH),SUB_STM32F2XX)' >>arch.mk
echo 'include $(PROJECT_PATH)/System/$(ARCH)/arch_f2.mk' >>arch.mk
echo 'endif' >>arch.mk
echo "" >>arch.mk
