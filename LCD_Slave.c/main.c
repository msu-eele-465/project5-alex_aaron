#include <msp430.h>

int i;
unsigned char dataIn;
unsigned char TempType;
int Space;
int Data;                       // Data is used to receive data from buffer
int Temp;
int n = 3;

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

    init();                     // call init function
    Command(0x00);              // clear LCD display

    UCB0CTLW0 &= ~UCSWRST;      // put out of reset

    UCB0IE |= UCRXIE0;          // ENABLE I2C RX IRQ

    __enable_interrupt();       // global enable

    while (1) {
        if (Data > 0x26) {
            TempType = 'F';
        } else {
            TempType = 'C';
        }
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

    switch (Data) {
        case 0x41:                      // Input window size mode
            command(0x01);      //clear display
            Delay(200);
            DisplayPattern("Input Window Size");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x42:                      // Input pattern mode
            command(0x01);      // clear display
            Delay(200);
            DisplayPattern("Input Pattern");
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x23:              // '#' clears display
            Command(0x01);
            Delay(200);
            break;
        case 0x24:              // Pattern 0
            Command(0x01);
            DisplayPattern("static");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x25:              // Pattern 1
            Command(0x01);
            DisplayPattern("toggle");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x26:              // Pattern 2
            Command(0x01);
            DisplayPattern("up counter");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x27:              // Pattern 3
            Command(0x01);
            DisplayPattern("in and out");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x28:              // Pattern 4
            Command(0x01);
            DisplayPattern("down counter");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x29:              // Pattern 5
            Command(0x01);
            DisplayPattern("rotate left");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x2A:              // Pattern 6
            Command(0x01);
            DisplayPattern("rotate right");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        case 0x2B:              // Pattern 7
            Command(0x01);
            DisplayPattern("fill left");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;
        default:
            Command(0x01);
            DisplayPattern("LED pattern");
            command(0x40);              // move to second line
            DisplayPattern("T=");
            DisplayPattern(Temp);
            DisplayPattern(0xDF);       // Degrees symbol
            DisplayPattern(TempType);
            command(0x4D);              // move to botton-right corner
            DisplayPattern("N=");
            DisplayPattern(n);
            break;          
    }
*/
    Delay(200);
    UCB0IE |= UCRXIE0;
}
