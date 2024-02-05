/*Control the addon QRP-LPF board in transceiver "Wolf"
 *
 *Autor: Tihomir Angelov
 *
 *Info:
 *From the transceiver are coming the 4 BAND signals and the PTT
 *the filter is selected only when the PTT is active to reduce the power consumption
 *
 *4 LPF bands are realized and 1 direct (for the VHF bands)
 *
 */


#include <msp430.h> 


/*
* DEFINE PINS
*/

//Inputs (BCD code)
#define BND_0 BIT0
#define BND_1 BIT1
#define BND_2 BIT2
#define BND_3 BIT3

//PTT input
#define PTT BIT4

//Outputs (to the relays)
#define Range1 BIT0
#define Range2 BIT1
#define Range3 BIT2
#define Range4 BIT3
#define Range5 BIT4



//Functions declarations
int Delay (int i);
void Init_Pins (void);


/**
 * main.c
 */
int main(void)
{
    char ReadData = 0;

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	Delay(100);

	Init_Pins ();                           //Initialize the pins



    while (1)
    {
       // read PORT 2 byte
        ReadData = 15 - (P2IN & 0xF);      // read PORT 2 byte and take in to acount only the lower 4 bits (band data coming from the transceiver)

        if (!(P2IN & PTT))                   //PTT is active
            {

            if (ReadData < 2)               //<80m - direct connection
                {
                P1OUT &= ~Range1; //Set range 1 LOW
                P1OUT &= ~Range2; //Set range 2 LOW
                P1OUT &= ~Range3; //Set range 3 LOW
                P1OUT &= ~Range4; //Set range 4 LOW
                P1OUT |= Range5;  //Set range 5 HIGH
                }
            else if (ReadData == 2)         //80m LPF
                {
                P1OUT |= Range1;  //Set range 1 HIGH
                P1OUT &= ~Range2; //Set range 2 LOW
                P1OUT &= ~Range3; //Set range 3 LOW
                P1OUT &= ~Range4; //Set range 4 LOW
                P1OUT &= ~Range5; //Set range 5 LOW
                }
            else if (ReadData == 4)          //40m LPF
                {
                P1OUT &= ~Range1; //Set range 1 LOW
                P1OUT |= Range2;  //Set range 2 HIGH
                P1OUT &= ~Range3; //Set range 3 LOW
                P1OUT &= ~Range4; //Set range 4 LOW
                P1OUT &= ~Range5; //Set range 5 LOW
                }
            else if  ((ReadData == 5)||(ReadData == 6))          //30m and 20m  LPF
                {
                P1OUT &= ~Range1; //Set range 1 LOW
                P1OUT &= ~Range2; //Set range 2 LOW
                P1OUT |= Range3;  //Set range 3 HIGH
                P1OUT &= ~Range4; //Set range 4 LOW
                P1OUT &= ~Range5; //Set range 5 LOW
                }
            else if  ((ReadData > 6)&&(ReadData < 12))          //17/15/12/10m LPF
                {
                P1OUT &= ~Range1; //Set range 1 LOW
                P1OUT &= ~Range2; //Set range 2 LOW
                P1OUT &= ~Range3; //Set range 3 LOW
                P1OUT |= Range4;  //Set range 4 HIGH
                P1OUT &= ~Range5; //Set range 5 LOW
                }
            else                                                // rest direct connection
                {
                P1OUT &= ~Range1; //Set range 1 LOW
                P1OUT &= ~Range2; //Set range 2 LOW
                P1OUT &= ~Range3; //Set range 3 LOW
                P1OUT &= ~Range4; //Set range 4 LOW
                P1OUT |= Range5;  //Set range 5 HIGH
                }
            }
        else                                                    //PTT is released
            {
            P1OUT &= ~Range1; //Set range 1 LOW
            P1OUT &= ~Range2; //Set range 2 LOW
            P1OUT &= ~Range3; //Set range 3 LOW
            P1OUT &= ~Range4; //Set range 4 LOW
            P1OUT &= ~Range5; //Set range 5 LOW
            }
    }


}

void Init_Pins (void)
{
    P1DIR |= Range1;  //Set as an Output pin
    P1DIR |= Range2;  //Set as an Output pin
    P1DIR |= Range3;  //Set as an Output pin
    P1DIR |= Range4;  //Set as an Output pin
    P1DIR |= Range5;  //Set as an Output pin

    P1OUT &= ~Range1; //Set LOW
    P1OUT &= ~Range2; //Set LOW
    P1OUT &= ~Range3; //Set LOW
    P1OUT &= ~Range4; //Set LOW
    P1OUT &= ~Range5; //Set LOW

    P2REN |= BND_0;                   // Enable internal pull-up/down resistors
    P2OUT |= BND_0;                    //Select pull-up mode

    P2REN |= BND_1;                   // Enable internal pull-up/down resistors
    P2OUT |= BND_1;                    //Select pull-up mode

    P2REN |= BND_2;                   // Enable internal pull-up/down resistors
    P2OUT |= BND_2;                    //Select pull-up mode

    P2REN |= BND_3;                   // Enable internal pull-up/down resistors
    P2OUT |= BND_3;                    //Select pull-up mode

    P2REN |= PTT;                   // Enable internal pull-up/down resistors
    P2OUT |= PTT;                    //Select pull-up mode

}

int Delay (int i)
{
    int Count;
    Count=i*100;            //
    while(Count>=0)
        {Count--;}
    return(1);
}
