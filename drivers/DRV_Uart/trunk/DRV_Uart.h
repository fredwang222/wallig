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
 * \if standalone_DRV_Uart
*	\mainpage Uart Driver
*    \else
*    \page Uart  Uart Driver
*    \endif
*   \author Gwendal Le Gall
*
*	\date 02/02/2010
*
*	\section Description Description
*	This a software timer driver, there is 3 way of using it:
*	- as a counter
*	- for time out management
*	- With a call back event
*	It's need a main hardware or system periodic timer to work
*	\section Configuration Configuration
*	Place here how to configure the driver
*	- This is an example of configuration
*
*	\section use use
*	This is an example of the use of a time out timer
*	\note the return values are not tested to simplify
        \code
        DRV_Uart_Cfg sParam;
        DRV_Uart_Handle hTimer1=NULL;

        DRV_Uart_Init();
        //config parameters
        sParam.tType=TimeOut;
        sParam.uiValue = 10;
        //open the timer
        DRV_Uart_Open("timer1",&hTimer1,&sParam)==No_Error)
        //start the timer
        DRV_Uart_Start(hTimer1);
        //do something
         ...
        //test the timer value
        if( DRV_Uart_GetValue(hTimer1) == 0 )
          printf("Timeout!\n");
        //close the timer
        DRV_Uart_Close(hTimer1);

        DRV_Uart_Terminate();
        \endcode
*	\section arch architecture specific
*
*	\subsection ARM7AT91NoOs ARM7AT91 NoOs
*
*	\ref UartLinux
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
	Init_Error,
	Bad_Param,
	No_Data,     //!< No Data in Buffer
	Input_Null,  //!< A input parameter pointer is NULL
	Unknown      //!< unexpected error
} DRV_Uart_Error;

/*!
 *  \brief Baudrate
 *  \public
 */
typedef enum
{
        BR4800,
        BR9600,
        BR115200,
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
/*! \fn DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle, DRV_Uart_Cfg *ptParam)
 *  \brief Open an instance of the driver.
 *  \param pcDeviceName Name of the Device to open.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptParam device configuration
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptParam);
/*! \fn DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle );
 *  \brief Close an instance of the driver.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle );

/*! \fn DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle , char *pcBuffer , int iLength);
 *  \brief Sen a buffer to the UART.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pcBuffer: pointer to the buffer to send
 *  \param iLength: length of the buffer
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle , char *pcBuffer , int iLength);

/*! \fn DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle , char *pcBuffer , int *piLength);
 *  \brief Receive a  buffer to the UART.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pcBuffer: pointer to the buffer
 *  \param piLength: length of the buffer
 *  \return Driver error.
 */
DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle , char *pcBuffer , int *piLength);

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

