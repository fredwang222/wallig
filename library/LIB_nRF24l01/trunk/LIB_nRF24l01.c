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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "LIB_nRF24l01.h"
#include "inc/LIB_nRF24l01_p.h"
#include "LIB_nRF24l01_Cfg.h"
#include "DRV_Spi.h"
#include "DRV_Timer.h"
#include "DRV_Gpio.h"

/********************************************************************************************************/
/*							Define																		*/
/********************************************************************************************************/
#define CHANNEL 112
#define ADDRESS_LENGTH 5

/********************************************************************************************************/
/*							Macro																		*/
/********************************************************************************************************/

#define CSN_LOW() DRV_Gpio_ValueSet(LIB_nRF24l01_Data.hCSN , 0)
#define CSN_HIGH() DRV_Gpio_ValueSet(LIB_nRF24l01_Data.hCSN , 1)
#define CE_LOW() DRV_Gpio_ValueSet(LIB_nRF24l01_Data.hCE , 0)
#define CE_HIGH() DRV_Gpio_ValueSet(LIB_nRF24l01_Data.hCE , 1)

/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/

typedef struct
{
	char *pcSPI_Device_Name;
	char *pcCE_Gpio_Name;
	char *pcCSN_Gpio_Name;
	char *pcIRQ_Gpio_Name;
} LIB_nRF24l01_Cfg_t;

typedef struct
{
	DRV_Spi_Handle hSpi;
	DRV_Gpio_Handle hCE;
	DRV_Gpio_Handle hCSN;
	DRV_Gpio_Handle hIRQ;
	uint8_t ucTransmit_lock;      //!< Flag which denotes that the radio is currently transmitting
	uint16_t tx_history ;         //!< the driver keeps track of the success status for the last 16 transmissions
	uint8_t tx_address[5];        //!< holds the transmit address (Rx pipe 0 is set to this address when transmitting with auto-ack enabled).
	uint8_t rx_pipe0_address[5];  //!< holds the receiver address for Rx pipe 0 (the address is overwritten when transmitting with auto-ack enabled).
	uint8_t rx_pipe_widths[6];    //!< tracks the payload widths of the Rx pipes
	LIB_nRF24l01_TX_STATUS tx_last_status;

} LIB_nRF24l01_Data_t;

/**************************************************************
					Local variables
***************************************************************/

LIB_nRF24l01_Cfg_t LIB_nRF24l01_Cfg = { NRF24L01_SPI_NAME , NRF24L01_CE_NAME , NRF24L01_CSN_NAME, NRF24L01_IRQ_NAME};
LIB_nRF24l01_Data_t LIB_nRF24l01_Data;

/********************************************************************************************************/
/*							Local functions	declaration													*/
/********************************************************************************************************/

void LIB_nRF24l01_Irq_CallBack( void );
void _delay_us( unsigned int uiDelay);
static void  Radio_Init( void );
static void configure_registers( void );
static void Enable_Irq( void );
static void Disable_Irq( void );
static void reset_pipe0_address( void );
static void set_tx_mode( void );
static void set_rx_mode( void );
static void send_instruction(uint8_t instruction, uint8_t* data, uint8_t* buffer, uint8_t len);
static uint8_t get_register(radio_register_t reg, uint8_t* buffer, uint8_t len);
static uint8_t set_register(radio_register_t reg, uint8_t* value, uint8_t len);
static uint8_t get_status(void);
/********************************************************************************************************/
/*							Public functions															*/
/********************************************************************************************************/


