#include  <msp430g2553.h>
#include  <stdlib.h>
#include  "adc.h"

struct map {
	int posTurn;
	int *arrTurn;
};

// -1: right
// 1: left
// 0: nothing
// 2: turn around

struct map path;

int arrTurnToTest[10] = {2, 1, 1};
void setPathToTest(){
	path.posTurn = 2;
	path.arrTurn = arrTurnToTest;
}

void wait(){ long i = 75000; while(i--); }
void forwardLeftWheel() { P2OUT &=~ BIT1; }
void backwardLeftWheel() { P2OUT |= BIT1; }
void forwardRightWheel(){ P2OUT &=~ BIT5; }
void backwardRightWheel(){ P2OUT |= BIT5; }
void forwardWheel(){ forwardLeftWheel(); forwardRightWheel(); }
void backwardWheel(){ backwardLeftWheel(); backwardRightWheel(); }


void startLeftWheel(){ TA1CCR1 = 900; }
void stopLeftWheel(){ TA1CCR1 = 0; }
void startRightWheel(){ TA1CCR2 = 900; }
void stopRightWheel(){ TA1CCR2 = 0; }
void slowLeftWheel(){ TA1CCR1 = 100; }
void slowRightWheel(){ TA1CCR2 = 250; }
void slow() { slowRightWheel(); slowLeftWheel(); }
void stop(){ stopLeftWheel(); stopRightWheel(); }

void turnLeft(){
	//backwardLeftWheel();
	forwardRightWheel();
	slowLeftWheel();
	startRightWheel();

	long i = 37000;
	while(i--);
	stopRightWheel();
	stopLeftWheel();
}

void turnRight(){
	forwardLeftWheel();
	backwardRightWheel();
	startLeftWheel();
	startRightWheel();

	long i = 37000;
	while(i--);
	stopRightWheel();
	stopLeftWheel();
}

// 0: straight
// 1: other
int onStraigt = 0;
void turnAround(){ turnLeft();turnLeft(); }

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

int front_sensor = 0;
int left_line_sensor = 0;
int right_line_sensor = 0;

void onLED0(){ P1OUT |= BIT0; }
void offLED0(){	P1OUT &=~ BIT0; }
void onLED6(){ P1OUT |= BIT6; }
void offLED6(){	P1OUT &=~ BIT6; }

int routeMissCounter = 0;
int leftSensor, rightSensor;
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
	/*
	  
	  turnLeft();
	  wait();
	  forwardRightWheel();
	  forwardLeftWheel();
	  goStaight(100);
	*/
	// routeToBank();

	goStaight();
	forwardWheel();
	while(1){

		front_sensor = read_adc(4);
		left_line_sensor = read_adc(1);
		right_line_sensor = read_adc(2);
		/*
		if (front_sensor > 400) {
			stopRightWheel();
			stopLeftWheel();
		}
		else{
			// run
			forwardLeftWheel();
			forwardRightWheel();
			startLeftWheel();
			startRightWheel();
		}
		*/

		if (left_line_sensor > 600) {
			// not touching line
			leftSensor = 1;
			onLED0();
		}
		else {
			leftSensor = 0;
			offLED0();
		}

		if (right_line_sensor > 600) {
			rightSensor = 1;
			onLED6();
		}
		else {
			rightSensor = 0;
			offLED6();
		}

		if (leftSensor && !rightSensor) {
			// right touching
			//startLeftWheel();
			forwardLeftWheel();
			backwardRightWheel();

		}
		if (!leftSensor && rightSensor) {
			// left touching
			//startRightWheel();
			forwardRightWheel();
			backwardLeftWheel();
		}
		if (leftSensor && rightSensor) {
			// not touching
			forwardWheel();
			slow();
		}

		// Touching both line means it meets a turn.
		if (!leftSensor && !rightSensor) {
			//stop();
			turnLeft();
		}

	
	}
}
