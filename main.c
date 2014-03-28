#include  <msp430g2553.h>
#include  <stdlib.h>
#include  "adc.h"
#include  "utils.h"

int leftSensor, rightSensor;

struct map {
	int posTurn;
	int *arrTurn;
};

// 1: right
// -1: left
// 0: nothing

struct map path;

int arrTurnToTest[10] = {1, 1, -1, -1};
void setPathToTest(){
	path.posTurn = 3;
	path.arrTurn = arrTurnToTest;
}


// 0: straight
// 1: other
int onStraigt = 0;

void goStaight(){
	onStraigt = 1;
	startLeftWheel();
	startRightWheel();
}
void goStaightIterate(){

	onStraigt = 0;

	switch (path.arrTurn[path.posTurn]) {
	case 1:
		turnLeft();
		break;
	case -1:
		turnRight();
		break;
	case 2:
		turnAround();
		break;
	}

	path.posTurn--;
}
void route(){
}

void routeToBank(){
	setPathToTest();
}


void initDistanceSensor(){

	__enable_interrupt();

	P2DIR &=~ (BIT0 + BIT3); // input
	P2IE |= (BIT0);
  
	P2IES |= (BIT0);
	P2IFG &=~ (BIT0);
}


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	route();
	//	P1OUT ^= BIT6;
	P2IFG &=~ (BIT0);
}

void initTA(){
	TA1CTL = TASSEL_2 + MC_1;
	TA1CCR0 = 1000;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;

	P2DIR |= BIT2 + BIT4;
	P2SEL |= BIT2 + BIT4;
}

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
   
	initTA();
	initDistanceSensor();
     
	P2DIR |= BIT1 + BIT5;

	P1DIR |= BIT0;
	P1DIR &= ~ BIT5;

	P1DIR |= BIT6;

	wait();

	// routeToBank();

	//goStaight();
	//forwardWheel();

	while(1){

		//front_sensor = read_adc(4);
	
		readLineSensor();
		if (touchTargetLeft())	{
			targetReached();
		}

		/*
		if (front_sensor > 400) {
			stop();
		}
		*/

		if (touchRoadLeft()) {
			// not touching line
			leftSensor = 1;
			onLED0();
		}
		else {
			leftSensor = 0;
			offLED0();
		}
	
		if (touchRoadRight()) {
			rightSensor = 1;
			onLED6();
		}
		else {
			rightSensor = 0;
			offLED6();
		}

		if (leftSensor && !rightSensor) {
			// right touching
			forwardLeftWheel();
			backwardRightWheel();
			slow();
		}
		if (!leftSensor && rightSensor) {
			// left touching
			forwardRightWheel();
			backwardLeftWheel();
			slow();
		}
		if (leftSensor && rightSensor) {
			// not touching
			forwardWheel();
			slow();
		}

		// Touching both line means it meets a turn.
		if (!leftSensor && !rightSensor) {
			/*
			if (path.posTurn >= 0) {
				switch (path.arrTurn[path.posTurn]) {
					case -1:
					turnLeft();
					break;
					case 1:
					turnRight();
					break;
				}
				path.posTurn--;
			}
			*/
			turnLeft();
		}

	}
}