LIB_nRF24l01_Error LIB_nRF24l01_init( void )
{
	DRV_GpioINT IRQ_int;

	if( DRV_Spi_Open( LIB_nRF24l01_Cfg.pcSPI_Device_Name , &LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Init_Error;
	if( DRV_Gpio_Open( LIB_nRF24l01_Cfg.pcCE_Gpio_Name , &LIB_nRF24l01_Data.hCE , NULL )!=GPIO_No_Error)
		return nRF24l01_Init_Error;
	if( DRV_Gpio_Open( LIB_nRF24l01_Cfg.pcCSN_Gpio_Name , &LIB_nRF24l01_Data.hCSN , NULL )!=GPIO_No_Error)
		return nRF24l01_Init_Error;
	IRQ_int.EventType=GPIO_INT_High2Low;
	IRQ_int.CallBack = LIB_nRF24l01_Irq_CallBack;
	if( DRV_Gpio_Open( LIB_nRF24l01_Cfg.pcIRQ_Gpio_Name , &LIB_nRF24l01_Data.hIRQ , &IRQ_int)!=GPIO_No_Error)
		return nRF24l01_Init_Error;

	LIB_nRF24l01_Data.tx_history = 0xFF;
	memset(LIB_nRF24l01_Data.tx_address,0xe7,sizeof(LIB_nRF24l01_Data.tx_address));
	memset(LIB_nRF24l01_Data.rx_pipe0_address,0xe7,sizeof(LIB_nRF24l01_Data.rx_pipe0_address));
	memset(LIB_nRF24l01_Data.rx_pipe_widths,0x00,sizeof(LIB_nRF24l01_Data.rx_pipe_widths));
	LIB_nRF24l01_Data.rx_pipe_widths[0]=32;
	LIB_nRF24l01_Data.rx_pipe_widths[1]=32;
	LIB_nRF24l01_Data.tx_last_status = RADIO_TX_SUCCESS;
	Radio_Init();

	return nRF24l01_No_Error;
}




// default address for pipe 0 is 0xe7e7e7e7e7
// default address for pipe 1 is 0xc2c2c2c2c2
// default address for pipe 2 is 0xc2c2c2c2c3 (disabled)
// default address for pipe 3 is 0xc2c2c2c2c4 (disabled)
// default address for pipe 4 is 0xc2c2c2c2c5 (disabled)
// default address for pipe 5 is 0xc2c2c2c2c6 (disabled)
LIB_nRF24l01_Error LIB_nRF24l01_Configure_Rx(LIB_nRF24l01_PIPE pipe, uint8_t* address, uint8_t enable)
{
	uint8_t value;
	uint8_t use_aa = 1;
	uint8_t payload_width = 32;
	if (payload_width < 1 || payload_width > 32 || pipe < RADIO_PIPE_0 || pipe > RADIO_PIPE_5)
		return nRF24l01_Bad_Param;


	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;

	Disable_Irq();
	// store the pipe 0 address so that it can be overwritten when transmitting with auto-ack enabled.
	if (pipe == RADIO_PIPE_0)
	{
		LIB_nRF24l01_Data.rx_pipe0_address[0] = address[0];
		LIB_nRF24l01_Data.rx_pipe0_address[1] = address[1];
		LIB_nRF24l01_Data.rx_pipe0_address[2] = address[2];
		LIB_nRF24l01_Data.rx_pipe0_address[3] = address[3];
		LIB_nRF24l01_Data.rx_pipe0_address[4] = address[4];
	}

	// Set the address.  We set this stuff even if the pipe is being disabled, because for example the transmitter
	// needs pipe 0 to have the same address as the Tx address for auto-ack to work, even if pipe 0 is disabled.
	set_register(RX_ADDR_P0 + pipe, address, pipe > RADIO_PIPE_1 ? 1 : ADDRESS_LENGTH);

	// Set auto-ack.
	get_register(EN_AA, &value, 1);
	if (use_aa)
		value |= (1<<pipe);
	else
		value &= ~(1<<pipe);
	set_register(EN_AA, &value, 1);

	// Set the pipe's payload width.  If the pipe is being disabled, then the payload width is set to 0.
	value = enable ? payload_width : 0;
	set_register(RX_PW_P0 + pipe, &value, 1);
	LIB_nRF24l01_Data.rx_pipe_widths[pipe] = value;

	// Enable or disable the pipe.
	get_register(EN_RXADDR, &value, 1);
	if (enable)
		value |= (1<<pipe);
	else
		value &= ~(1<<pipe);
	set_register(EN_RXADDR, &value, 1);

	Enable_Irq();
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );

	return nRF24l01_No_Error;
}

// default transmitter address is 0xe7e7e7e7e7.
LIB_nRF24l01_Error LIB_nRF24l01_Set_Tx_Addr(uint8_t* address)
{

	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;
	Disable_Irq();
	LIB_nRF24l01_Data.tx_address[0] = address[0];
	LIB_nRF24l01_Data.tx_address[1] = address[1];
	LIB_nRF24l01_Data.tx_address[2] = address[2];
	LIB_nRF24l01_Data.tx_address[3] = address[3];
	LIB_nRF24l01_Data.tx_address[4] = address[4];
	set_register(TX_ADDR, address, ADDRESS_LENGTH);

	Enable_Irq();
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );

	return nRF24l01_No_Error;
}

