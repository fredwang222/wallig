ODIR=obj
OBJ=$(ODIR)/LIB_Ntl_BBM.o

all: $(ODIR) $(OBJ)
	
obj/%.o : Src/LIB_Ntl_BBM.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I $(PROJECT_PATH)/$(DRV_NAND_PATH) 

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

