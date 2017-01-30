/*
 * Authentication_functions.c
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#include "Authentication_functions.h"
#include "Driver.h"

unsigned char regAddress = 0x00;

unsigned char AuthenticationDataLength[2] = { 0 };
unsigned char authenticationBuffer[128] = { 0 };
unsigned char ProcControl[1] = { 0x01 };

unsigned int maximumPages = 0;

unsigned int challengeDataSent = 0;
unsigned int getAuthentication = 0;

void InitializeI2Cread()
{
	SetupI2C(0x11, 12);
	SetupUART(52);
	SendRegisterAddress(regAddress);
	I2Cstop();
	Delay(500);
}

void InitializeI2Cwrite()
{
	SetupI2C(0x11, 12);
	SendRegisterAddress(regAddress);
}

void GetAuthenticationCertificateLength()
{
	regAddress = 0x30;
	InitializeI2Cread();
	StartRead(2, AuthenticationDataLength);
	unsigned int numberBytes = CalculateDataLength();
	maximumPages = numberBytes / 128;

	getAuthentication = 1;
}

void GetAuthenticationInfo(unsigned int page_number)
{
	ClearPayloadPacketAuthentication();

	switch (page_number)
	{
		case 0:
			regAddress = 0x31;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 1:
			regAddress = 0x32;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 2:
			regAddress = 0x33;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 3:
			regAddress = 0x34;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 4:
			regAddress = 0x35;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 5:
			regAddress = 0x36;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 6:
			regAddress = 0x37;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 7:
			regAddress = 0x38;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 8:
			regAddress = 0x39;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;

		case 9:
			regAddress = 0x3A;
			InitializeI2Cread();
			StartRead(128, authenticationBuffer);
			break;
	}
}

void ReturnAuthenticationInfo(unsigned int currentPageAuth, unsigned int maxPageAuth)
{
	unsigned char length = 0x88;
	unsigned char commandID = 0x15;
	unsigned char currentPage = currentPageAuth & 0xFF;
	unsigned char maxPage = maxPageAuth & 0xFF;

	unsigned int packetLength = CertificateCommandPacket(length, commandID, currentPage, maxPage, authenticationBuffer);

	TransmitPacket(packetLength);
}

void SendChallengeData(unsigned char *challengeData)
{
	unsigned int cData = 0;
	unsigned int challengePtr = 0;
	for (cData=4; cData<24; cData++)
	{
		authenticationBuffer[challengePtr++] = challengeData[cData];
	}
	WriteChallengeData();
}

void WriteChallengeDataLength()
{
	regAddress = 0x20;
	InitializeI2Cwrite();

	AuthenticationDataLength[0] = 0x00;
	AuthenticationDataLength[1]	= 0x14;

	StartWrite(2, AuthenticationDataLength);
}

void WriteChallengeData()
{
	WriteChallengeDataLength();
	Delay(500);
	regAddress = 0x21;
	InitializeI2Cwrite();

	ReadyToTransmit();

	StartWrite(20, authenticationBuffer);
	challengeDataSent = 1;
}

void WriteAuthenticationControl()
{
	regAddress = 0x10;
	InitializeI2Cwrite();

	ProcControl[0] = 0x01;

	StartWrite(1, ProcControl);
}

void ReadSignatureDataLength()
{
	regAddress = 0x11;
	InitializeI2Cread();

	StartRead(2, AuthenticationDataLength);
}

void ReadSignatureData()
{
	ReadSignatureDataLength();
	regAddress = 0x12;
	InitializeI2Cread();

	StartRead(128, authenticationBuffer);
}

void ReturnSignatureData()
{
	unsigned char length = 0x84;
	unsigned char commandID = 0x18;

	unsigned int packetLength = SignatureCommandPacket(length, commandID, 128, authenticationBuffer);

	TransmitPacket(packetLength);
}

void ReadAuthenticationControl()
{
	regAddress = 0x10;
	InitializeI2Cread();

	StartRead(131, authenticationBuffer);
	CheckProcControl();
}

void CheckProcControl()
{
	if (ProcControl[0] != 0x10)
	{
		Delay(500);
		ReadAuthenticationControl();
	}
}

unsigned int WaitForGetAuthentication()
{
	while (!getAuthentication);
	getAuthentication = 0;
	TransmitSync();

	return maximumPages;
}

void WaitForChallengeDataToBeSent()
{
	while (!challengeDataSent);
	challengeDataSent = 0;
}

unsigned int CalculateDataLength()
{
	unsigned int numberBytes = (256 * AuthenticationDataLength[0]) + AuthenticationDataLength[1];

	return numberBytes;
}

void ClearPayloadPacketAuthentication()
{
	unsigned int payloadCtr = 0;
	for (payloadCtr=0; payloadCtr<128; payloadCtr++)
	{
		authenticationBuffer[payloadCtr] = 0x00;
	}
}
