ODIR=obj

ifeq ($(SUB_ARCH),SUB_STM32F2XX)
include arch_f2.mk
endif

all: .system  $(ODIR) $(SYSTEM_LIB_OBJ)


.system: lib stm32.ld

Archive/$(LIB_SOURCE_ARCHIVE_NAME):
	#@echo Archive/$(LIB_SOURCE_ARCHIVE_NAME)
	wget $(LIB_SOURCE_PATH)/$(LIB_SOURCE_ARCHIVE_NAME) -O $@
	

lib: Archive/$(LIB_SOURCE_ARCHIVE_NAME)
	mkdir temp
	mkdir lib
	@unzip Archive/$(LIB_SOURCE_ARCHIVE_NAME) -d temp
	cp -r temp/*/Libraries/* lib/
	rm -rf temp 
	
ifeq ($(SUB_ARCH),SUB_STM32F2XX)
include Makefile.f2
endif


$(ODIR):
	@echo $(STM32LIB_PATH)
	mkdir $@


