/*
 * Authentication_functions.h
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#ifndef AUTHENTICATION_FUNCTIONS_H_
#define AUTHENTICATION_FUNCTIONS_H_

void InitializeI2Cread();
void InitializeI2Cwrite();
unsigned int WaitForGetAuthentication();
void GetAuthenticationCertificateLength();
void GetAuthenticationInfo();
void ReturnAuthenticationInfo();
void SendChallengeData(unsigned char *challengeData);
void WriteChallengeDataLength();
void WriteChallengeData();
void WaitForChallengeDataToBeSent();
void WriteAuthenticationControl();
void ReadAuthenticationControl();
void ReadSignatureDataLength();
void ReadSignatureData();
void ReturnSignatureData();
void CheckProcControl();
unsigned int CalculateDataLength();
void ClearPayloadPacketAuthentication();


#endif /* AUTHENTICATION_FUNCTIONS_H_ */
