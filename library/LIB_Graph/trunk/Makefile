ODIR=obj
OBJ= $(ODIR)/LIB_Graph_Touch.o $(ODIR)/LIB_Graph.o $(ODIR)/LIB_Graph_Base.o  $(ODIR)/LIB_Graph_Text.o  $(ODIR)/fixed_18.o  
OUTOBJ=$(ODIR)/LIB_Graph.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I${PROJECT_PATH}/${DRV_GPIO_PATH} -I${PROJECT_PATH}/$(DRV_SPI_PATH) -I${PROJECT_PATH}/$(DRV_TFT_PATH) -I${PROJECT_PATH}/$(DRV_TIMER_PATH)

$(ODIR)/%.o : Fonts/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I${PROJECT_PATH}/${DRV_GPIO_PATH} -I${PROJECT_PATH}/$(DRV_SPI_PATH) -I${PROJECT_PATH}/$(DRV_TFT_PATH) -I${PROJECT_PATH}/$(DRV_TIMER_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
