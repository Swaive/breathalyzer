/*
 * IDPS_functions.h
 *
 *  Created on: Feb 12, 2013
 *      Author: gURpaL
 */

#ifndef IDPS_FUNCTIONS_H_
#define IDPS_FUNCTIONS_H_

void InitializeUART();
void StartIDPS();
void RequestMaxPayloadSize();
void GetiPodOptions();
void GetiPodOptionsForLingo();
void SetFIDtokensFirst();
void SetFIDtokensSecond();
void SetFIDtokensThird();
void EndIDPS();
unsigned int IdentifyAccCapsTokensPayload();
unsigned int AccInfoTokenPayload();
unsigned int EABundleSeedIDMetadataTokensPayload();
unsigned int EndIDPSPayload();
void ClearPayloadPacketIDPS();

#endif /* IDPS_FUNCTIONS_H_ */
