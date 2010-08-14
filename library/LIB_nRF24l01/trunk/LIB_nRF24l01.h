/*
 * 	 Author : Gwendal Le Gall
 *
*    Modified from:
 *     - nRF24l01 driver of Neil MacMillan see: http://code.google.com/p/nrf24l01/
 *     - http://www.tinkerer.eu/AVRLib/nRF24L01

 *
 *    This file is part of Wallig Library and Drivers.
 *
 *    Copyright (C) 2010  Gwendal Le Gall
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef NRF24L01_H_
#define NRF24L01_H_

/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/

typedef enum
{
	nRF24l01_No_Error,    //!< OK
	nRF24l01_Failed,      //!< generic error
	nRF24l01_Spi_Busy,	  //!< the spi is currently used by another task
	nRF24l01_Init_Error,
	nRF24l01_Bad_Param,
	nRF24l01__Rx_Fifo_Empty,  //!< there aren't any packets in the Rx FIFO to receive (Radio_Receive does not receive data)
	nRF24l01__Rx_More_Packet, //!< after copying out the head of the Rx FIFO, there is still another packet in the FIFO.
	nRF24l01_Err_Unknown      //!< unexpected error
}LIB_nRF24l01_Error;


/*****					Add labels for the packet types to the enumeration					*****/

typedef enum _pt
{
	MESSAGE,
	ACK,
} PACKET_TYPE;

/*****							Construct payload format structures							*****/

// structures must be 29 bytes long or less.

typedef struct _msg
{
	uint8_t messageid;
	uint8_t address[5];
	uint8_t messagecontent[23];
} pf_message_t;

typedef struct _ack
{
	uint8_t messageid;
} pf_ack_t;

/*****							Add format structures to the union							*****/

/// The application-dependent packet format.  Add structures to the union that correspond to the packet types defined
/// in the PACKET_TYPE enumeration.  The format structures may not be more than 29 bytes long.  The _filler array must
/// be included to ensure that the union is exactly 29 bytes long.
typedef union _pf
{
	uint8_t _filler[29];	// make sure the packet is exactly 32 bytes long - this array should not be accessed directly.
	pf_message_t message;
	pf_ack_t ack;
} payloadformat_t;

/*****						Leave the radiopacket_t structure alone.						*****/

typedef struct _rp
{
	PACKET_TYPE type;
	uint16_t timestamp;
	payloadformat_t payload;
} radiopacket_t;


#define RADIO_ADDRESS_LENGTH 5

typedef enum _radio_pipe {
	RADIO_PIPE_0 = 0,
	RADIO_PIPE_1 = 1,
	RADIO_PIPE_2 = 2,
	RADIO_PIPE_3 = 3,
	RADIO_PIPE_4 = 4,
	RADIO_PIPE_5 = 5,
	RADIO_PIPE_EMPTY = 7,	// FIFO is empty when pipe number bits in status register are 0b111.
}LIB_nRF24l01_PIPE;

typedef enum _radio_tx_power {
	RADIO_LOWEST_POWER = 0,		// -18 dBm (about 16 uW)
	RADIO_LOW_POWER = 1,		// -12 dBm (about 63 uW)
	RADIO_HIGH_POWER = 2,		// -6 dBm (about 251 uW)
	RADIO_HIGHEST_POWER = 3,	// 0 dBm (1 mW)
}LIB_nRF24l01_TX_POWER;

typedef enum _radio_dr {
	RADIO_1MBPS = 0,		// that's Mbps, not MBps.
	RADIO_2MBPS = 1,
}LIB_nRF24l01_DATA_RATE;

typedef enum _radio_transmit {
	RADIO_TX_MAX_RT,
	RADIO_TX_SUCCESS,
}LIB_nRF24l01_TX_STATUS;


typedef enum _radio_ack {
	RADIO_ACK,
	RADIO_NO_ACK,
}LIB_nRF24l01_USE_ACK;

/********************************************************************************************************/
/*							Public functions Declarations												*/
/********************************************************************************************************/

LIB_nRF24l01_Error LIB_nRF24l01_init( void );
LIB_nRF24l01_Error LIB_nRF24l01_Configure_Rx(LIB_nRF24l01_PIPE pipe, uint8_t* address, uint8_t enable);
LIB_nRF24l01_Error LIB_nRF24l01_Set_Tx_Addr(uint8_t* address);
LIB_nRF24l01_Error LIB_nRF24l01_Configure(LIB_nRF24l01_DATA_RATE dr, LIB_nRF24l01_TX_POWER power);
LIB_nRF24l01_Error LIB_nRF24l01_Receive(radiopacket_t* buffer  );
uint8_t LIB_nRF24l01_Success_Rate( void );
LIB_nRF24l01_Error LIB_nRF24l01_Flush( void );
#endif
