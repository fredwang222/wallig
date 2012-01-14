if grep -q "CONFIG_ARCH_STM32=y" $PRJ_CONFIG_FILE
then
Arch/Stm32/SetConfig.sh
fi
