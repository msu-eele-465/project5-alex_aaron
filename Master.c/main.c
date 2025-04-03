#include <msp430.h>
#include <math.h>

int i,j;
char Button = '/0';
char keypad;                    // keypad character value
int Passcode = 0;               // initialize as 0
char Pressed = 0x00;            // initialize as 0
float tempToDisplay;		//SET
int   temp;				//SET
unsigned int ADC_Value;
int n;                          // window size


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    UCB0CTLW0   |=  UCSWRST;    // put into reset

    UCB0CTLW0   |= UCSSEL_3;    // setting up SMCLK
    UCB0BRW = 10;               // dividing by 10

    UCB0CTLW0   |=  UCMODE_3;   // put into I2C mode
    UCB0CTLW0   |=  UCMST;      // put into master mode
    UCB0CTLW0   |=  UCTR;       // put into send (TX) mode

    UCB0CTLW1   |=  UCASTP_2;   // auto stop
    UCB0TBCNT   =   0x01;       // send 1 byte of data

    P1SEL1  &=  ~BIT3;          // P1.3 as SCL
    P1SEL0  |=  BIT3;
    P1SEL1  &=  ~BIT2;          // P1.2 as SDA
    P1SEL0  |=  BIT2;

    // configure ADC ports
    P1SEL1 |= BIT4;                     // set P1.2 for LM19 Vout  //SET
    P1SEL0 |= BIT4;								//SET

    PM5CTL0 &=  ~LOCKLPM5;      // disable digital I/O low-power default

    UCB0CTLW0   &=  ~UCSWRST;   // put out of reset

    P2DIR |= BIT2;              // initialize lock LED  //SET
    P2OUT |= BIT2;              // initialize as On		//SET
    P2DIR |= BIT3;              // initialize unlock LED //SET
    P2OUT &= ~BIT3;             // initialize as OFF  //SET

    // ADC setup
    ADCCTL0 &= ~ADCSHT;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;
    ADCCTL1 |= ADCSSEL_2;
    ADCCTL1 |= ADCSHP;
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;
    ADCMCTL0 |= ADCINCH_4;

    UCB0IE  |=  UCTXIE0;        // ENABLE I2C TX IRQ

    __enable_interrupt();       // global enable

    int n = 3;

    tempToDisplay = 43.1;   		//SET
    temp = tempToDisplay/10;	 	//SET 
	
    // Passcode
    while(Passcode < 3) {
        keypad = ButtonCheck();

        if(keypad == '3' && Passcode == 0) {
            Passcode++;
        }
        if(keypad == '6' && Passcode == 1) {
            Passcode++;
        }
        if(keypad == '9' && Passcode == 2) {
            Passcode++;
        }
    }

    while(1){
        P2OUT &= ~BIT2;                 // red light off
        P2OUT |= BIT3;                  // green light on
        Delay(80);


		//SET
        UCB0I2CSA   =   0x0012;         // LED slave address set to 0x12
        keypad = ButtonCheck();         // check keypad value
        if(keypad != '\0') {
            if(keypad == 'A'){
                keypad = 0x01;
                UCB0CTLW0   |=  UCTXSTT;    // send start
                Delay(100);
            }else if(keypad == 'B'){
                keypad = 0x02;
                UCB0CTLW0   |=  UCTXSTT;    // send start
                Delay(100);
            }else if(keypad == 'C'){
                keypad = 0x03;
                UCB0CTLW0   |=  UCTXSTT;    // send start
                Delay(100);
            }else if(keypad == 'D'){
                keypad = 0x04;
                UCB0CTLW0   |=  UCTXSTT;    // send start
                Delay(100);
            }else if(keypad == '*'){
                keypad = 0x2A;
                UCB0CTLW0   |=  UCTXSTT;    // send start
                Delay(100);
            }
        }
        UCB0I2CSA   =   0x0013;         // LCD slave address set to 0x13
        temp = 0x34;
        UCB0CTLW0   |=  UCTXSTT;    // send start
        Delay(100);

/*
        ADCCTL0 | ADCENC | ADCSC;
        while((ADCIFG & ADCIFG0) == 0){}
        ADC_Value = ADCMEM0;

        tempToDisplay = ADC_Value/300;
        temp = tempToDisplay;
*/
    }

    return 0;
}