LIB_nRF24l01_Error LIB_nRF24l01_Configure(LIB_nRF24l01_DATA_RATE dr, LIB_nRF24l01_TX_POWER power)
{
	uint8_t value;

	if (power < RADIO_LOWEST_POWER || power > RADIO_HIGHEST_POWER || dr < RADIO_1MBPS || dr > RADIO_2MBPS)
		return nRF24l01_Bad_Param;

	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;

	Disable_Irq();
	// set the address
	//Radio_Set_Tx_Addr(address);

	// set the data rate and power bits in the RF_SETUP register
	get_register(RF_SETUP, &value, 1);

	value |= 3 << RF_PWR;			// set the power bits so that the & will mask the power value in properly.
	value &= power << RF_PWR;		// mask the power value into the RF status byte.

	if (dr)
		value |= RF_DR;
	else
		value &= ~RF_DR;

	set_register(RF_SETUP, &value, 1);


	Enable_Irq();
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );

	return nRF24l01_No_Error;
}

LIB_nRF24l01_Error LIB_nRF24l01_Transmit(radiopacket_t* payload )
{
	//if (block && ucTransmit_lock) while (ucTransmit_lock);
	//if (!block && ucTransmit_lock) return 0;
	uint8_t len = 32;

	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;

	Disable_Irq();
	// indicate that the driver is transmitting.
	LIB_nRF24l01_Data.ucTransmit_lock = 1;

	// disable the radio while writing to the Tx FIFO.
    CE_LOW();

	set_tx_mode();

    // for auto-ack to work, the pipe0 address must be set to the Tx address while the radio is transmitting.
    // The register will be set back to the original pipe 0 address when the TX_DS or MAX_RT interrupt is asserted.
    set_register(RX_ADDR_P0, (uint8_t*)LIB_nRF24l01_Data.tx_address, ADDRESS_LENGTH);

    // transfer the packet to the radio's Tx FIFO for transmission
    send_instruction(W_TX_PAYLOAD, (uint8_t *) payload, NULL, len);

    // start the transmission.
    CE_HIGH();

    /* No blocking transmit*/
    /*if (wait == RADIO_WAIT_FOR_TX)
    {
    	while (LIB_nRF24l01_Data.ucTransmit_lock);
    	return LIB_nRF24l01_Data.tx_last_status;
    	nRF24l01_No_Error;
    }*/

    Enable_Irq();
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );

	return nRF24l01_No_Error;

}

