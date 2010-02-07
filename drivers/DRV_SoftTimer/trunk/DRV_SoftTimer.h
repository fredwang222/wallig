/**
*	\file DRV_SoftTimer.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Public declaration for the driver
*
*/

/*!
 * \if standalone_DRV_SoftTimer
*	\mainpage SoftTimer Driver
*    \else
*    \page SoftTimer  SoftTimer Driver
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
        DRV_SoftTimer_Cfg sParam;
        DRV_SoftTimer_Handle hTimer1=NULL;

        DRV_SoftTimer_Init();
        //config parameters
        sParam.tType=TimeOut;
        sParam.uiValue = 10;
        //open the timer
        DRV_SoftTimer_Open("timer1",&hTimer1,&sParam)==No_Error)
        //start the timer
        DRV_SoftTimer_Start(hTimer1);
        //do something
         ...
        //test the timer value
        if( DRV_SoftTimer_GetValue(hTimer1) == 0 )
          printf("Timeout!\n");
        //close the timer
        DRV_SoftTimer_Close(hTimer1);

        DRV_SoftTimer_Terminate();
        \endcode
*	\section arch architecture specific
*
*	\subsection ARM7AT91NoOs ARM7AT91 NoOs
*
*	\ref SoftTimerLinux
*/

/**************************************************************
						Typedef
***************************************************************/
/*!
 *  \brief Handle for device
 */
typedef void * DRV_SoftTimer_Handle;

/*!
 *  \brief Returned error for driver
 *  \public
 */
typedef enum
{
	No_Error,    //!< OK
	Failed,      //!< generic error
	Input_Null,  //!< A input parameter pointer is NULL
	Unknown      //!< unexpected error
} DRV_SoftTimer_Error;

/*!
 *  SoftTimer Type
 */
typedef enum
{
	Counter, //!< increment when the main timer reach DRV_SoftTimer_Cfg::uiValue
	TimeOut, //!< start at DRV_SoftTimer_Cfg::uiValue , and decrement
	Periodic //!< Execute the callback function periodically
} DRV_SoftTimerType;

/*!
 *  \brief configuration parameters for device
 */
typedef struct
{
	DRV_SoftTimerType tType; //!< Type of timer
	unsigned int uiValue;    //!< Value of the timer

} DRV_SoftTimer_Cfg;

/**************************************************************
					Functions
***************************************************************/
/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Init( void )
 *  \brief Initialization the driver.
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Init( void );

/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Terminate( void ) 
 *  \brief Terminate the driver.
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Terminate( void );
/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Open( const char * pcDeviceName , DRV_SoftTimer_Handle *phDeviceHandle, DRV_SoftTimer_Cfg *ptParam)
 *  \brief Open an instance of the driver.
 *  \param pcDeviceName Name of the Device to open.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptParam device configuration
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Open( const char * pcDeviceName , DRV_SoftTimer_Handle *phDeviceHandle , DRV_SoftTimer_Cfg *ptParam);
/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Close( DRV_SoftTimer_Handle hDeviceHandle );
 *  \brief Close an instance of the driver.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Close( DRV_SoftTimer_Handle hDeviceHandle );

/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Start( DRV_SoftTimer_Handle hDeviceHandle );
 *  \brief Start the timer.
 *  \param hDeviceHandle Handle of timer.
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Start( DRV_SoftTimer_Handle hDeviceHandle );

/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Stop( DRV_SoftTimer_Handle hDeviceHandle );
 *  \brief Stop the timer.
 *  \param hDeviceHandle Handle of timer.
 *  \return Driver error.
 */
DRV_SoftTimer_Error DRV_SoftTimer_Stop( DRV_SoftTimer_Handle hDeviceHandle );

/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_GetValue( DRV_SoftTimer_Handle hDeviceHandle );
 *  \brief Get The Timer value.
 *  \param hDeviceHandle Handle of timer.
 *  \return The value of the timer.
 */
unsigned int DRV_SoftTimer_GetValue( DRV_SoftTimer_Handle hDeviceHandle );

/*! \fn DRV_SoftTimer_Error DRV_SoftTimer_Reset( DRV_SoftTimer_Handle hDeviceHandle );
 *  \brief Reset The Timer value.
 *  \param hDeviceHandle Handle of timer.
 *  \return The value of the timer.
 */
void DRV_SoftTimer_Reset( DRV_SoftTimer_Handle hDeviceHandle );
