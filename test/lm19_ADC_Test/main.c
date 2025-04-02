#include <msp430.h>

unsigned int ADC_Value;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //-- Configure Ports
    P1DIR |= BIT0;              // Config P1.0 (LED1) as output
    P6DIR |= BIT6;              // Config P6.6 (LED2) as output

    P1SEL1 |= BIT2;             // Configure P1.2 Pin for A2
    P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;       // Turn on GPIO

    //-- Configure ADC
    ADCCTL0 &= ~ADCSHT;         // Clear ADCSHT from def. of ADCSHT=01
    ADCCTL0 |= ADCSHT_3;        // Conversion Cycles = 16 (ADCSHT=10)
    ADCCTL0 |= ADCON;           // ADC = on.

    ADCCTL1 |= ADCSSEL_2;       // ADC Clock Source = SMCLK
    ADCCTL1 |= ADCSHP;          // Sample signal source = sampling timer

    ADCCTL2 &= ~ADCRES;         // Clear ADCRES from def. of ADCRES=01
    ADCCTL2 |= ADCRES_2;        // Resolution = 12-bit (ADCRES=10)

    ADCMCTL0 |= ADCINCH_2;      // ADC Input Channel = A2 (P1.2)

    while(1)
    {
        ADCCTL0 |= ADCENC | ADCSC;    // Enable and Start conversion
        while((ADCIFG & ADCIFG0) == 0); // wait for conversion complete
        ADC_Value = ADCMEM0;          // Read ADC result

        if(ADC_Value > 3613)
        {  
            P1OUT |= BIT0;            // If (A2 > 3V), LED1=ON (red)
            P6OUT &= ~BIT6;           // LED2=OFF
        }
        else
        {
            P1OUT &= ~BIT0;           // If (A2 < 3V), LED1=OFF
            P6OUT |= BIT6;            // LED2=ON (green)
        }
    }

    return 0;
}
