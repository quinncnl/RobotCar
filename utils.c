#include  <msp430g2553.h>
#include "main.h"


void wait(){ long i = 75000; while(i--); }
void delay(long i) {while(i--);}


void initTA(){
	TA1CTL = TASSEL_2 + MC_1;
	TA1CCR0 = 1000;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;

	P2DIR |= BIT2 + BIT4;
	P2SEL |= BIT2 + BIT4;
}

void initPins(){

	P1DIR &=~ (BIT7 + BIT5); // Destination selector
	P1REN |= (BIT7 + BIT5);
	P1OUT &=~ (BIT7 + BIT5);

    P2DIR |= BIT1 + BIT5; // Motors
	P1DIR |= BIT0 + BIT6; // Debug leds

	// Button
	P1DIR &= ~BIT3;
	P1REN |= BIT3;
	P1OUT |= BIT3;
	P1IE |= BIT3;
	P1IES |= BIT3;
	P1IFG &= ~BIT3;
}


void onLED0(){ P1OUT |= BIT0; }
void offLED0(){	P1OUT &=~ BIT0; }
void onLED6(){ P1OUT |= BIT6; }
void offLED6(){	P1OUT &=~ BIT6; }

void debugWithLED(int i) {
	i = i % 4;

	switch (i) {
	case 0:
		offLED0();
		offLED6();
		break;
	case 1:
		onLED0();
		offLED6();
		break;
	case 2:
		onLED6();
		offLED0();
		break;
	case 3:
		onLED6();
		onLED0();
	}
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	int P17 = P1IN & BIT7;
	int P15 = P1IN & BIT5;
	if (P1IFG & BIT3) {

		P1IFG &= ~BIT3;
		if (P15 && P17) {
			// on/on
			debugWithLED(3);
			routeToBoulangerie();
		}
		else if (P15 == 0 && P17) {
			// of/on
			debugWithLED(1);
			routeToBank();
		}
		else if (P15 && P17 == 0) {
			// on/of
			debugWithLED(2);
			routeToPoste();
		}
		else if (P15 == 0 && P17 == 0) {
			// of/of
			debugWithLED(0);
			routeToHospital();
		}
	}
}
