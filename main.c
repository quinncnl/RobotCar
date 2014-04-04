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
} path;

// -1: left
// 1: right
// 0: nothing

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

int arrTurnToPoste[10] = {0, 0, 1, -1, 1, -1, 0, 0};
void routeToPoste(){
	path.posTurn = 7;
	path.arrTurn = arrTurnToPoste;
}

int arrTurnToHospital[10] = {1, 0, -1, -1, 1, 0, -1, 0, 0};
void routeToHospital(){
	path.posTurn = 8;
	path.arrTurn = arrTurnToHospital;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

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
     
	P2DIR |= BIT1 + BIT5;

	P1DIR |= BIT0;
	P1DIR &= ~ BIT5;

	P1DIR |= BIT6;

	wait();

	routeToBoulangerie();

	while(1){

		readFrontSensor();	
		readLineSensor();

		if (touchTargetLeft())	{
			targetReached();
		}

		if (meetObstacle()) {
			stop();
		}

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
			if (path.posTurn == -1) {
				enterGarage();
				exit(0);
			}
			switch (path.arrTurn[path.posTurn]) {
				case -1:
					fixStraigt();
					turnLeft();
					break;
				case 1:
					fixStraigt();
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
	
		}
		debugWithLED(path.posTurn);
	}
}
