/*
 * Application_functions.c
 *
 *  Created on: Feb 16, 2013
 *      Author: gURpaL
 */

#include "Application_functions.h"
#include "Driver.h"

unsigned char applicationPayload[40] = { 0 };

unsigned int dataSessionIsOpen = 0;
unsigned int transferDataReceived = 0;

void ApplicationLaunch()
{
	unsigned int payloadLength = ApplicationLaunchPayload();
	unsigned char length = payloadLength + 0x04;
	unsigned char command = 0x64;

	unsigned int packetLength = CommandPayloadPacket(length, command, payloadLength, applicationPayload);

	TransmitPacket(packetLength);
}

void OpenDataSession()
{
	dataSessionIsOpen = 1;
}

void iPodDataTransfer(unsigned char *transferData)
{
	transferDataReceived = 1;
	/*
	 * App protocol
	 */

}

void DataTransferEnableStartButton()
{

}

void DataTrasnferSendResults()
{

}


unsigned int ApplicationLaunchPayload()
{
	ClearApplicationPayloadPacket();
	unsigned int payloadPtr = 0;

	applicationPayload[payloadPtr++] = 0x00;		// Reserved
	applicationPayload[payloadPtr++] = 0x02;		// Reserved
	applicationPayload[payloadPtr++] = 0x00;		// Reserved
	applicationPayload[payloadPtr++] = 0x63;		// 'c'
	applicationPayload[payloadPtr++] = 0x6F;		// 'o'
	applicationPayload[payloadPtr++] = 0x6D;		// 'm'
	applicationPayload[payloadPtr++] = 0x2E;		// '.'
	applicationPayload[payloadPtr++] = 0x73;		// 's'
	applicationPayload[payloadPtr++] = 0x77;		// 'w'
	applicationPayload[payloadPtr++] = 0x61;		// 'a'
	applicationPayload[payloadPtr++] = 0x69;		// 'i'
	applicationPayload[payloadPtr++] = 0x76;		// 'v'
	applicationPayload[payloadPtr++] = 0x65;		// 'e'
	applicationPayload[payloadPtr++] = 0x2E;		// '.'
	applicationPayload[payloadPtr++] = 0x62;		// 'b'
	applicationPayload[payloadPtr++] = 0x72;		// 'r'
	applicationPayload[payloadPtr++] = 0x65;		// 'e'
	applicationPayload[payloadPtr++] = 0x61;		// 'a'
	applicationPayload[payloadPtr++] = 0x74;		// 't'
	applicationPayload[payloadPtr++] = 0x68;		// 'h'
	applicationPayload[payloadPtr++] = 0x61;		// 'a'
	applicationPayload[payloadPtr++] = 0x6C;		// 'l'
	applicationPayload[payloadPtr++] = 0x79;		// 'a'
	applicationPayload[payloadPtr++] = 0x7A;		// 'z'
	applicationPayload[payloadPtr++] = 0x65;		// 'e'
	applicationPayload[payloadPtr++] = 0x72;		// 'r'
	applicationPayload[payloadPtr++] = 0x00;		// 'NULL'

	return payloadPtr;
}

void SendAccessoryAck(unsigned char iPodCommandID)
{
	unsigned int payloadLength = 0;

	ClearApplicationPayloadPacket();
	applicationPayload[payloadLength++] = 0x00;
	applicationPayload[payloadLength++] = iPodCommandID;

	unsigned char length = payloadLength + 0x04;
	unsigned char commandID = 0x41;

	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, applicationPayload);

	TransmitPacket(packetLength);
}

void WaitForOpenDataSession()
{
	while (!dataSessionIsOpen);

	dataSessionIsOpen = 0;
}

void WaitForiPodDataTransfer()
{
	while (!transferDataReceived);

	transferDataReceived = 0;
}

void ClearApplicationPayloadPacket()
{
	int payloadPtr = 0;

	for (payloadPtr=0; payloadPtr<50; payloadPtr++)
	{
		applicationPayload[payloadPtr] = 0x00;
	}
}

