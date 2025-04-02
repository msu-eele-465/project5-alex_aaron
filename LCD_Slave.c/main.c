#include <msp430.h>

int i;
unsigned char dataIn;
int Space;
int Data;                       // Data is used to receive data from buffer

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    UCB0CTLW0 |= UCSWRST;       // put into software reset

    UCB0CTLW0 |= UCMODE_3;      // put into I2C mode
    UCB0CTLW0 &= ~UCMST;        // put into slave mode
    UCB0I2COA0 = 0x0013;        // LCD slave address set to 0x13
    UCB0I2COA0 |= UCOAEN;       // slave address enable
    UCB0CTLW0 &= ~UCTR;         // put into receive mode
    UCB0CTLW1 &= ~UCSWACK;      // auto acknowledge
    UCB0TBCNT = 0x01;           // send 1 byte of data

    UCB0CTLW1 &= ~UCTXIFG0;

    P1SEL1 &= ~BIT3;            // P1.3 as SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;            // P1.2 as SDA
    P1SEL0 |= BIT2;

    // Setting upper bits of port 1 as output
    P1DIR |= BIT7 | BIT6 | BIT5 | BIT4;
    P1OUT &= ~(BIT7 | BIT6 | BIT5 | BIT4);

    P2DIR |= BIT6 | BIT7;       // P2.6 as RS, P2.7 as Enable
    P2OUT &= ~(BIT6 | BIT7);

    PM5CTL0 &= ~LOCKLPM5;       // disable digital I/O low-power default

    // ADC Setup
    // Configure ADC A1 pin
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;

    PM5CTL0 &= ~LOCKLPM5;                                   // Turn on GPIO

    // Configure ADC12
    ADCCTL0 &= ~ADCSHT;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;
    
    ADCCTL1 |= ADCSSEL_2;
    ADCCTL1 |= ADCSHP;

    ADCMCTL0 |= ADCINCH_1;

    init();                     // call init function
    Command(0x00);              // clear LCD display

    UCB0CTLW0 &= ~UCSWRST;      // put out of reset

    UCB0IE |= UCRXIE0;          // ENABLE I2C RX IRQ

    __enable_interrupt();       // global enable

    while (1) {
        ADCCTL0 |= ADCENC | ADCSC;
        while((ADCIFG & ADCIFG0) == 0){}
        ADC_Value = ADCMEM0;
    }
    return 0;
}

void Command(unsigned char SendCom) {
    for (int shift = 0; shift <= 4; shift += 4) {
        P1OUT &= 0x0F;
        P1OUT |= (SendCom << shift) & 0xF0;
        P2OUT &= ~BIT6;         // Command mode
        Nybble();
    }
}

void Write(unsigned char SendWR) {
    for (int shift = 0; shift <= 4; shift += 4) {
        P1OUT &= 0x0F;
        P1OUT |= (SendWR << shift) & 0xF0;
        P2OUT |= BIT6;          // Write mode
        Nybble();
    }
}

void Nybble() {
    P2OUT |= BIT7;
    Delay(1);
    P2OUT &= ~BIT7;
}

void init() {
    Delay(100);
    P1OUT |= BIT5 | BIT4;
    Delay(30);
    Nybble();
    Delay(10);
    Nybble();
    Delay(10);
    Nybble();
    Delay(10);
    P1OUT &= ~BIT4;
    Nybble();
    Command(0x28);              // 4-bit/2-line mode
    Command(0x10);              // Set cursor
    Command(0x0F);              // Display on, cursor blinking
    Command(0x06);              // Entry mode
}

int Delay(int CountLong) {
    while (CountLong-- > 0) {
        InnerDelay();
    }
    return 0;
}

int InnerDelay() {
    for (int CountShort = 0; CountShort < 102; CountShort++) {}
    return 0;
}

// Display the pattern name
void DisplayPattern(const char* text) {
    while (*text) {
        Delay(20);
        Write(*text++);
        Space++;
    }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {
    UCB0IE &= ~UCRXIE0;
    Data = UCB0RXBUF;
/*
    switch (Data) {
        case 0x23:              // '#' clears display
            Command(0x01);
            Delay(200);
            Space = 1;
            Write(Data);
            break;
        case 0x24:              // Pattern 0
            DisplayPattern("static");
            break;
        case 0x25:              // Pattern 1
            DisplayPattern("toggle");
            break;
        case 0x26:              // Pattern 2
            DisplayPattern("up counter");
            break;
        case 0x27:              // Pattern 3
            DisplayPattern("in and out");
            break;
        case 0x28:              // Pattern 4
            DisplayPattern("down counter");
            break;
        case 0x29:              // Pattern 5
            DisplayPattern("rotate left");
            break;
        case 0x2A:              // Pattern 6
            DisplayPattern("rotate right");
            break;
        case 0x2B:              // Pattern 7
            DisplayPattern("fill left");
            break;          
        default:
            if (Space == 32) {
                Command(0x01);
                Delay(200);
                Space = 1;
            } 
            else if (Space == 16) {
                Command(0xC0);
                Space++;
            }
            Write(Data);
            Space++;
            break;
    }
*/
    Delay(200);
    UCB0IE |= UCRXIE0;
}
