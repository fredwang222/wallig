ODIR=obj
OBJ= $(ODIR)/DRV_Nand.o 

all: $(ODIR) $(OBJ)

$(ODIR)/%.o : Arch/$(ARCH)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)  -I . $(IDIR) -I$(CONFIG_PATH) 

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
