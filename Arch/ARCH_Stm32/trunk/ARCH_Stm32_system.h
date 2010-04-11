typedef void (*IrqTimerHandler_t)(void *pvParam);

void Sys_TimerIrq_Register(  uint8_t NVIC_IRQChannel , IrqTimerHandler_t Handler , void *pvParam);
void Sys_TimerIrq_Unregister(  uint8_t NVIC_IRQChannel );
