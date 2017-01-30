/*
 * Identification.c
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#include "Identification.h"
#include "IDPS_functions.h"
#include "Driver.h"

void Identification()
{
	InitializeUART();

	Delay(10000);
	TransmitSync();
	Delay(2000);

	StartIDPS();

	RequestMaxPayloadSize();

	//GetiPodOptions();
	GetiPodOptionsForLingo();

	SetFIDtokensFirst();
	SetFIDtokensSecond();
	SetFIDtokensThird();

	EndIDPS();
}

