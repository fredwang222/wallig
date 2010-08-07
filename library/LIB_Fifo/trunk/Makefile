ODIR=obj
OBJ=$(ODIR)/LIB_Fifo.o

all: $(ODIR) $(OBJ)
	
obj/%.o : LIB_Fifo.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . 

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

