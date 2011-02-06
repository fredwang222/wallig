/**
*	\file DRV_Uart.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Public declaration for the driver
*
*/
//! number of devices configured
#ifndef DRV_UART_H
#define DRV_UART_H
/**************************************************************
						Typedef
***************************************************************/
/*!
 *  \brief Handle for device
 */
typedef void * DRV_Uart_Handle;

/*!
 *  \brief Returned error for driver
 *  \public
 */
typedef enum
{
	UART_No_Error,    //!< OK
	UART_Failed,      //!< generic error
	UART_Device_Not_Found,
	UART_AlreadyOpened,
	UART_Init_Error,
	UART_Bad_Param,
	UART_No_Data,     //!< No Data in Buffer
	UART_Input_Null,  //!< A input parameter pointer is NULL
	UART_RXError,
	UART_TXError,
	UART_Unknown      //!< unexpected error
} DRV_Uart_Error;


typedef struct
{
	void (*RXCallBack)( DRV_Uart_Handle hDeviceHandle , uint8_t *pcBuffer , uint8_t *piLength);
	void (*TXCallBack)( DRV_Uart_Handle hDeviceHandle );
} DRV_Uart_Cb;

/*!
 *  \brief configuration parameters for device
 */
typedef struct
{
	uint8_t *pucTxBuffer;
	uint8_t ucTxBufferLen;
	DRV_Uart_Cb Callbacks;

} DRV_Uart_Param;

/** @} */ //
/**************************************************************
					Functions
***************************************************************/
/*! \fn DRV_Uart_Error DRV_Uart_Init( void )
 *  \brief Initialization the driver.
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Init( void );

/*! \fn DRV_Uart_Error DRV_Uart_Terminate( void )
 *  \brief Terminate the driver.
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Terminate( void );
/*! \fn DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle, DRV_Uart_Param *ptParam)
 *  \brief Open an instance of the driver.
 *  \param pcDeviceName Name of the Device to open.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptSettings device settings
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Param *ptParam);
/*! \fn DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle );
 *  \brief Close an instance of the driver.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle );

/*! \fn DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength);
 *  \brief Sen a buffer to the UART.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pucBuffer: pointer to the buffer to send
 *  \param iLength: length of the buffer
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer , int iLength);


/*!
 * \brief Is TX is busy?
 * \param hDeviceHandle Device Handle
 * \return 1 if busy else 0
 */
int DRV_Uart_TXBusy( DRV_Uart_Handle hDeviceHandle );

DRV_Uart_Error DRV_Uart_TXFlush( DRV_Uart_Handle hDeviceHandle );


#endif
