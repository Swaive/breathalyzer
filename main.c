
/*
 * main.c
 */

#include <msp430.h>
#include "Authentication.h"
#include "Application.h"

void main() {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    Authentication();
    Application();

}
