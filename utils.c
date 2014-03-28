#include  <msp430g2553.h>
#include  "utils.h"


int front_sensor = 0;
int left_line_sensor = 0;
int right_line_sensor = 0;

void wait(){ long i = 75000; while(i--); }
void forwardLeftWheel() { P2OUT &=~ BIT1; }
void backwardLeftWheel() { P2OUT |= BIT1; }
void forwardRightWheel(){ P2OUT &=~ BIT5; }
void backwardRightWheel(){ P2OUT |= BIT5; }
void forwardWheel(){ forwardLeftWheel(); forwardRightWheel(); }
void backwardWheel(){ backwardLeftWheel(); backwardRightWheel(); }

void fullSpeed(){ TA1CCR1 = 1000; TA1CCR2 = 950; }
void startLeftWheel(){ TA1CCR1 = 600; }
void stopLeftWheel(){ TA1CCR1 = 0; }
void startRightWheel(){ TA1CCR2 = 600; }
void stopRightWheel(){ TA1CCR2 = 0; }
void slowLeftWheel(){ TA1CCR1 = 430; }
void slowRightWheel(){ TA1CCR2 = 440; }
void slow() { slowRightWheel(); slowLeftWheel(); }
void stop(){ stopLeftWheel(); stopRightWheel(); }

void onLED0(){ P1OUT |= BIT0; }
void offLED0(){	P1OUT &=~ BIT0; }
void onLED6(){ P1OUT |= BIT6; }
void offLED6(){	P1OUT &=~ BIT6; }

void readLineSensor() {
	left_line_sensor = read_adc(1);
	right_line_sensor = read_adc(2);
}

void turnLeft(){
	backwardLeftWheel();
	forwardRightWheel();
	slowLeftWheel();
	slowRightWheel();

	long i = 22000;
	while(i--);
	stopRightWheel();
	stopLeftWheel();
}

void turnRight(){
	forwardLeftWheel();
	backwardRightWheel();
	startLeftWheel();
	startRightWheel();

	long i = 27000;
	while(i--);
	stopRightWheel();
	stopLeftWheel();
}

void turnAround(){ 

// turn right
	forwardLeftWheel();
	backwardRightWheel();
//	fullSpeed();
	slow();

	long i = 99900;
	while(i--) {

		readLineSensor();

		if (!touchRoad(left_line_sensor)) {
			stop();
			while (1);
		}

		if (!touchRoad(right_line_sensor)) {
			stop();
			while (1);
		}
	}
}

void determineValue(int v){
	if (v < 500)	{
		onLED0();
		onLED6();
	}
	else if (v < 600)	{
		offLED0();
		onLED6();
	}
	else if (v < 900)	{
		onLED0();
		offLED6();
	}
	else {
		offLED0();
		offLED6();
	}
}

int touchTarget(int s){ return (s > 900); }
int touchTargetLeft(){return touchTarget(left_line_sensor);}
int touchTargetRight(){return touchTarget(right_line_sensor);}
int touchRoad(int i){ return (i > 600 && i < 900); }
int touchRoadLeft(){ return touchRoad(left_line_sensor); }
int touchRoadRight(){ return touchRoad(right_line_sensor); }

void targetReached(){

	// fullSpeed();
	startRightWheel();
	startLeftWheel();
	forwardWheel();

	// go straight
	long i = 150000;
	while(i--);

	backwardWheel();
	i = 180000;
	while(i--);

	// turn
	turnAround();

	forwardWheel();
	slow();

}