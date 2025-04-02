#include <msp430.h>

unsigned int ADC_Value;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;                                           // Set P1.0/LED to output direction
    P1OUT &= ~BIT0;                                          // P1.0 LED off

    while(1)
    {

    }
    return 0;
}