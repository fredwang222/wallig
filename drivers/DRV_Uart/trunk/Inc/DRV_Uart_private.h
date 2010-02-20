/**
*	\file DRV_Uart_private.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Private declarations for the driver
*
*/
#include "DRV_Uart_CFG.h"
typedef enum
{
        Closed,
	started,
	stopped
} tState;

/*!
 *  \brief Uart device data
 */
typedef struct
{
	char *pcDeviceName;            ///< Device Name
	DRV_Uart_Cfg cfg;         ///< Driver configuration
	void *pArchData;
	char tcRXBuff[kDRV_Uart_RXBuffSize];
	char tcTXBuff[kDRV_Uart_TXBuffSize];
	tState eRxState;
	tState eTxState;
} DRV_Uart_Devicedata;

/*!
 *  \brief Safe section enter
 */
void DRV_Uart_SafeEnter( DRV_Uart_Devicedata *pUart );

/*!
 *  \brief Safe section leave
 */
void DRV_Uart_SafeLeave( DRV_Uart_Devicedata *pUart );

/*!
 *  \brief Uart driver init
 *  \note This is architecture dependent
 */
DRV_Uart_Error DRV_Uart_ArchInit(void );

/*!
 *  \brief Uart driver terminate
 *  \note This is architecture dependent
 */
DRV_Uart_Error DRV_Uart_ArchTerminate(void );


DRV_Uart_Error DRV_UART_ArchOpen( DRV_Uart_Devicedata *pUart );
DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart );
/*!
 *  \brief Called by the Main Timer
 */
void DRV_Softimer_TimerCallBack( void );
