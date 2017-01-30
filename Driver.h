/*
 * Driver.h
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#ifndef DRIVER_H_
#define DRIVER_H_

// UART Functions
void SetupUART(unsigned int baudRatio);
void TransactionIDcounter();
unsigned char ChecksumByte(unsigned int packetPtr);
unsigned int CommandPacket(unsigned char length, unsigned char command);
unsigned int CommandPayloadPacket(unsigned char length, unsigned char command, unsigned int payloadLength, unsigned char *payload);
unsigned int CertificateCommandPacket(unsigned char length, unsigned char command, unsigned char currentP, unsigned char maxP, unsigned char *certificateData);
unsigned int SignatureCommandPacket(unsigned char length, unsigned char command, 	unsigned int signatureLength, unsigned char *signatureData);
void ReadyToTransmit();
void TransmitSync();
void TransmitPacket(unsigned int numBytes);
void ProcessPacket();

// I2C Functions
void SetupI2C(unsigned char slaveAddr, unsigned int clk);
void I2Cstart();
void I2Cstop();
void SendRegisterAddress(unsigned char registerAddr);
void StartRead(unsigned int byteCount, unsigned char *ReadBuffer);
void StartWrite(unsigned int byteCount, unsigned char *WriteData);
void Delay(unsigned int time);


#endif /* DRIVER_H_ */
