#include <msp430.h>

unsigned int ADC_Value;
float temperature;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    //-- Configure Ports
    P1DIR |= BIT0;              // Config P1.0 (LED1) as output
    P6DIR |= BIT6;              // Config P6.6 (LED2) as output

    P1SEL1 |= BIT2;             // Configure P1.2 Pin for ADC (A2)
    P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;       // Enable GPIO

    //-- Configure ADC
    ADCCTL0 &= ~ADCSHT;         // Clear ADCSHT
    ADCCTL0 |= ADCSHT_2;        // Conversion Cycles = 16
    ADCCTL0 |= ADCON;           // Turn ADC on

    ADCCTL1 |= ADCSSEL_2;       // ADC Clock Source = SMCLK
    ADCCTL1 |= ADCSHP;          // Sample signal source = sampling timer

    ADCCTL2 &= ~ADCRES;         // Clear ADCRES
    ADCCTL2 |= ADCRES_2;        // 12-bit resolution

    ADCMCTL0 |= ADCINCH_2;      // Select ADC channel (A2)
    ADCIE |= ADCIE0;            // Enable ADC interrupt

    while(1)
    {
        ADCCTL0 |= ADCENC | ADCSC;  // Start ADC conversion
        __bis_SR_register(GIE);  // Enable interrupts, enter LPM0
    }

    return 0;
}

//------ Interrupt Service Routine (ISR) ------------------
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    ADC_Value = ADCMEM0;  // Read ADC value

    // Convert ADC value to voltage (assuming 3.3V reference)
    float Vout;
    Vout = (ADC_Value * 3.3) / 4096.0;

    // Convert voltage to temperature
    temperature = (1.863 - Vout) / 11.69;

    __bic_SR_register_on_exit(LPM0_bits);  // Wake up CPU

    // Turn LED1 ON if temp > 30°C, else turn LED2 ON
    if (temperature > 15.0)
    {
        P1OUT |= BIT0;  // LED1 ON (Red)
        P6OUT &= ~BIT6; // LED2 OFF
    }
    else
    {
        P1OUT &= ~BIT0; // LED1 OFF
        P6OUT |= BIT6;  // LED2 ON (Green)
    }
}
