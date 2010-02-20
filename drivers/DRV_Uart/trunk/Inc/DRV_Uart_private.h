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
    RXClosed,
	RXEmpty,
	RXFull,
} tRxState;

typedef enum
{
    TXClosed,
	TXIdle,
	TXBusy,
} tTxState;
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
	tRxState eRxState;
	tTxState eTxState;
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
/*!
 *  \brief RX function call by the device
 *  \param pUart device data
 *  \param pucBuffer input buffer
 *  \param iLength Number of char in the buffer
 *  \return the number of char read int the buffer
 */
int DRV_Uart_Private_Callback( DRV_Uart_Devicedata *pUart ,  unsigned char *pucBuffer , int iLength );

DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength);
