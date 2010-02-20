/**
*	\file DRV_Uart_private.h
*
*	\author Gwendal Le Gall
*
*	\date 20/02/2010
*	\brief Private declarations for the driver
*
*/
#include "DRV_Uart_SLIP.h"
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
	char *pcDeviceName;            			///< Device Name
	DRV_Uart_Cfg cfg;        				///< Driver configuration
	void *pArchData;						///< Architecture data
	tSLIP_Data tSLIP;						///< SLIP protocol data
	unsigned char tucRXBuff[kDRV_Uart_RXBuffSize];	///< Rx buffer
	int iRxBuffIndex;								///< Rx buffer index
	unsigned char tucTXBuff[kDRV_Uart_TXBuffSize];	///< Tx buffer
	int iTxBuffIndex;								///< Tx buffer index
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
/*!
 *  \brief open a device
 *  \note This is architecture dependent
 */

DRV_Uart_Error DRV_UART_ArchOpen( DRV_Uart_Devicedata *pUart );
/*!
 *  \brief Close a device
 *  \note This is architecture dependent
 */
DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart );

int DRV_Uart_Private_Callback( DRV_Uart_Devicedata *pUart ,  unsigned char *pucFiFoBuffer , int iLength );
