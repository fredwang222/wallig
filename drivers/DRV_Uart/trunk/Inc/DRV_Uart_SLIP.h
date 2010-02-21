/**
*	\file DRV_Uart_SLIP.h
*
*	\author Gwendal Le Gall
*
*	\date 20/02/2010
*	\brief SLIP Protocol interface
*
*/
/**************************************************************
						Typedef
***************************************************************/
typedef struct
{
	int iReceived;
	int iSlipState;
}tSLIP_Data;

/**************************************************************
					Functions
***************************************************************/
/*!
 * \brief slip protocol rx function
 * 	receives a packet into the buffer located at "p".
 *  If more than len bytes are received, the packet will be truncated.
 *  \return the number of bytes stored in the buffer.
 */
int DRV_Uart_Slip_Rx( tSLIP_Data *ptData , unsigned char ucInput , unsigned char *pucOutPutBuff, int iOutputMaxLen);
/*!
 *  \brief sends a packet of length "iLength", starting at * location "pucInputBuff".
 *  \return the length of the output buffer
 */
int  DRV_Uart_Slip_Tx(unsigned char *pucInputBuff, int iLength ,unsigned char *pucOutputBuff);
