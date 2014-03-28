#include  <msp430g2553.h>
#include  <stdlib.h>
#include  "adc.h"
#include  "utils.h"

int leftSensor, rightSensor;
int timing = 0;
int timeCounterOn = 0;

struct map {
	int posTurn;
	int *arrTurn;
};

// -1: left
// 1: right
// 0: nothing

struct map path;

int arrTurnToBoulangerie[10] = {1, 1, -1, -1};
void routeToBoulangerie(){
	path.posTurn = 3;
	path.arrTurn = arrTurnToBoulangerie;
}

int arrTurnToBanque[10] = {0, 1, 1, -1, -1, 0};
void routeToBank(){
	path.posTurn = 5;
	path.arrTurn = arrTurnToBanque;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

	//	P1OUT ^= BIT6;
	P2IFG &=~ (BIT0);
}

void startTimeCounter(){
	timeCounterOn = 1;
	timing = 0;
}
void stopTimeCounter(){
	timeCounterOn = 0;
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
     
	P2DIR |= BIT1 + BIT5;

	P1DIR |= BIT0;
	P1DIR &= ~ BIT5;

	P1DIR |= BIT6;

	wait();

	routeToBank();

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
			//onLED0();
		}
		else {
			leftSensor = 0;
			//offLED0();
		}
	
		if (touchRoadRight()) {
			rightSensor = 1;
			//onLED6();
		}
		else {
			rightSensor = 0;
			//offLED6();
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

		if (timeCounterOn) timing++;
		if (timeCounterOn && timing > 59999) {
			// passed a corner
			stopTimeCounter();
			stop();
			delay(30000);
			slow();
		}

		// Touching both line means it meets a turn.
		if (!leftSensor && !rightSensor) {

			//if (timeCounterOn == 0 && path.posTurn >= 0) {
				switch (path.arrTurn[path.posTurn]) {
					case -1:
					turnLeft();
					break;
					case 1:
					turnRight();
					break;
					case 0:

					stop();
					delay(50000);
					forwardWheel();
					fullSpeed();
					delay(9000);

				}
				path.posTurn--;
	
				forwardWheel();
				slow();
			
				debugWithLED(path.posTurn);
				startTimeCounter();
			//}
			
		}
		debugWithLED(path.posTurn);
	}
}
