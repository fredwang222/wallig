/**
*	\file DRV_Uart_private.h
*
*	\author Gwendal Le Gall
*
*	\date 20/02/2010
*	\brief Private declarations for the driver
*
*/
/** @defgroup DRV_Uart_Private_grp Private interface for DRV_Uart driver
 *  This the privates types and functions. There is 2 parts:
 *  - \ref DRV_Uart_Private_Common_grp the common data for all architectures
 *  - \ref DRV_Uart_Private_Arch_grp the Architecture dependent calls
 *  @{
 */

#include "DRV_Uart_SLIP.h"
#include "DRV_Uart_CFG.h"

/** @defgroup DRV_Uart_Private_Common_grp private common
 *
 *  @ingroup DRV_Uart_Private_grp
 *  @{
 */

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
 *  \brief RX function call by the device
 *  \param pUart device data
 *  \param pucBuffer input buffer
 *  \param iLength Number of char in the buffer
 *  \return the number of char read in the buffer
 */
int DRV_Uart_Private_Callback( DRV_Uart_Devicedata *pUart ,  unsigned char *pucBuffer , int iLength );

/** @} */

/** @defgroup DRV_Uart_Private_Arch_grp private Architecture
 *  This is the first group
 *  @ingroup DRV_Uart_Private_grp
 *  @{
 */

/*! \brief Safe section enter */
void DRV_Uart_Arch_RxSafeEnter( DRV_Uart_Devicedata *pUart );

/*!\brief Safe section leave  */
void DRV_Uart_Arch_RxSafeLeave( DRV_Uart_Devicedata *pUart );

/*! \brief Safe section enter */
void DRV_Uart_Arch_TxSafeEnter( DRV_Uart_Devicedata *pUart );

/*!\brief Safe section leave  */
void DRV_Uart_Arch_TxSafeLeave( DRV_Uart_Devicedata *pUart );

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
 *  \brief Send a buffer to the device
 */
DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength);

/*! flush the TXBuffer (both driver and device) */
DRV_Uart_Error DRV_Uart_ArchTXFlush( DRV_Uart_Devicedata *pUart);
/*! flush the RXBuffer (both driver and device) */
DRV_Uart_Error DRV_Uart_ArchRXFlush( DRV_Uart_Devicedata *pUart);
/** @} */
/** @} */ // end of DRV_Uart_Private_grp
