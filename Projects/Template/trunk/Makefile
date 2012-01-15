export PROJECT_PATH := $(CURDIR)
-include Config/config.mk
-include System/$(ARCH)/arch.mk
-include Drv/Drv.mk
export PRJ_CONFIG_FILE := $(PROJECT_PATH)/.config
export CONFIG_PATH := $(PROJECT_PATH)/Config
PROJECT_EXE = main.elf



all: Config/config.h  System/$(ARCH)/.system Drv/Drv.mk Drvivers $(PROJECT_EXE) 

config:
.config: 
	make -f Makefile.kconfig gconfig

Config/config.h: Config/config.mk
	@echo "debug"
	@echo '#ifndef CONFIG_H' > $@
	@echo '#define CONFIG_H' >> $@
	@grep "#" -v .config | grep -v ^$$ | sed s/CONFIG_/"#define "/ | sed s/"="/" "/ >> $@
	@echo '#endif' >> $@

Config/config.mk: .config
	@echo generation de $@
	@grep "#" -v .config | grep -v ^$$ | sed s/CONFIG_// > conf.txt
	@grep PRJ_NAME conf.txt | sed s/"="/":="/ >$@
	@echo ARCH := `grep ARCH_ conf.txt | sed s/"=y"//`>>$@
	@echo SUB_ARCH := `grep SUB_ conf.txt | sed s/"=y"//`>>$@
	@sed -i s/^/"export "/ $@
	#@rm conf.txt


Drv/Drv.mk: .config
	@(cd Drv && ./Drv.sh )	
	
Drvivers:
	@(cd Drv && $(MAKE) )	

System/$(ARCH)/.system:
	@(cd System/$(ARCH) && ./SetConfig.sh && $(MAKE))

src/main.o: src/main.c 
	@$(CC) -o $@ -c $< $(CFLAGS)  $(DRV_INC)
	@echo "compiling $@ "

$(PROJECT_EXE): src/main.o  $(DRV_OBJ) $(SYSTEM_LIB_OBJ) 
	$(CC) -o $@  $^  $(LDFLAGS) 
	@echo Creat binary file:
	arm-none-eabi-objcopy -O binary $@ $(patsubst %.elf,%.bin,$@) 
	@echo Creat disassembly file:
	arm-none-eabi-objdump -h -S -C -r $@  > $(patsubst %.elf,%.lss,$@)

clean:
	@(cd System/$(ARCH) && $(MAKE) clean)
	@(cd Drv && $(MAKE) clean)	
	@rm -rf src/*.o

test:
	@echo $(DRV_OBJ)

