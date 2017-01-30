/*
 * Application_functions.h
 *
 *  Created on: Feb 16, 2013
 *      Author: gURpaL
 */

#ifndef APPLICATION_FUNCTIONS_H_
#define APPLICATION_FUNCTIONS_H_

void ApplicationLaunch();
void OpenDataSession();
void iPodDataTransfer(unsigned char *transferData);
void SendAccessoryAck(unsigned char iPodCommandID);
unsigned int ApplicationLaunchPayload();
void WaitForiPodDataTransfer();
void WaitForOpenDataSession();
void ClearApplicationPayloadPacket();


#endif /* APPLICATION_FUNCTIONS_H_ */