LIB_nRF24l01_Error LIB_nRF24l01_Receive(radiopacket_t* buffer  )
{
	uint8_t len = 32;
	uint8_t status;
	uint8_t pipe_number;
	LIB_nRF24l01_Error Return;

	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;

	Disable_Irq();
	LIB_nRF24l01_Data.ucTransmit_lock = 0;

	CE_LOW();

    status = get_status();
	pipe_number =  (status & 0xE) >> 1;

	if (pipe_number == RADIO_PIPE_EMPTY)
	{
		Return = nRF24l01__Rx_Fifo_Empty;
	}
	else
	{
		if (LIB_nRF24l01_Data.rx_pipe_widths[pipe_number] > len)
		{
			// the buffer isn't big enough, so don't copy the data.
			Return = nRF24l01_Bad_Param;
		}
		else
		{
			// Move the data payload into the local
			send_instruction(R_RX_PAYLOAD, (uint8_t*)buffer, (uint8_t*)buffer, LIB_nRF24l01_Data.rx_pipe_widths[pipe_number]);

			status = get_status();
			pipe_number =  (status & 0xE) >> 1;

			if (pipe_number != RADIO_PIPE_EMPTY)
				Return = nRF24l01__Rx_More_Packet;
			else
				Return = nRF24l01_No_Error; // there was a packet to receive, it was successfully received, and the Rx FIFO is now empty.

		}
	}

	CE_HIGH();

	LIB_nRF24l01_Data.ucTransmit_lock = 0;

	//release_radio();
	Enable_Irq();
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );

	return Return;

}

// This is only accurate if all the failed packets were sent using auto-ack.
uint8_t LIB_nRF24l01_Success_Rate( void )
{
	uint16_t wh = LIB_nRF24l01_Data.tx_history;
	uint8_t weight = 0;
	while (wh != 0)
	{
		if ((wh & 1) != 0) weight++;
		wh >>= 1;
	}
	wh = (16 - weight) * 100;
	wh /= 16;
	return wh;
}

LIB_nRF24l01_Error LIB_nRF24l01_Flush()
{
	if( DRV_Spi_LockSet( LIB_nRF24l01_Data.hSpi ) != Spi_No_Error )
		return nRF24l01_Spi_Busy;

	Disable_Irq();
	send_instruction(FLUSH_TX, NULL, NULL, 0);
	send_instruction(FLUSH_RX, NULL, NULL, 0);
	DRV_Spi_LockRelease( LIB_nRF24l01_Data.hSpi );
	Enable_Irq();
	return nRF24l01_No_Error;
}

/********************************************************************************************************/
/*							Local functions																*/
/********************************************************************************************************/

/**
 * Retrieve the status register.
 */
static uint8_t get_status( void )
{
	uint8_t status = 0;
	CSN_LOW();

	status = DRV_Spi_RW_Byte(LIB_nRF24l01_Data.hSpi,NOP);

	CSN_HIGH();

	return status;
}
/**
 * Set a register in the radio
 * \param reg The register value defined in nRF24L01.h (e.g. CONFIG, EN_AA, &c.).
 * \param value The value to write to the given register (the whole register is overwritten).
 * \return The status register.
 */
static uint8_t set_register(radio_register_t reg, uint8_t* value, uint8_t len)
{
	uint8_t status;
	CSN_LOW();

	status = DRV_Spi_RW_Byte(LIB_nRF24l01_Data.hSpi, W_REGISTER | (REGISTER_MASK & reg));
	DRV_Spi_Write_Buffer(LIB_nRF24l01_Data.hSpi,value, len);

	CSN_HIGH();

	return status;
}

/**
 * Retrieve a register value from the radio.
 * \param reg The register value defined in nRF24L01.h (e.g. CONFIG, EN_AA, &c.).
 * \param buffer A contiguous memory block into which the register contents will be copied.  If the buffer is too long for the
 * 		register contents, then the remaining bytes will be overwritten with 0xFF.
 * \param len The length of the buffer.
 */
static uint8_t get_register(radio_register_t reg, uint8_t* buffer, uint8_t len)
{
	uint8_t status, i;
	for (i = 0; i < len; i++)
	{
		// If the buffer is too long for the register results, then the radio will interpret the extra bytes as instructions.
		// To remove the risk, we set the buffer elements to NOP instructions.
		buffer[i] = 0xFF;
	}
	CSN_LOW();

	status = DRV_Spi_RW_Byte(LIB_nRF24l01_Data.hSpi,R_REGISTER | (REGISTER_MASK & reg));
	DRV_Spi_Read_Buffer(LIB_nRF24l01_Data.hSpi,buffer, len);

	CSN_HIGH();

	return status;
}


