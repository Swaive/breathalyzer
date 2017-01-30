/*
 * Authentication.c
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#include "Identification.h"
#include "Authentication.h"
#include "Authentication_functions.h"
#include "Driver.h"

unsigned int maxPages = 0;
unsigned int currentPage = 0;

void Authentication()
{
	Identification();

	maxPages = WaitForGetAuthentication();

	GetandReturnCertificate();

	WaitForChallengeDataToBeSent();

	Delay(500);
	WriteAuthenticationControl();
	Delay(50000);

	ReadSignatureData();
	Delay(1000);

	ReturnSignatureData();
}

void GetandReturnCertificate()
{
	for (currentPage=0; currentPage<=maxPages; currentPage++)
	{
		GetAuthenticationInfo(currentPage);
		Delay(1000);
		ReturnAuthenticationInfo(currentPage, maxPages);
	}
}
