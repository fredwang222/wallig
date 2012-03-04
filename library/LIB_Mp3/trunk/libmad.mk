LIBMP3_OBJ=libmp3.a
ODIR=obj
_OBJ=version.o fixed.o bit.o timer.o stream.o frame.o synth.o decoder.o layer12.o layer3.o huffman.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

vpath %.c Src/$(LIB_SOURCE_DIRECTORY_NAME)

all: $(LIBMP3_OBJ) 
	
obj/%.o : %.c
	@$(CC) -o $@ -c $< $(CFLAGS_S) -I .  -DFPM_DEFAULT
	@echo compiling $@


$(LIBMP3_OBJ): $(ODIR) $(OBJ)
	@echo $(OBJ)
	arm-none-eabi-ar -cr $(LIBMP3_OBJ) $(OBJ)   
	arm-none-eabi-ranlib $(LIBMP3_OBJ)
	@echo "make $(LIBMP3_OBJ)"

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

