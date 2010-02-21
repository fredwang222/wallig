/**
*	\file DRV_Uart.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Public declaration for the driver
*
*/
/*!
 * \page svn_log SVN log
 *  \include svn_log.txt
 */
/*!
 * \if standalone_DRV_Uart
*	\mainpage Uart Driver
*    \else
*    \page Uart  Uart Driver
*    \endif
*   \author Gwendal Le Gall
*
*	\date 20/02/2010
*
*	\section Description Description
*	This a simple Uart driver with SLIP protocole support
*	\section Configuration Configuration
*	The driver configuration is done at the opening of the device with DRV_Uart_Open(); \n
*	The settings are set thru this structure: DRV_Uart_Cfg \n
*	The input  packet  detection can be configured by:
*	- A number of char ( DRV_Uart_Cfg::iRXNbChar)
*	- A specific end char ( ( DRV_Uart_Cfg::cEndOfBuff) )
*	- A time out ( not yet implemented )
*	- Using the SLIP protocol ( DRV_Uart_Cfg::eSLIPModeEnable );
*
*	\section use use
*	The public declaration: \ref DRV_Uart_Public_grp
*	\note the return values are not tested to simplify\n
	\code
	DRV_Uart_Init( );
	tUart0Settings.RXCallBack=NULL;
	tUart0Settings.eBaudRate = BR115200;
	tUart0Settings.cEndOfBuff='\r';
	tUart0Settings.eParity = PARNone;
	tUart0Settings.eSLIPModeEnable=0;
	tUart0Settings.eStopBit = SB_1;
	tUart0Settings.iRXNbChar = 0;
	DRV_Uart_Open("/dev/ttyUSB0",&hUart,&tUart0Settings);
	DRV_Uart_Send(hUart,(unsigned char *) "Hello World\n",sizeof("HelloWorld\n"));
	while( 1 )
	{
		usleep(1000*100);
		if(DRV_Uart_RXDataReceived(hUart))
		{
			iNbCharIn=sizeof(tucRXBuffer);
			DRV_Uart_Receive(hUart , tucRXBuffer , &iNbCharIn);
			tucRXBuffer[iNbCharIn]=0;
			printf("RX %d chars: %s\n",iNbCharIn,tucRXBuffer);
		}

	}
	DRV_Uart_Close(hUart);
	DRV_Uart_Terminate();
	\endcode
*	\section arch architecture specific
*
*	\subsection ARM7AT91NoOs ARM7AT91 NoOs
*
*	\ref UartLinux
*
*	\section history History
*	\ref svn_log
*/
/** @defgroup DRV_Uart_Public_grp Public interface for DRV_Uart driver
 *  This is the first group
 *  @{
 */

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
	No_Error,    //!< OK
	Failed,      //!< generic error
	Device_Not_Found,
	AlreadyOpened,
	Init_Error,
	Bad_Param,
	No_Data,     //!< No Data in Buffer
	Input_Null,  //!< A input parameter pointer is NULL
	RXError,
	TXError,
	Unknown      //!< unexpected error
} DRV_Uart_Error;

/** @defgroup DRV_Uart_Settings Uart basics settings
 *  @ingroup DRV_Uart_Public_grp
 *  @{
 */

/*!
 *  \brief Baudrate
 *  \public
 */
typedef enum
{
        BR4800,         //!< 4800 bauds
        BR9600,         //!< 9600 bauds
        BR115200,       //!< 115000 bauds
        BRUnknown      //!< unexpected error
} DRV_Uart_BaudRate;

/*!
 *  \brief Stop bit length
 *  \public
 */
typedef enum
{
        SB_1,
        SB_1_5,
        SB_2,
        SBUnknown      //!< unexpected error
} DRV_Uart_StopBit;

/*!
 *  \brief Parity
 *  \public
 */
typedef enum
{
        PARNone,
        PAREven,
        PAROdd,
        PARUnknown      //!< unexpected error
} DRV_Uart_Parity;


/*!
 *  \brief configuration parameters for device
 */
typedef struct
{
      DRV_Uart_BaudRate eBaudRate;
      DRV_Uart_Parity eParity;
      DRV_Uart_StopBit eStopBit;
      int iRXNbChar;                    //!< Expected number of char to receive
      char cEndOfBuff;                  //!< End char for rx buff
      char eSLIPModeEnable;
      void (*RXCallBack)(char *pcBuffer , int iLength);
} DRV_Uart_Cfg;

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
/*! \fn DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle, DRV_Uart_Cfg *ptSettings)
 *  \brief Open an instance of the driver.
 *  \param pcDeviceName Name of the Device to open.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptSettings device settings
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptSettings);
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

/*! \fn DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle ,unsigned char *pucBuffer , int *piLength);
 *  \brief Receive a  buffer to the UART.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pucBuffer: pointer to the buffer
 *  \param piLength: length of the buffer
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle ,unsigned char *pucBuffer , int *piLength);

/*! \fn DRV_Uart_Error DRV_Uart_RXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag );
 *  \brief Enable reception
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param cFlag: 1 to enable 0 to disable
  *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_RXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag );

/*! \fn DRV_Uart_Error DRV_Uart_TXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag );
 *  \brief Enable transmission
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param cFlag: 1 to enable 0 to disable
  *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_TXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag );

/*!
 * \brief Is TX is busy?
 * \param hDeviceHandle Device Handle
 * \return 1 if busy else 0
 */
int DRV_Uart_TXBusy( DRV_Uart_Handle hDeviceHandle );

/*!
 * \brief Is there any data received?
 * \param hDeviceHandle Device Handle
 * \return 1 if data else 0
 */
int DRV_Uart_RXDataReceived( DRV_Uart_Handle hDeviceHandle );

/*!
 * \brief Flus tx buffers?
 * \param hDeviceHandle Device Handle
  */

DRV_Uart_Error DRV_Uart_TXFlush( DRV_Uart_Handle hDeviceHandle );

/*!
 * \brief Flus rx buffers?
 * \param hDeviceHandle Device Handle
  */

DRV_Uart_Error DRV_Uart_RXFlush( DRV_Uart_Handle hDeviceHandle );
/** @} */ // end of DRV_Uart_Public_grp
