target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
#monitor arm7_9 force_hw_bkpts enable
#monitor arm7_9 dbgrq enable
monitor gdb_breakpoint_override hard
monitor gdb_memory_map disable 
monitor gdb_flash_program enable 
#monitor arm7_9 fast_memory_access enable 
monitor arm7_9 dcc_downloads enable
monitor flash probe 0
# WDT_MR, disable watchdog 
monitor mww 0xFFFFFD44 0x00008000

# RSTC_MR, enable user reset
monitor mww 0xfffffd08 0xa5000001

# CKGR_MOR
monitor mww 0xFFFFFC20 0x00000601
monitor sleep 10

# CKGR_PLLR
monitor mww 0xFFFFFC2C 0x00481c0e
monitor sleep 10

# PMC_MCKR
monitor mww 0xFFFFFC30 0x00000007
monitor sleep 10

# PMC_IER
monitor mww 0xFFFFFF60 0x00480100
monitor sleep 100
#monitor flash erase_address  0x00100000 0x20000
monitor sleep 500
monitor flash write_image erase Debug/Sam7P256.elf 0 elf
#load
#break main
display/i $pc
stepi
#continue
