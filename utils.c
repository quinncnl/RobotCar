#include  <msp430g2553.h>
#include  "utils.h"


int front_sensor = 0;
int left_line_sensor = 0;
int right_line_sensor = 0;

void wait(){ long i = 75000; while(i--); }
void delay(long i) {while(i--);}
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
void slowLeftWheel(){ TA1CCR1 = 400; }
void slowRightWheel(){ TA1CCR2 = 400; }
void slow() { slowRightWheel(); slowLeftWheel(); }
void stop(){ stopLeftWheel(); stopRightWheel(); }

void onLED0(){ P1OUT |= BIT0; }
void offLED0(){	P1OUT &=~ BIT0; }
void onLED6(){ P1OUT |= BIT6; }
void offLED6(){	P1OUT &=~ BIT6; }

void debugWithLED(int i) {
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
void readLineSensor() {
	left_line_sensor = read_adc(1);
	right_line_sensor = read_adc(2);
}

int front_sensor;
void readFrontSensor(){
	front_sensor = read_adc(4);
}

int meetObstacle(){
	if (front_sensor > 400) 
		return 1;
	else 
		return 0;
}


void fixStraigt(){
	forwardWheel();
	fullSpeed();
	delay(26000);
}

void forceTurnLeft(long d){
	backwardLeftWheel();
	forwardRightWheel();
	slow();

	delay(d);
	stop();
}
void turnLeft(){

	backwardLeftWheel();
	forwardRightWheel();
	slow();

	int i = 90000;

	while(i--) {

		readLineSensor();

		if (!touchRoadLeft()) {

			return;
		}

		if (!touchRoadRight()) {

			return;
		}
	}
}

void turnRight(){

	forwardLeftWheel();
	backwardRightWheel();
	slow();

	int i = 90000;

	while(i--) {

		readLineSensor();

		if (!touchRoadLeft()) {

			return;
		}

		if (!touchRoadRight()) {

			return;
		}
	}
}

void determineValue(int v){
	if (v < 200)	{
		debugWithLED(0);
	}
	else if (v < 400)	{
		debugWithLED(1);
	}
	else if (v < 600)	{
		debugWithLED(2);
	}
	else {
		debugWithLED(3);
	}
}
void determineLeftSensorValue(){
	determineValue(left_line_sensor);
}

int touchTarget(int s){ return (s > 900); }
int touchTargetLeft(){return touchTarget(left_line_sensor);}
int touchTargetRight(){return touchTarget(right_line_sensor);}
int touchRoad(int i){ return (i > 600 && i < 900); }
int touchRoadLeft(){ return touchRoad(left_line_sensor); }
int touchRoadRight(){ return touchRoad(right_line_sensor); }
int touchStart(){ return (left_line_sensor > 900); }

void targetReached(){

	fullSpeed();
	forwardWheel();

	// go straight
	delay(90000);
	stop();
	delay(50000);

	fullSpeed();
	backwardWheel();
	delay(100000);

	slow();
	forceTurnLeft(80000);
	turnLeft();

}

void enterGarage(){
	fullSpeed();
	forwardWheel();

	// go straight
	delay(40000);

	forceTurnLeft(170000);

	stop();
}