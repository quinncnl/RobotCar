#include  <msp430g2553.h>
#include  <stdlib.h>
#include  "adc.h"

struct map {
	int posDis;
	int posTurn;
	int *arrDis;
	int *arrTurn;
};


// -1: right
// 1: left
// 0: nothing
// 2: turn around

int arrDisToBank[10] = {100, 100, 100, 100};
int arrTurnToBank[10] = {0, 2, 1, -1};

int arrDisToRiver[10] = {100, 100, 100, 100, 40, 30, 50, 20};
int arrTurnToRiver[10] = {0, 2, 1, -1, 2, 1, -1, 2};

int arrDisToTest[10] = {100, 100, 100, 500};
int arrTurnToTest[10] = {0, 2, -1, 1};

struct map path;

void setPathToBank(){
	path.posDis = 3;
	path.posTurn = 3;
	path.arrDis = arrDisToBank;
	path.arrTurn = arrTurnToBank;
}
void setPathToRiver(){
	path.posDis = 7;
	path.posTurn = 7;
	path.arrDis = arrDisToRiver;
	path.arrTurn = arrTurnToRiver;
}
void setPathToTest(){
	path.posDis = 3;
	path.posTurn = 3;
	path.arrDis = arrDisToTest;
	path.arrTurn = arrTurnToTest;
}

void wait(){ long i = 75000; while(i--); }
void forwardLeftWheel() { P2OUT &=~ BIT1; }
void backwardLeftWheel() { P2OUT |= BIT1; }
void forwardRightWheel(){ P2OUT &=~ BIT5; }
void backwardRightWheel(){ P2OUT |= BIT5; }

void startLeftWheel(){ TA1CCR1 = 850; }
void stopLeftWheel(){ TA1CCR1 = 0; }
void startRightWheel(){ TA1CCR2 = 800; }
void stopRightWheel(){ TA1CCR2 = 0; }


void turnLeft(){
	backwardLeftWheel();
	forwardRightWheel();
	startLeftWheel();
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
int distance = 0;
void turnAround(){ turnLeft();turnLeft(); }

void goStaight(int dis){
	onStraigt = 1;
	distance = dis;
	startLeftWheel();
	startRightWheel();
	path.posDis--;
}
void goStaightIterate(){
	distance--;
	if (distance == 0) {
		stopLeftWheel();
		stopRightWheel();
		onStraigt = 0;

		if (path.posDis < 0) {
			stopLeftWheel();
			stopRightWheel();
			exit(0);
		}

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
		goStaight(path.arrDis[path.posDis]);
 
	}
}
void route(){
	if (onStraigt)
		goStaightIterate();
}

void routeToBank(){
	setPathToTest();
	goStaight(path.arrDis[path.posDis]);
}


void initDistanceSensor(){

	__enable_interrupt();

	P2DIR &=~ (BIT0 + BIT3); // input
	P2IE |= (BIT0 + BIT3);
  
	P2IES |= (BIT0 + BIT3);
	P2IFG &=~ (BIT0 + BIT3);
}


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	route();
	P1OUT ^= BIT6;
	P2IFG &=~ (BIT0 + BIT3);
}

void initTA(){
	TA1CTL = TASSEL_2 + MC_1 + ID_2 ; // SMCLK/8, upmode
	TA1CCR0 = 1000;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;

	P2DIR |= BIT2 + BIT4;
	P2SEL |= BIT2 + BIT4;
}

int front_sensor = 0;
int left_line_sensor = 0;
int right_line_sensor = 0;

void main(void)
{
            WDTCTL = WDTPW + WDTHOLD;
 
  
	initTA();
	initDistanceSensor();
     
	P2DIR |= BIT1 + BIT5;

	P1DIR |= BIT0;
	P1DIR &= ~ BIT5;

	P1DIR |= BIT6;

	/*
	  wait();
	  turnLeft();
	  wait();
	  forwardRightWheel();
	  forwardLeftWheel();
	  goStaight(100);
	*/
	routeToBank();
	while(1){

		front_sensor = read_adc(4);
		left_line_sensor = read_adc(1);
		right_line_sensor = read_adc(2);
		if (front_sensor > 400) {
			// stop
			stopRightWheel();
			stopLeftWheel();
			P1OUT |= BIT0;
		}
		else{
			// run
			forwardLeftWheel();
			forwardRightWheel();
			startLeftWheel();
			startRightWheel();
			P1OUT &=~ BIT0;
		}

		/*
		  if (left_line_sensor > 700) {
		  stopLeftWheel();
		  }
		  else {
		  startLeftWheel();
		  }

		  if (right_line_sensor > 700) {
		  stopRightWheel();
		  }
		  else {
		  startRightWheel();
		  }
		*/
	}
}
