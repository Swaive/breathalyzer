/*
 * Driver.c
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#include <msp430.h>
#include "Driver.h"
#include "Authentication_functions.h"
#include "Application_functions.h"

// UART Variables
typedef enum
{
	UART_RX_IDLE = 0,								// Idle state
	UART_RX_TRANSPORT,								// 0xFF state
	UART_RX_START,									// 0x55 state
	UART_RX_DATA									// Length and data bytes
} UART_RX_MODE;										// UART packet receiver mode

UART_RX_MODE uartRxState = UART_RX_IDLE;

#define UART_TRANSPORT 0xFF							// Start byte of packet
#define UART_START 0x55								// Start byte of packet
#define UART_LINGO 0x00								// Lingo ID of packet
#define AUTH_MAJOR 0x02								// Authorization ID major
#define AUTH_MINOR 0x00								// Authorization ID minor

unsigned char transIDupper = 0x00;					// Transaction ID (upper byte)
unsigned char transIDlower = 0x00;					// Transaction ID (lower byte)
unsigned char transIDupperAcc = 0x00;				// Transaction ID (upper byte), from accessory
unsigned char transIDlowerAcc = 0x00;				// Transaction ID (lower byte), from accessory
unsigned char transIDupperiPod = 0x00;				// Transaction ID (upper byte), from the iPod
unsigned char transIDloweriPod = 0x00;				// Transaction ID (lower byte), from the iPod
unsigned int transactionID = 0;						// Transaction ID (integer)
unsigned int transIDenable = 1;						// Determines whether transaction ID needs incrementing
unsigned int transmitReady = 1;						// Determines whether iPod is ready to receive

unsigned char iPodSessionID[2] = { 0 };				// Session ID for data transfers

unsigned char UartBuffer[140];						// UART transmit buffer
unsigned char *pUartRxBuffer;						// Pointer to RX buffer
unsigned int receiveCounter = 0;					// Receive counter
unsigned int rxCount = 0;							// Receive packet counter

//I2C Variables
unsigned char slaveAddress;							// I2C slave address
unsigned char registerAddress;						// I2C register address

unsigned char *pI2CTxBuffer;						// Pointer to I2C TX buffer
unsigned int I2CTxByteCtr = 0;						// I2C TX byte counter
unsigned char *pI2CRxBuffer;						// Pointer to I2C RX buffer
unsigned int I2CRxByteCtr = 0;						// I2C RX byte counter
unsigned int I2CRXenable = 0;						// '0' = I2C transmit; '1' = I2C receive

/*-----------------------------------------------------------------------------------
 * void SetupI2C(unsigned char slaveAddr, unsigned int clk)
 *
 * This function intializes the I2C communication.
 *
 * IN:	unsigned char slaveAddress	=>	the slave address of the I2C slave
 * 		unsigned int clk			=>	ratio to set the frequency
 *
 * ----------------------------------------------------------------------------------
 */
void SetupI2C(unsigned char slaveAddr, unsigned int clk)
{
	slaveAddress = slaveAddr;						// Assign slave address

	P1SEL |= BIT6 + BIT7;							// Set pin 1.6 -> SDA
	P1SEL2 |= BIT6 + BIT7;							// Set pin 1.7 -> SCL
	UCB0CTL1 |= UCSWRST;							// Hold in reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;			// Master, I2C mode, synchronous
	UCB0CTL1 = UCSSEL_2 + UCSWRST;					// SMCLK, hold in reset
	UCB0BR0 = clk;									// fSCL = SMCL/UCB0BR0
	UCB0BR1 = 0;									// fSCL (lower byte)
	UCB0I2COA = 0;									// Initiate own address
	UCB0I2CSA = 0;									// Initiate slave address
	UCB0CTL1 &= ~UCSWRST;							// Resume operation
	UCB0I2CIE |= UCNACKIE;							// Enable NACK interrupt
	__bis_SR_register(GIE);
}

/*-----------------------------------------------------------------------------------
 * void I2Cstart()
 *
 * This function sends the start sequence.
 *
 * IN:	NONE
 *
 * ----------------------------------------------------------------------------------
 */
void I2Cstart()
{
	UCB0CTL1 |= UCTXSTT;					// I2C start condition
}

