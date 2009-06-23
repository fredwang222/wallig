################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libHttpd/http-strings.c \
../lib/libHttpd/httpd-cgi.c \
../lib/libHttpd/httpd-fs.c \
../lib/libHttpd/httpd-fsdata.c \
../lib/libHttpd/httpd.c 

OBJS += \
./lib/libHttpd/http-strings.o \
./lib/libHttpd/httpd-cgi.o \
./lib/libHttpd/httpd-fs.o \
./lib/libHttpd/httpd-fsdata.o \
./lib/libHttpd/httpd.o 

C_DEPS += \
./lib/libHttpd/http-strings.d \
./lib/libHttpd/httpd-cgi.d \
./lib/libHttpd/httpd-fs.d \
./lib/libHttpd/httpd-fsdata.d \
./lib/libHttpd/httpd.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libHttpd/%.o: ../lib/libHttpd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DkUSE_IP_STACK -DVECTORS_IN_RAM -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


