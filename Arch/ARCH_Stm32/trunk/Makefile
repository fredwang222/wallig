ODIR=obj
OBJ=$(ODIR)/ARCH_Stm32_system.o $(ODIR)/stm32f10x_it.o $(ODIR)/stm32f10x_Vector.o 
OUTOBJ=$(ODIR)/SystemStm32.a

all: $(OUTOBJ) lib

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)
	
obj/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) -DDATA_IN_ExtSRAM -I . $(IDIR)

lib:
	@(cd STM32/lib && $(MAKE))

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