/**
 * Send an instruction to the nRF24L01.
 * \param instruction The instruction to send (see the bottom of nRF24L01.h)
 * \param data An array of argument data to the instruction.  If len is 0, then this may be NULL.
 * \param buffer An array for the instruction's return data.  This can be NULL if the instruction has no output.
 * \param len The length of the data and buffer arrays.
 */
static void send_instruction(uint8_t instruction, uint8_t* data, uint8_t* buffer, uint8_t len)
{
    CSN_LOW();
	// send the instruction
    DRV_Spi_RW_Byte(LIB_nRF24l01_Data.hSpi,instruction);
	// pass in args
	if (len > 0)
	{
		if (buffer == NULL)	//
			DRV_Spi_Write_Buffer(LIB_nRF24l01_Data.hSpi,data, len);
		else
		{

			/* FIXME: check if write is needed?*/
			DRV_Spi_Read_Buffer(LIB_nRF24l01_Data.hSpi,buffer, len);
		}
	}
    // resynch SPI
    CSN_HIGH();
}

/**
 * Switch the radio to receive mode.  If the radio is already in receive mode, this does nothing.
 */
static void set_rx_mode( void )
{
	uint8_t config;
	get_register(CONFIG, &config, 1);
	if ((config & PRIM_RX) == 0)
	{
		config |= PRIM_RX;
		set_register(CONFIG, &config, 1);
		// the radio takes 130 us to power up the receiver.
		_delay_us(65);
		_delay_us(65);
	}
}


/**
 * Switch the radio to transmit mode.  If the radio is already in transmit mode, this does nothing.
 */
static void set_tx_mode( void )
{
	uint8_t config;
	get_register(CONFIG, &config, 1);
	if ((config & PRIM_RX) != 0)
	{
		config &= ~PRIM_RX;
		set_register(CONFIG, &config, 1);
		// The radio takes 130 us to power up the transmitter
		// You can delete this if you're sending large packets (I'm thinking > 25 bytes, but I'm not sure) because it
		// sending the bytes over SPI can take this long.
		_delay_us(65);
		_delay_us(65);
	}
}


/**
 * Reset the pipe 0 address if pipe 0 is enabled.  This is necessary when the radio is using Enhanced Shockburst, because
 * the pipe 0 address is set to the transmit address while the radio is transmitting (this is how the radio receives
 * auto-ack packets).
 */
static void reset_pipe0_address( void )
{
	if (LIB_nRF24l01_Data.rx_pipe_widths[RADIO_PIPE_0] != 0)
	{
		// reset the pipe 0 address if pipe 0 is enabled.
		set_register(RX_ADDR_P0, (uint8_t*)LIB_nRF24l01_Data.rx_pipe0_address, ADDRESS_LENGTH);
	}
}

/* disable all irq */
static void Disable_Irq( void )
{
	uint8_t config;

	get_register(CONFIG, &config, 1);
	config |= MASK_ALL_IRQ;
	set_register(CONFIG, &config, 1);
}

/* enable all irq */
static void Enable_Irq( void )
{
	uint8_t config;

	get_register(CONFIG, &config, 1);
	config &= ~MASK_ALL_IRQ;
	set_register(CONFIG, &config, 1);
}
/**
 * Configure radio defaults and turn on the radio in receive mode.
 * This configures the radio to its max-power, max-packet-header-length settings.  If you want to reduce power consumption
 * or increase on-air payload bandwidth, you'll have to change the config.
 */
