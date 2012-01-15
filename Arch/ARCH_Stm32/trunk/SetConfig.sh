grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ | sed s/CONFIG_// > conf.txt
grep LIB_SOURCE_ARCHIVE_NAME conf.txt | sed s/"="/" := "/ | sed s/\"//g>arch.mk
grep LIB_SOURCE_PATH conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
grep INTERNAL_RAM conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
grep INTERNAL_ROM conf.txt | sed s/"="/" := "/ | sed s/\"//g>>arch.mk
sed -i s/^/"export "/ arch.mk
echo 'ifeq ($(SUB_ARCH),SUB_STM32F2XX)' >>arch.mk
echo 'include $(PROJECT_PATH)/System/$(ARCH)/arch_f2.mk' >>arch.mk
echo 'endif' >>arch.mk
echo "" >>arch.mk