/*-----------------------------------------------------------------------------------
 * void I2Cstop()
 *
 * This function sends the stop sequence.
 *
 * IN:	NONE
 *
 * ----------------------------------------------------------------------------------
 */
void I2Cstop()
{
	UCB0CTL1 |= UCTXSTP;					// I2C stop condition
}

/*-----------------------------------------------------------------------------------
 * void SendRegisterAddress(unsigned char registerAddr)
 *
 * This function begins the communication process for I2C. Using the slave address
 * already passed in, this function starts the communication between the master and
 * slave and transmits the register address (data).
 *
 * IN:	unsigned char registerAddr	=>	char containing register address
 *
 * ----------------------------------------------------------------------------------
 */
void SendRegisterAddress(unsigned char registerAddr)
{
	registerAddress = registerAddr;			// Assign the register address
	UCB0CTL1 |= UCTR;						// Enable transmit communication

	UCB0I2CSA = slaveAddress;				// Pass slave address to I2CSA
	I2Cstart();								// Send the I2C start sequence
	UCB0TXBUF = registerAddress;			// Pass register address to TX buffer

	while (!(IFG2 & UCB0TXIFG));
}

/*-----------------------------------------------------------------------------------
 * void StartRead(unsigned int byteCount, unsigned char *ReadBuffer)
 *
 * This function initiates the read process. Using the slave address already passed
 * in. Sends the data read in from the buffer to ReadBuffer.
 *
 * IN:	unsigned int byteCount		=> number of bytes to read
 * 		unsigned char *ReadBuffer	=> pointer to read data destination
 *
 * ----------------------------------------------------------------------------------
 */
void StartRead(unsigned int byteCount, unsigned char *ReadBuffer)
{
	I2CRXenable = 1;						// Enable receive
	I2CRxByteCtr = byteCount;				// Update RX byte counter
	UCB0I2CSA = slaveAddress;				// Pass slave address to I2CSA
	UCB0CTL1 &= ~UCTR;						// Enable receive communication
	pI2CRxBuffer = ReadBuffer;				// Start of RX buffer

	while (UCB0CTL1 & UCTXSTP);
	I2Cstart();								// Send the I2C start sequence

	IE2 |= UCB0RXIE;						// Enable RX interrupt
	__bis_SR_register(CPUOFF + GIE);		// Enter LPM0 and general interrupt enable
}

/*-----------------------------------------------------------------------------------
 * void StartWrite(unsigned int byteCount, unsigned char *WriteData)
 *
 * This function initiates the write process. It transmits the number
 * of bytes declared in byteCount from WriteData to the buffer.
 *
 * IN:	unsigned int byteCount 		=> number of bytes to transmit
 * 		unsigned char *WriteData	=> data to write to the buffer
 *
 * ----------------------------------------------------------------------------------
 */
void StartWrite(unsigned int byteCount, unsigned char *WriteData)
{
	I2CRXenable = 0;						// Enable transmit
	I2CTxByteCtr = byteCount;				// Update TX byte counter
	UCB0I2CSA = slaveAddress;				// Pass slave address to I2CSA
	pI2CTxBuffer = WriteData;				// Start of TX buffer

	IE2 |= UCB0TXIE;						// Enable RX interrupt
	__bis_SR_register(CPUOFF + GIE);		// Enter LPM0 and general interrupt enable
}

/*-----------------------------------------------------------------------------------
 * void SetupUART(unsigned int baudRatio)
 *
 * This function initializes the UART communication depending on the incoming integer
 * (baudRatio).
 *
 * IN:	unsigned int baudRatio	=>	ratio used to set the baud rate
 *
 * ----------------------------------------------------------------------------------
 */
void SetupUART(unsigned int baudRatio)
{
	BCSCTL1 = CALBC1_1MHZ;							// Set DCO
	DCOCTL 	= CALDCO_1MHZ;
	P1SEL 	|= BIT1 + BIT2;							// P1.1=RXD
	P1SEL2	|= BIT1 + BIT2;							// P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;							// SMCLK
	UCA0BR0 = baudRatio;							// Set BaudRate (52=19200) (17=57600)
	UCA0BR1 = 0;
	UCA0CTL1 &= ~UCSWRST;							// Initialize USCI state machine

	IE2 |= UCA0RXIE;								// Enable USCI_A0 RX interrupt

	pUartRxBuffer = UartBuffer;						// Point to receive buffer
	receiveCounter = 0;								// Initialize receive counter
	uartRxState = UART_RX_IDLE;						// Initialize RX state
}

