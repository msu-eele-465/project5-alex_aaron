#include <msp430.h>

unsigned char TempType;
int Data, Temp;
int n = 3;
int i;

void Command(unsigned char SendCom);
void Write(unsigned char SendWR);
void Nybble();
void init();
void Delay(int CountLong);
void DisplayPattern(const char *text);
void UpdateLCD(const char *patternName);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // I2C Configuration
    UCB0CTLW0 |= UCSWRST;       // Put into software reset
    UCB0CTLW0 |= UCMODE_3;      // I2C mode
    UCB0CTLW0 &= ~UCMST;        // Slave mode
    UCB0I2COA0 = 0x0013 | UCOAEN; // Set slave address to 0x13 and enable
    UCB0CTLW0 &= ~UCTR;         // Receive mode
    UCB0CTLW1 &= ~UCSWACK;      // Auto acknowledge
    UCB0TBCNT = 0x01;           // Send 1 byte of data

    P1SEL0 |= BIT3 | BIT2;      // P1.3 as SCL, P1.2 as SDA

    P1DIR |= BIT7 | BIT6 | BIT5 | BIT4; // Upper bits of P1 as output
    P1OUT &= ~(BIT7 | BIT6 | BIT5 | BIT4);

    P2DIR |= BIT6 | BIT7;       // P2.6 as RS, P2.7 as Enable
    P2OUT &= ~(BIT6 | BIT7);

    PM5CTL0 &= ~LOCKLPM5;       // Enable digital I/O

    init();
    Command(0x00);              // Clear LCD display

    UCB0CTLW0 &= ~UCSWRST;      // Release reset
    UCB0IE |= UCRXIE0;          // Enable I2C RX interrupt
    __enable_interrupt();       // Enable global interrupts

    while (1) {
        TempType = (Data > 0x26) ? 'F' : 'C';
    }
}

void Command(unsigned char SendCom) {
    P1OUT &= 0x0F;
    P1OUT |= (SendCom & 0xF0);
    P2OUT &= ~BIT6;  // Command mode
    Nybble();

    P1OUT &= 0x0F;
    P1OUT |= ((SendCom << 4) & 0xF0);
    Nybble();
}

void Write(unsigned char SendWR) {
    P1OUT &= 0x0F;
    P1OUT |= (SendWR & 0xF0);
    P2OUT |= BIT6;  // Write mode
    Nybble();

    P1OUT &= 0x0F;
    P1OUT |= ((SendWR << 4) & 0xF0);
    Nybble();
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

    Command(0x28); // 4-bit/2-line mode
    Command(0x10); // Set cursor
    Command(0x0F); // Display on, cursor blinking
    Command(0x06); // Entry mode
}

void Delay(int CountLong) {
    while (CountLong-- > 0) {
        for (i = 0; i < 102; i++); // Inline InnerDelay
    }
}

void DisplayPattern(const char *text) {
    while (*text) {
        Delay(20);
        Write(*text++);
    }
}

void UpdateLCD(const char *patternName) {
    Command(0x01);  // Clear display
    DisplayPattern(patternName);

    Command(0x40);  // Move to second line
    DisplayPattern("T=");
    DisplayPattern(Temp);
    Write(0xDF);  // Degrees symbol
    Write(TempType);

    Command(0x4D);  // Move to bottom-right corner
    DisplayPattern("N=");
    DisplayPattern(n);
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {
    UCB0IE &= ~UCRXIE0;
    Data = UCB0RXBUF;

    switch (Data) {
        case 0x41: UpdateLCD("Input Window Size"); break;
        case 0x42: UpdateLCD("Input Pattern"); break;
        case 0x23: Command(0x01); Delay(200); break;  // Clear display
        case 0x24: UpdateLCD("Static"); break;
        case 0x25: UpdateLCD("Toggle"); break;
        case 0x26: UpdateLCD("Up Counter"); break;
        case 0x27: UpdateLCD("In and Out"); break;
        case 0x28: UpdateLCD("Down Counter"); break;
        case 0x29: UpdateLCD("Rotate Left"); break;
        case 0x2A: UpdateLCD("Rotate Right"); break;
        case 0x2B: UpdateLCD("Fill Left"); break;
        default:    UpdateLCD("LED Pattern"); break;
    }

    Delay(200);
    UCB0IE |= UCRXIE0;
}
