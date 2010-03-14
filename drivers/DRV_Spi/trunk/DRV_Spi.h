/**
*	\file DRV_Spi.h
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
 * \if standalone_DRV_Spi
*	\mainpage SPI Driver
*    \else
*    \page SPI  SPI Driver
*    \endif
*   \author Gwendal Le Gall
*
*	\date 20/02/2010
*
*	\section Description Description
*	This a simple SPI driver
*	\section Configuration Configuration
*
*
*	\subsection ARM7AT91NoOs ARM7AT91 NoOs
*
*	\ref SPILinux
*
*	\section history History
*	\ref svn_log
*/
/** @defgroup DRV_Spi_Public_grp Public interface for DRV_Spi driver
 *  This is the first group
 *  @{
 */

/**************************************************************
						Typedef
***************************************************************/
/*!
 *  \brief Handle for device
 */
typedef void * DRV_Spi_Handle;

/*!
 *  \brief Returned error for driver
 *  \public
 */
typedef enum
{
	SPI_No_Error,    //!< OK
	SPI_Failed,      //!< generic error
	SPI_Device_Not_Found,
	SPI_AlreadyOpened,
	SPI_Init_Error,
	SPI_Bad_Param,
	SPI_No_Data,     //!< No Data in Buffer
	SPI_Input_Null,  //!< A input parameter pointer is NULL
	SPI_RXError,
	SPI_TXError,
	SPI_Unknown      //!< unexpected error
} DRV_Spi_Error;

/** @defgroup DRV_Spi_Settings SPI basics settings
 *  @ingroup DRV_Spi_Public_grp
 *  @{
 */



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
} DRV_Spi_Parity;


/*!
 *  \brief configuration parameters for device
 */
typedef struct
{
      unsigned int uiClkFreq;							//!< clock frequency in kHz
      unsigned char ucBitsPerTransfer;
      void (*RXCallBack)(char *pcBuffer , int iLength); //!< End of reception call back
      void (*TXCallBack)( int iLength); 				//!< End of transmission call back
} DRV_Spi_Cfg;

/** @} */ //
/**************************************************************
					Functions
***************************************************************/
/*! \fn DRV_Spi_Error DRV_Spi_Init( void )
 *  \brief Initialization the driver.
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Init( void );

/*! \fn DRV_Spi_Error DRV_Spi_Terminate( void )
 *  \brief Terminate the driver.
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Terminate( void );
/*! \fn DRV_Spi_Error DRV_Spi_Open( const char * pcDeviceName , DRV_Spi_Handle *phDeviceHandle, DRV_Spi_Cfg *ptSettings)
 *  \brief Open an instance of the driver.
 *  \param pcDeviceName Name of the Device to open.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptSettings device settings
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Open( const char * pcDeviceName , DRV_Spi_Handle *phDeviceHandle , DRV_Spi_Cfg *ptSettings);
/*! \fn DRV_Spi_Error DRV_Spi_Close( DRV_Spi_Handle hDeviceHandle );
 *  \brief Close an instance of the driver.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Close( DRV_Spi_Handle hDeviceHandle );

/*! \fn DRV_Spi_Error DRV_Spi_Send( DRV_Spi_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength);
 *  \brief Sen a buffer to SPI.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pucBuffer: pointer to the buffer to send
 *  \param iLength: length of the buffer
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Send( DRV_Spi_Handle hDeviceHandle ,unsigned  char *pucBuffer , int iLength);

/*! \fn DRV_Spi_Error DRV_Spi_Receive( DRV_Spi_Handle hDeviceHandle ,unsigned char *pucBuffer , int *piLength);
 *  \brief Receive a  buffer to SPI.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param pucBuffer: pointer to the buffer
 *  \param piLength: length of the buffer
 *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_Receive( DRV_Spi_Handle hDeviceHandle ,unsigned char *pucBuffer , int *piLength);

/*! \fn DRV_Spi_Error DRV_Spi_RXEnable( DRV_Spi_Handle hDeviceHandle , char cFlag );
 *  \brief Enable reception
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param cFlag: 1 to enable 0 to disable
  *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_RXEnable( DRV_Spi_Handle hDeviceHandle , char cFlag );

/*! \fn DRV_Spi_Error DRV_Spi_TXEnable( DRV_Spi_Handle hDeviceHandle , char cFlag );
 *  \brief Enable transmission
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param cFlag: 1 to enable 0 to disable
  *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_TXEnable( DRV_Spi_Handle hDeviceHandle , char cFlag );


/*! \fn DRV_Spi_Error DRV_Spi_ClockSpeedSet( DRV_Spi_Handle hDeviceHandle , unsigned int uiClkSpeed );
 *  \brief Set the clock speed
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param uiClkSpeed: New clock frequency in kHz.
  *  \return Driver error.
 */
DRV_Spi_Error DRV_Spi_ClockSpeedSet( DRV_Spi_Handle hDeviceHandle , unsigned int uiClkSpeed );

/*!
 * \brief Is TX is busy?
 * \param hDeviceHandle Device Handle
 * \return 1 if busy else 0
 */
int DRV_Spi_TXBusy( DRV_Spi_Handle hDeviceHandle );

/*!
 * \brief Is there any data received?
 * \param hDeviceHandle Device Handle
 * \return 1 if data else 0
 */
int DRV_Spi_RXDataReceived( DRV_Spi_Handle hDeviceHandle );

/*!
 * \brief Flus tx buffers?
 * \param hDeviceHandle Device Handle
  */

DRV_Spi_Error DRV_Spi_TXFlush( DRV_Spi_Handle hDeviceHandle );

/*!
 * \brief Flus rx buffers?
 * \param hDeviceHandle Device Handle
  */
DRV_Spi_Error DRV_Spi_RXFlush( DRV_Spi_Handle hDeviceHandle );
/** @} */ // end of DRV_Spi_Public_grp
