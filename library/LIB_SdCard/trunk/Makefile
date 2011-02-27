ODIR=obj
OBJ=$(ODIR)/LIB_SdCard.o

all: $(ODIR) $(OBJ)
	
obj/%.o : LIB_SdCard.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . 

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

