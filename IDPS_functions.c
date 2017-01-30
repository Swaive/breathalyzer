/*
 * IDPS_functions.c
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#include "IDPS_functions.h"
#include "Driver.h"

unsigned char payloadPacket[63] = { 0 };

void InitializeUART()
{
	SetupUART(52);
}

void StartIDPS()
{
	unsigned char length = 0x04;
	unsigned char commandID = 0x38;
	unsigned int packetLength = CommandPacket(length, commandID);
	TransmitPacket(packetLength);
}

void RequestMaxPayloadSize()
{
	unsigned char length = 0x04;
	unsigned char commandID = 0x11;
	unsigned int packetLength = CommandPacket(length, commandID);
	TransmitPacket(packetLength);
}

void GetiPodOptionsForLingo()
{
	unsigned char length = 0x05;
	unsigned char commandID = 0x4B;

	unsigned int payloadLength = 1;
	ClearPayloadPacketIDPS();
	payloadPacket[0] = 0x00;

	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, payloadPacket);

	TransmitPacket(packetLength);
}

void GetiPodOptions()
{
	unsigned char length = 0x04;
	unsigned char commandID = 0x24;
	unsigned int packetLength = CommandPacket(length, commandID);
	TransmitPacket(packetLength);
}

void SetFIDtokensFirst()
{
	unsigned char length = 0x1D;
	unsigned char commandID = 0x39;
	unsigned int payloadLength = IdentifyAccCapsTokensPayload();
	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, payloadPacket);
	TransmitPacket(packetLength);
}

void SetFIDtokensSecond()
{
	unsigned char length = 0x3F;
	unsigned char commandID = 0x39;
	unsigned int payloadLength = AccInfoTokenPayload();
	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, payloadPacket);
	TransmitPacket(packetLength);
}

void SetFIDtokensThird()
{
	unsigned char length = 0x34;
	unsigned char commandID = 0x39;
	unsigned int payloadLength = EABundleSeedIDMetadataTokensPayload();
	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, payloadPacket);
	TransmitPacket(packetLength);
}

void EndIDPS()
{
	unsigned char length = 0x05;
	unsigned char commandID = 0x3B;
	unsigned int payloadLength = EndIDPSPayload();
	unsigned int packetLength = CommandPayloadPacket(length, commandID, payloadLength, payloadPacket);
	TransmitPacket(packetLength);
}

unsigned int IdentifyAccCapsTokensPayload()
{
	ClearPayloadPacketIDPS();
	unsigned int payloadPtr = 0;

	payloadPacket[payloadPtr++] = 0x02;			// Number of Lingoes
	payloadPacket[payloadPtr++] = 0x0C;			// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;			// FIDType
	payloadPacket[payloadPtr++] = 0x00;			// FIDSubType = IdentifyToken
	payloadPacket[payloadPtr++] = 0x01;			// Number of bytes in accessoryLingoes
	payloadPacket[payloadPtr++] = 0x00;			// General Lingo
	payloadPacket[payloadPtr++] = 0x00;			// Device options 31:24
	payloadPacket[payloadPtr++] = 0x00;			// Device options 23:16
	payloadPacket[payloadPtr++] = 0x00;			// Device options 15:08
	payloadPacket[payloadPtr++] = 0x02;			// Device options 07:00
	payloadPacket[payloadPtr++] = 0x00;			// Device ID 31:24
	payloadPacket[payloadPtr++] = 0x00;			// Device ID 23:16
	payloadPacket[payloadPtr++] = 0x02;			// Device ID 15:08
	payloadPacket[payloadPtr++] = 0x00;			// Device ID 07:00
	payloadPacket[payloadPtr++] = 0x0A;			// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;			// FIDType
	payloadPacket[payloadPtr++] = 0x01;			// FIDSubType = AccessoryCapsToken
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask
	payloadPacket[payloadPtr++] = 0x02;			// accCapsBitmask - Supports communication with app
	payloadPacket[payloadPtr++] = 0x00;			// accCapsBitmask

	return payloadPtr;
}

unsigned int AccInfoTokenPayload()
{
	ClearPayloadPacketIDPS();
	unsigned int payloadPtr = 0;

	payloadPacket[payloadPtr++] = 0x06;		// Number of Lingoes
	payloadPacket[payloadPtr++] = 0x10;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType = AccessoryInfoToken
	payloadPacket[payloadPtr++] = 0x01;		// accInfoType = Accessory Name
	payloadPacket[payloadPtr++] = 0x42;		// 'B'
	payloadPacket[payloadPtr++] = 0x72;		// 'r'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x74;		// 't'
	payloadPacket[payloadPtr++] = 0x68;		// 'h'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x6C;		// 'l'
	payloadPacket[payloadPtr++] = 0x79;		// 'y'
	payloadPacket[payloadPtr++] = 0x7A;		// 'z'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x72;		// 'r'
	payloadPacket[payloadPtr++] = 0x00;		// 'NULL'
	payloadPacket[payloadPtr++] = 0x06;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType = AccessoryInfoToken
	payloadPacket[payloadPtr++] = 0x04;		// accInfoType = Accessory Firmware Version
	payloadPacket[payloadPtr++] = 0x01;		// '1'
	payloadPacket[payloadPtr++] = 0x00;		// '0'
	payloadPacket[payloadPtr++] = 0x00;		// '0'
	payloadPacket[payloadPtr++] = 0x06;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType
	payloadPacket[payloadPtr++] = 0x05;		// accInfoType = Accessory hardware version
	payloadPacket[payloadPtr++] = 0x01;		// '1'
	payloadPacket[payloadPtr++] = 0x00;		// '0'
	payloadPacket[payloadPtr++] = 0x00;		// '0'
	payloadPacket[payloadPtr++] = 0x0A;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType = AccessoryInfoToken
	payloadPacket[payloadPtr++] = 0x06;		// accInfoType = Accessory manufacturer
	payloadPacket[payloadPtr++] = 0x53;		// 'S'
	payloadPacket[payloadPtr++] = 0x77;		// 'w'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x69;		// 'i'
	payloadPacket[payloadPtr++] = 0x76;		// 'v'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x00;		// 'NULL'
	payloadPacket[payloadPtr++] = 0x07;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType = AccessoryInfoToken
	payloadPacket[payloadPtr++] = 0x07;		// accInfoType = Accessory model number
	payloadPacket[payloadPtr++] = 0x31;		// '1'
	payloadPacket[payloadPtr++] = 0x2E;		// '.'
	payloadPacket[payloadPtr++] = 0x30;		// '0'
	payloadPacket[payloadPtr++] = 0x00;		// 'NULL'
	payloadPacket[payloadPtr++] = 0x07;		// Length in bytes of current Lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x02;		// FIDSubType = AccessoryInfoToken
	payloadPacket[payloadPtr++] = 0x0C;		// accInfoType = Accessory RF certifications
	payloadPacket[payloadPtr++] = 0x00;		// RFCertificationDeclaration 31:24
	payloadPacket[payloadPtr++] = 0x00;		// RFCertificationDeclaration 23:16
	payloadPacket[payloadPtr++] = 0x00;		// RFCertificationDeclaration 15:08
	payloadPacket[payloadPtr++] = 0x00;		// iPhone 4 (GSM & CDMA), 4S, 5 (A1428 & A1429)

	return payloadPtr;
}

unsigned int EABundleSeedIDMetadataTokensPayload()
{
	ClearPayloadPacketIDPS();
	unsigned int payloadPtr = 0;

	payloadPacket[payloadPtr++] = 0x03;		// Number of lingoes
	payloadPacket[payloadPtr++] = 0x1B;		// Length in bytes of current lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x04;		// FIDSubType = EAProtocolToken
	payloadPacket[payloadPtr++] = 0x01;		// protocolIndex = 1st protocol
	payloadPacket[payloadPtr++] = 0x63;		// 'c'
	payloadPacket[payloadPtr++] = 0x6F;		// 'o'
	payloadPacket[payloadPtr++] = 0x6D;		// 'm'
	payloadPacket[payloadPtr++] = 0x2E;		// '.'
	payloadPacket[payloadPtr++] = 0x73;		// 's'
	payloadPacket[payloadPtr++] = 0x77;		// 'w'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x69;		// 'i'
	payloadPacket[payloadPtr++] = 0x76;		// 'v'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x2E;		// '.'
	payloadPacket[payloadPtr++] = 0x62;		// 'b'
	payloadPacket[payloadPtr++] = 0x72;		// 'r'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x74;		// 't'
	payloadPacket[payloadPtr++] = 0x68;		// 'h'
	payloadPacket[payloadPtr++] = 0x61;		// 'a'
	payloadPacket[payloadPtr++] = 0x6C;		// 'l'
	payloadPacket[payloadPtr++] = 0x79;		// 'y'
	payloadPacket[payloadPtr++] = 0x7A;		// 'z'
	payloadPacket[payloadPtr++] = 0x65;		// 'e'
	payloadPacket[payloadPtr++] = 0x72;		// 'r'
	payloadPacket[payloadPtr++] = 0x00;		// 'NULL'
	payloadPacket[payloadPtr++] = 0x0D;		// Length in bytes of current lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x05;		// FIDSubType = BundleSeedIDPrefToken
	payloadPacket[payloadPtr++] = 0x41;		// 'A'
	payloadPacket[payloadPtr++] = 0x31;		// '1'
	payloadPacket[payloadPtr++] = 0x42;		// 'B'
	payloadPacket[payloadPtr++] = 0x32;		// '2'
	payloadPacket[payloadPtr++] = 0x43;		// 'C'
	payloadPacket[payloadPtr++] = 0x33;		// '3'
	payloadPacket[payloadPtr++] = 0x44;		// 'D'
	payloadPacket[payloadPtr++] = 0x34;		// '4'
	payloadPacket[payloadPtr++] = 0x45;		// 'E'
	payloadPacket[payloadPtr++] = 0x35;		// '5'
	payloadPacket[payloadPtr++] = 0x00;		// 'NULL'
	payloadPacket[payloadPtr++] = 0x04;		// Length in bytes of current lingo
	payloadPacket[payloadPtr++] = 0x00;		// FIDType
	payloadPacket[payloadPtr++] = 0x08;		// FIDSubType = EAProtocolMetadataToken
	payloadPacket[payloadPtr++] = 0x01;		// Protocol index number
	payloadPacket[payloadPtr++] = 0x01;		// "Find App for this Accessory"

	return payloadPtr;
}

unsigned int EndIDPSPayload()
{
	ClearPayloadPacketIDPS();
	unsigned int payloadPtr = 0;

	payloadPacket[payloadPtr++] = 0x00;		// accEndIDPSStatus = continue with authentication

	return payloadPtr;
}

void ClearPayloadPacketIDPS()
{
	unsigned int payloadCtr = 0;
	for (payloadCtr=0; payloadCtr<60; payloadCtr++)
	{
		payloadPacket[payloadCtr] = 0x00;
	}
}