/*-----------------------------------------------------------------------------------
 * void TransactionIDcounter()
 *
 * This function sets the transaction ID depending on transIDenable.
 *
 * NONE
 *
 * ----------------------------------------------------------------------------------
 */
void TransactionIDcounter()
{
	if (transIDenable)
	{
		transactionID = (256 * transIDupperAcc) + transIDlowerAcc;
		++transactionID;
		transIDupperAcc = transactionID / 0xFF;
		transIDlowerAcc = transactionID & 0xFF;
		transIDupper = transIDupperAcc;
		transIDlower = transIDlowerAcc;
	}
	else
	{
		transIDupper = transIDupperiPod;
		transIDlower = transIDloweriPod;
	}
}

/*-----------------------------------------------------------------------------------
 * unsigned char ChecksumByte(unsigned int packetPtr)
 *
 * This function calculates the checksum byte of the packet.
 *
 * IN:	unsigned int packetPtr	=>	contains the last byte to include in the checksum
 * ----------------------------------------------------------------------------------
 */
unsigned char ChecksumByte(unsigned int packetPtr)
{
	unsigned int ctr = 0;							// Packet counter
	unsigned char sum = 0x00;						// Checksum counter
	unsigned char checksum = 0x00;					// Checksum byte

	for (ctr=2; ctr<packetPtr; ctr++)
	{
		sum = sum + UartBuffer[ctr];
	}

	checksum = (0x00 - sum) & 0xFF;					// Calculate checksum byte

	return checksum;
}

/*-----------------------------------------------------------------------------------
 * void CommandPacket(unsigned char length, unsigned char command)
 *
 * This function receives the command packet and passes it onto the UartBuffer
 * array.
 *
 * IN:	unsigned char length		=>	length of the packet
 * 		unsigned char command		=>	command ID
 *
 * ----------------------------------------------------------------------------------
 */
unsigned int CommandPacket(unsigned char length, unsigned char command)
{
	unsigned int packetPtr = 0;						// Packet pointer

	UartBuffer[packetPtr++] = UART_TRANSPORT;		// '0xFF'
	UartBuffer[packetPtr++] = UART_START;			// '0x55'
	UartBuffer[packetPtr++] = length;				// Length of packet in bytes
	UartBuffer[packetPtr++] = UART_LINGO;			// Lingo ID
	UartBuffer[packetPtr++] = command;				// Command ID

	TransactionIDcounter();

	UartBuffer[packetPtr++] = transIDupper;			// Transaction ID (upper)
	UartBuffer[packetPtr++] = transIDlower;			// Transaction ID (lower)

	unsigned char checksum = ChecksumByte(packetPtr);
	UartBuffer[packetPtr++] = checksum;				// Checksum byte

	return packetPtr;
}

/*-----------------------------------------------------------------------------------
 * unsigned int CommandPayloadPacket(unsigned char length, unsigned char command,
 * 		unsigned int payloadLength, unsigned char *payload)
 *
 * This function receives the command packet when the specific command requires a
 * payload. The payload along with the length of the payload is passed in.
 *
 * IN:	unsigned char length		=>	length of the packet
 * 		unsigned char command		=>	command ID
 * 		unsigned int payloadLength	=>	length of the payload
 * 		unsigned char *payload		=>	pointer to the payload data
 *
 * RETURN:	length of this packet
 *
 * ----------------------------------------------------------------------------------
 */
unsigned int CommandPayloadPacket(unsigned char length, unsigned char command,
		unsigned int payloadLength, unsigned char *payload)
{
	unsigned int packetPtr = 0;
	unsigned int payloadPtr = 0;

	UartBuffer[packetPtr++] = UART_TRANSPORT;		// '0xFF'
	UartBuffer[packetPtr++] = UART_START;			// '0x55'
	UartBuffer[packetPtr++] = length;				// Length of packet in bytes
	UartBuffer[packetPtr++] = UART_LINGO;			// Lingo ID
	UartBuffer[packetPtr++] = command;				// Command ID

	TransactionIDcounter();

	UartBuffer[packetPtr++] = transIDupper;			// Load transaction ID
	UartBuffer[packetPtr++] = transIDlower;			// Load transaction ID

	for (payloadPtr=0; payloadPtr<payloadLength; payloadPtr++)
	{
		UartBuffer[packetPtr++] = payload[payloadPtr];
	}

	unsigned char checksum = ChecksumByte(packetPtr);
	UartBuffer[packetPtr++] = checksum;				// Checksum byte

	return packetPtr;
}

