ODIR=obj
OBJ=$(ODIR)/LIB_Ntl_BBM.o
LIB_NTL_OBJ=libntl.a
all: $(ODIR) $(LIB_NTL_OBJ)
	
obj/%.o : Src/LIB_Ntl_BBM.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . $(DRV_INC)  -I$(CONFIG_PATH)

$(LIB_NTL_OBJ) : $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)   

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

