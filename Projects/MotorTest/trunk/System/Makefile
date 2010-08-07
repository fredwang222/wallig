ODIR=obj
OBJ=$(ODIR)/ARCH_Stm32_system.o $(ODIR)/stm32f10x_it.o $(ODIR)/stm32f10x_Vector.o
OUTOBJ=$(ODIR)/SystemStm32.a

all: $(OUTOBJ) lib

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)
	
obj/%.o : Arch/Stm32/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH)  -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH)

lib:
	@(cd Arch/Stm32/lib && $(MAKE))

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