/*-----------------------------------------------------------------------------------
 * unsigned char CertificateCommandPacket(unsigned char length, unsigned char command,
 * 		unsigned char currentP, unsigned char maxP, unsigned char *certificateData)
 *
 * This function receives the command packet during the certification process. It
 * passes through all of the required bytes for each of the certification pages.
 *
 * IN:	unsigned char length			=>	length of the packet
 * 		unsigned char command			=>	command ID
 * 		unsigned char currentP			=>	current certificate page
 * 		unsigned char maxP				=>	maximum certificate pages
 * 		unsigned char *certificateData	=>	pointer to the certificate data
 *
 * RETURN:	length of this packet
 *
 * ----------------------------------------------------------------------------------
 */
unsigned int CertificateCommandPacket(unsigned char length, unsigned char command,
		unsigned char currentP, unsigned char maxP, unsigned char *certificateData)
{
	unsigned char packetPtr = 0;
	unsigned char payloadPtr = 0;

	UartBuffer[packetPtr++] = UART_TRANSPORT;		// '0xFF'
	UartBuffer[packetPtr++] = UART_START;			// '0x55'
	UartBuffer[packetPtr++] = length;				// Length of packet in bytes
	UartBuffer[packetPtr++] = UART_LINGO;			// Lingo ID
	UartBuffer[packetPtr++] = command;				// Command ID

	//TransactionIDcounter();

	UartBuffer[packetPtr++] = transIDupperiPod;		// Load transaction ID (upper)
	UartBuffer[packetPtr++] = transIDloweriPod;		// Load transaction ID (lower)

	UartBuffer[packetPtr++] = AUTH_MAJOR;			// Authentication major version
	UartBuffer[packetPtr++] = AUTH_MINOR;			// Authentication minor version
	UartBuffer[packetPtr++] = currentP;				// Current certificate page
	UartBuffer[packetPtr++] = maxP;					// Maximum number of certificate pages

	for (payloadPtr=0; payloadPtr<128; payloadPtr++)
	{
		UartBuffer[packetPtr++] = certificateData[payloadPtr];
	}

	unsigned char checksum = ChecksumByte(packetPtr);
	UartBuffer[packetPtr++] = checksum;				// Checksum byte

	return packetPtr;
}

/*-----------------------------------------------------------------------------------
 * unsigned int SignatureCommandPacket(unsigned char length, unsigned char
 * 		command, unsigned int signatureLength, unsigned char *signatureData)
 *
 * This function receives and passes the signature data to the UartBuffer.
 *
 * IN:	unsigned char length			=>	length of the packet
 * 		unsigned char command			=>	command ID
 * 		unsigned int signatureLength	=>	length of the signature data
 * 		unsigned char *signatureData	=>	pointer to the signature data
 *
 * RETURN:	length of this packet
 *
 * ----------------------------------------------------------------------------------
 */
unsigned int SignatureCommandPacket(unsigned char length, unsigned char command,
		unsigned int signatureLength, unsigned char *signatureData)
{
	unsigned int packetPtr = 0;
	unsigned int payloadPtr = 0;

	UartBuffer[packetPtr++] = UART_TRANSPORT;		// '0xFF'
	UartBuffer[packetPtr++] = UART_START;			// '0x55'
	UartBuffer[packetPtr++] = length;				// Length of packet in bytes
	UartBuffer[packetPtr++] = UART_LINGO;			// Lingo ID
	UartBuffer[packetPtr++] = command;				// Command ID

	//TransactionIDcounter();

	UartBuffer[packetPtr++] = transIDupperiPod;		// Load transaction ID (upper)
	UartBuffer[packetPtr++] = transIDloweriPod;		// Load transaction ID (lower)

	for (payloadPtr=0; payloadPtr<signatureLength; payloadPtr++)
	{
		UartBuffer[packetPtr++] = signatureData[payloadPtr];
	}

	unsigned char checksum = ChecksumByte(packetPtr);
	UartBuffer[packetPtr++] = checksum;				// Checksum byte

	return packetPtr;
}

