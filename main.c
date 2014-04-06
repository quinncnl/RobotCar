#include  <msp430g2553.h>
#include  <stdlib.h>
#include  "adc.h"
#include  "utils.h"
#include  "drive.h"
#include  "main.h"

int leftSensor, rightSensor;

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
	mainLoop();
}

int arrTurnToBanque[10] = {0, 1, 1, -1, -1, 0};
void routeToBank(){
	path.posTurn = 5;
	path.arrTurn = arrTurnToBanque;
	mainLoop();
}

int arrTurnToPoste[10] = {0, 0, 1, -1, 1, -1, 0, 0};
void routeToPoste(){
	path.posTurn = 7;
	path.arrTurn = arrTurnToPoste;
	mainLoop();
}

int arrTurnToHospital[10] = {1, 0, -1, -1, 1, 1, 0, -1, 0, 0};
void routeToHospital(){
	path.posTurn = 9;
	path.arrTurn = arrTurnToHospital;
	mainLoop();
}

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
   
	initTA();
 	initPins();

	while(1){
		determineLeftSensorValue();
	}

	// If you want to detect left sensor value, comment out the line below.
	_BIS_SR(CPUOFF + GIE);
}

void mainLoop(){
	wait();

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
			if (path.posTurn == -1)
				fullSpeed();
			else 
				slow();
	
		}

	}
}