static void configure_registers( void )
{
	uint8_t status ,value;


	status = get_status();

	// set address width to 5 bytes.
	value = ADDRESS_LENGTH - 2;			// 0b11 for 5 bytes, 0b10 for 4 bytes, 0b01 for 3 bytes
	set_register(SETUP_AW, &value, 1);

	// set Enhanced Shockburst retry to every 586 us, up to 5 times.  If packet collisions are a problem even with AA enabled,
	// then consider changing the retry delay to be different on the different stations so that they do not keep colliding on each retry.
	value = 0x15;
	//value = 0x10;
	set_register(SETUP_RETR, &value, 1);

	// Set to use 2.4 GHz channel 110.
	value = CHANNEL;
	set_register(RF_CH, &value, 1);

	// Set radio to 2 Mbps and high power.  Leave LNA_HCURR at its default.
	value = RF_DR | LNA_HCURR;
	set_register(RF_SETUP, &value, 1);

	// Enable 2-byte CRC and power up in receive mode.
	value = EN_CRC | CRCO | PWR_UP | PRIM_RX;
	set_register(CONFIG, &value, 1);

	// clear the interrupt flags in case the radio's still asserting an old unhandled interrupt
    value = RX_DR | TX_DS | MAX_RT;
    set_register(STATUS, &value, 1);

    // flush the FIFOs in case there are old data in them.
	send_instruction(FLUSH_TX, NULL, NULL, 0);
	send_instruction(FLUSH_RX, NULL, NULL, 0);

	status = get_status();

	get_register( OBSERVE_TX ,&value, 1);


	return;
}


void LIB_nRF24l01_Irq_CallBack( void )
{
	 uint8_t status;
	    uint8_t pipe_number;

	    CE_LOW();

	    status = get_status();

	    if (status & RX_DR)
	    {
	    	pipe_number =  (status & 0xE) >> 1;
	    	/* FIXME: add receive function*/
	    	//radio_rxhandler(pipe_number);
	    }
	    // We can get the TX_DS or the MAX_RT interrupt, but not both.
	    if (status & TX_DS)
	    {
	        // if there's nothing left to transmit, switch back to receive mode.
	    	LIB_nRF24l01_Data.ucTransmit_lock = 0;
	        reset_pipe0_address();
	        set_rx_mode();

	    	// indicate in the history that a packet was transmitted successfully by appending a 1.
	        LIB_nRF24l01_Data.tx_history <<= 1;
	        LIB_nRF24l01_Data.tx_history |= 1;

	        LIB_nRF24l01_Data.tx_last_status = RADIO_TX_SUCCESS;
	    }
	    else if (status & MAX_RT)
	    {
	        send_instruction(FLUSH_TX, NULL, NULL, 0);

	        LIB_nRF24l01_Data.ucTransmit_lock = 0;
	    	reset_pipe0_address();
			set_rx_mode();
	    	// indicate in the history that a packet was dropped by appending a 0.
			LIB_nRF24l01_Data.tx_history <<= 1;

			LIB_nRF24l01_Data.tx_last_status = RADIO_TX_MAX_RT;
	    }

	    // clear the interrupt flags.
		status = RX_DR | TX_DS | MAX_RT;
		set_register(STATUS, &status, 1);

	    CE_HIGH();
}


static void  Radio_Init( void )
{
	LIB_nRF24l01_Data.ucTransmit_lock= 0;
	// disable radio during config
	CE_LOW();

	// A 10.3 ms delay is required between power off and power on states (controlled by 3.3 V supply).
	mDRV_TIME_DELAY(11);

	// Configure the radio registers that are not application-dependent.
	configure_registers();

	// A 1.5 ms delay is required between power down and power up states (controlled by PWR_UP bit in CONFIG)
	mDRV_TIME_DELAY(2);

	// enable radio as a receiver
	CE_HIGH();
}


/*  FIXME: to be implemented*/
void _delay_us( unsigned int uiDelay)
{
	unsigned int uiIndex;
	volatile unsigned int uiDummy=0;

	for( uiIndex=0;uiIndex<(uiDelay*10);uiIndex++)
		uiDummy++;
}