unsigned int AccessoryDataTransferCommandPacket(unsigned char length, unsigned int dataLength, unsigned char *appData)
{
	unsigned int packetPtr = 0;
	unsigned int payloadPtr = 0;

	UartBuffer[packetPtr++] = UART_TRANSPORT;		// '0xFF'
	UartBuffer[packetPtr++] = UART_START;			// '0x55'
	UartBuffer[packetPtr++] = length;				// Length of packet in bytes
	UartBuffer[packetPtr++] = UART_LINGO;			// Lingo ID
	UartBuffer[packetPtr++] = 0x42;					// Command ID = AccessoryDataTransfer

	TransactionIDcounter();

	UartBuffer[packetPtr++] = transIDupper;			// Load transaction ID (upper)
	UartBuffer[packetPtr++] = transIDlower;			// Load transaction ID (lower)

	UartBuffer[packetPtr++] = iPodSessionID[0];		// Load session ID (upper)
	UartBuffer[packetPtr++] = iPodSessionID[1];		// Load session ID (lower)

	for (payloadPtr=0; payloadPtr<dataLength; payloadPtr++)
	{
		UartBuffer[packetPtr++] = appData[payloadPtr];
	}

	unsigned char checksum = ChecksumByte(packetPtr);
	UartBuffer[packetPtr++] = checksum;				// Checksum byte

	return packetPtr;
}

/*-----------------------------------------------------------------------------------
 * void ReadyToTransmit()
 *
 * This function determines whether the iPod is ready to receive packets.
 *
 * NONE
 *
 * ----------------------------------------------------------------------------------
 */
void ReadyToTransmit()
{
	while (!transmitReady);							// Wait until iPod is ready to receive
}

/*-----------------------------------------------------------------------------------
 * void TransmitSync()
 *
 * This function transmits a sync bytes to the iPod. (0xFF)
 *
 * NONE
 *
 * ----------------------------------------------------------------------------------
 */
