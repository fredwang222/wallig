ODIR=obj
OBJ=$(ODIR)/LIB_Trace.o

all: $(ODIR) $(OBJ)
	
obj/%.o : LIB_Trace.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(LIB_UIP_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

