ODIR=obj
OBJ=$(ODIR)/LIB_Cli.o  $(ODIR)/LIB_Cli_Help.o

all: $(ODIR) $(OBJ)
	
obj/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(LIB_PATH) -I$(PROJECT_PATH)/$(DRV_PATH) -I$(PROJECT_PATH)/$(SYSTEM_INCLUDE_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

