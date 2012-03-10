export SYSTEM_PATH = $(PROJECT_PATH)/System/$(ARCH)
export SYS_INCLUDE = -I $(SYSTEM_PATH)
export CC=gcc 
export CFLAGS= -O0 -g3  $(SYS_INCLUDE) 
export SYSTEM_LIB_OBJ =$(SYSTEM_PATH)/system.o 