// Checks for keypad button press
char ButtonCheck() {

    // Setting up input ports
    P3DIR &= ~BIT0;
    P3DIR &= ~BIT1;
    P3DIR &= ~BIT2;
    P3DIR &= ~BIT3;

    P3REN |= BIT0;
    P3REN |= BIT1;
    P3REN |= BIT2;
    P3REN |= BIT3;

    P3OUT &= ~BIT0;
    P3OUT &= ~BIT1;
    P3OUT &= ~BIT2;
    P3OUT &= ~BIT3;

    P3DIR |= BIT4;
    P3DIR |= BIT5;
    P3DIR |= BIT6;
    P3DIR |= BIT7;

    P3OUT |= BIT4;
    P3OUT |= BIT5;
    P3OUT |= BIT6;
    P3OUT |= BIT7;


    char ButtonPressed = P3IN;

    // Switch statement for the columns
    switch(ButtonPressed) {
        case 0xF8:
            Pressed = 0x08;
            break;

        case 0xF4:
            Pressed = 0x04;
            break;

        case 0xF2:
            Pressed = 0x02;
            break;

        case 0xF1:
            Pressed = 0x01;
            break;

        default:
            return '\0';
        }

    // Setting up input ports
    P3DIR |= BIT0;		//SET
    P3DIR |= BIT1;		//SET
    P3DIR |= BIT2;		//SET
    P3DIR |= BIT3;		//SET

    P3OUT |= BIT0;		//SET
    P3OUT |= BIT1;		//SET
    P3OUT |= BIT2;		//SET
    P3OUT |= BIT3;		//SET
	//SET
    P3DIR &= ~BIT4;
    P3DIR &= ~BIT5;
    P3DIR &= ~BIT6;
    P3DIR &= ~BIT7;
	//SET
    P3REN |= BIT4;
    P3REN |= BIT5;
    P3REN |= BIT6;
    P3REN |= BIT7;
	//SET
    P3OUT &= ~BIT4;
    P3OUT &= ~BIT5;
    P3OUT &= ~BIT6;
    P3OUT &= ~BIT7;


    ButtonPressed = P3IN;

    // Switch statement for the rows
    switch(ButtonPressed) {
        case 0x8F:
            Pressed ^= 0x80;
            break;

        case 0x4F:
            Pressed ^= 0x40;
            break;

        case 0x2F:
            Pressed ^= 0x20;
            break;

        case 0x1F:
            Pressed ^= 0x10;
            break;

        default:
            return '\0';
    }

    return ButtonDisplay(Pressed);
}

char ButtonDisplay(char Pressed) {

    switch(Pressed) {
        case 0x88:
            Button = '1';
            n = 1;
            break;
        case 0x84:
            Button = '2';
            n = 2;
            break;
        case 0x82:
            Button = '3';
            n = 3;
            break;
        case 0x81:
            Button = 'A';
            break;
        case 0x48:
            Button = '4';
            n = 4;
            break;
        case 0x44:
            Button = '5';
            n = 5;
            break;
        case 0x42:
            Button = '6';
            n = 6;
            break;
        case 0x41:
            Button = 'B';
            break;

        case 0x28:
            Button = '7';
            n = 7;
            break;
        case 0x24:
            Button = '8';
            n = 8;
            break;
        case 0x22:
            Button = '9';
            n = 9;
            break;
        case 0x21:
            Button = 'C';
            break;

        case 0x18:
            Button = '*';
            break;
        case 0x14:
            Button = '0';
            break;
        case 0x12:
            Button = '#';
            break;
        case 0x11:
            Button = 'D';
            break;
    }
    return Button;
}


void Delay(int cnt){
    for(j=0; j<cnt; j++){
        for(i=0; i<102; i++){}
    }
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0TXBUF = temp;             // send data to buffer //SET
}