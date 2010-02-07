/**
*	\file DRV_SoftTimer_private.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Private declarations for the driver
*
*/

typedef enum
{
        Closed,
	started,
	stopped
} tState;

/*!
 *  \brief SoftTimer device data
 */
typedef struct
{
	char *pcDeviceName;            ///< Device Name
	DRV_SoftTimer_Cfg cfg;         ///< Driver configuration
	unsigned int uiCountValue;     ///< internal count value
	unsigned int uiReturnedValue;  ///< User value
	tState eState;
} DRV_Softimer_Devicedata;

/*!
 *  \brief Safe section enter
 */
void DRV_SoftTimer_SafeEnter( void );

/*!
 *  \brief Safe section leave
 */
void DRV_SoftTimer_SafeLeave( void );

/*!
 *  \brief Main Timer init
 *  \note This is architecture dependent
 */
DRV_SoftTimer_Error DRV_SofTimer_MainInit(void );

/*!
 *  \brief Main Timer init
 *  \note This is architecture dependent
 */
DRV_SoftTimer_Error DRV_SofTimer_MainTerminate(void );


/*!
 *  \brief Called by the Main Timer
 */
void DRV_Softimer_TimerCallBack( void );