void TransmitSync()
{
	while (!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = UART_TRANSPORT;						// Transmit '0xFF'
}


/*-----------------------------------------------------------------------------------
 * void TransmitPacket(unsigned int numBytes)
 *
 * This function transmits the array UartBuffer to the iPod.
 *
 * IN:	unsigned int numBytes	=>	the number of bytes to transmit
 *
 * ----------------------------------------------------------------------------------
 */
void TransmitPacket(unsigned int numBytes)
{
	ReadyToTransmit();								// Wait until iPod is ready to receive

	unsigned int ctr;								// Packet counter
	for (ctr=0; ctr<numBytes; ctr++)
	{
		while (!(IFG2 & UCA0TXIFG));
		UCA0TXBUF = UartBuffer[ctr];				// Transmit packet, byte by byte
	}

	transmitReady = 0;								// Disable transmit until iPod is ready
	__bis_SR_register(CPUOFF + GIE);				// LMP0 + general interrupt enable
}

/*-----------------------------------------------------------------------------------
 * void ProcessPacket(unsigned int packetLength)
 *
 * This function processes the incoming packet from the iPod. Determines whether or
 * not the iPod is ready to receive an incoming transaction depending on incoming
 * command ID.
 *
 * IN:	unsigned int packetLength	=>	the length of the packet
 *
 * ----------------------------------------------------------------------------------
 */
void ProcessPacket()
{
	rxCount = 0;

	if (pUartRxBuffer[3] == transIDlower)
	{
		transIDenable = 1;							// Increment transaction ID
	}
	else
	{
		transIDenable = 0;
		if (pUartRxBuffer[1] == 0x14)
		{
			transIDupper = pUartRxBuffer[2];
			transIDlower = pUartRxBuffer[3];
		}
		else
		{
			transIDupperiPod = pUartRxBuffer[2];
			transIDloweriPod = pUartRxBuffer[3];
		}
	}

	if (pUartRxBuffer[1] == 0x3F)
	{
		unsigned char iPodCommandID = 0x3F;
		iPodSessionID[0] = pUartRxBuffer[4];
		iPodSessionID[1] = pUartRxBuffer[5];
		SendAccessoryAck(iPodCommandID);
		OpenDataSession();
	}
	if (pUartRxBuffer[1] == 0x40)
	{
		unsigned char iPodCommandID = 0x40;
		iPodSessionID[0] = pUartRxBuffer[4];
		iPodSessionID[1] = pUartRxBuffer[5];
		SendAccessoryAck(iPodCommandID);
	}
	if (pUartRxBuffer[1] == 0x43)
	{
		unsigned char iPodCommandID = 0x43;
		iPodSessionID[0] = pUartRxBuffer[4];
		iPodSessionID[1] = pUartRxBuffer[5];
		SendAccessoryAck(iPodCommandID);
		iPodDataTransfer(pUartRxBuffer);
	}

	if (pUartRxBuffer[1] == 0x16)
	{
		transmitReady = 0;							// Don't transmit, wait for response
	}
	if (pUartRxBuffer[1] == 0x3C)
	{
		transmitReady = 0;
	}
	if (pUartRxBuffer[1] == 0x14)
	{
		GetAuthenticationCertificateLength();
		transmitReady = 1;
	}
	if (pUartRxBuffer[1] == 0x17)
	{
		TransmitSync();
		SendChallengeData(UartBuffer);				// Pass challenge bytes
		transmitReady = 1;
	}
	else
	{
		transmitReady = 1;
	}
}


/*-----------------------------------------------------------------------------------
 * void Delay(unsigned int time)
 *
 * This function causes a delay.
 *
 * IN:	unsigned int time => time used for delay
 *
 * ----------------------------------------------------------------------------------
 */
void Delay(unsigned int time)
{
	volatile int i=0;
	for (i=0; i<time; i++);
}

// USCI A0/B0 Receive ISR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	if (UCB0STAT & UCNACKIFG)
	{
		I2Cstop();
		UCB0STAT &= ~UCNACKIFG;
		Delay(50);
		SendRegisterAddress(registerAddress);
	}

	unsigned char buffer = 0x00;
	switch (uartRxState)
	{
		case UART_RX_IDLE:
			buffer = UCA0RXBUF;
			if (buffer == UART_TRANSPORT) {
				uartRxState = UART_RX_TRANSPORT;
			}
			break;

		case UART_RX_TRANSPORT:
			buffer = UCA0RXBUF;
			if (buffer == UART_START) {
				uartRxState = UART_RX_START;
			}
			break;

		case UART_RX_START:
			/*
			 * Read length byte to determine number of incoming bytes
			 * for this packet.  +1 for the checksum byte
			 */
			receiveCounter = UCA0RXBUF + 1;
			//currentPacketSize = receiveCounter;
			uartRxState = UART_RX_DATA;
			break;

		case UART_RX_DATA:
			pUartRxBuffer[rxCount++] = UCA0RXBUF;
			--receiveCounter;
			if (receiveCounter == 0) {
				uartRxState = UART_RX_IDLE;
				__bic_SR_register_on_exit(CPUOFF);			// Exit LPM0
				ProcessPacket();
			}
			break;

		default:
			break;
	}

}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if (I2CRXenable == 1)
	{
		I2CRxByteCtr--;							// Decrement RX byte counter
		if (I2CRxByteCtr)
		{
			*pI2CRxBuffer++ = UCB0RXBUF;		// Read RX data from the buffer

			if (I2CRxByteCtr == 1)				// If only one byte left to read
			{
				I2Cstop();						// Send the I2C stop sequence
			}
		}
		else
		{
			*pI2CRxBuffer = UCB0RXBUF;			// Read the last byte
			__bic_SR_register_on_exit(CPUOFF);
		}
	}
	else
	{
		if (I2CTxByteCtr)
		{
			UCB0TXBUF = *pI2CTxBuffer++;
			I2CTxByteCtr--;
		}
		else
		{
			UCB0TXBUF = *pI2CTxBuffer;
			I2Cstop();
			IFG2 &= ~UCB0TXIFG;
			__bic_SR_register_on_exit(CPUOFF);
		}
	}
}
