/*
	Carl-Bot

	Owen Moogk
	Eidan Erlich
	Shabd Gupta
	Richard Wang

	November 25, 2022
	Version 58.3

	This code is used to operate the 'carl-bot' chess robot.
	We poured lots of effort into this project and are very proud of the result.

	Assumptions:
		- On startup, the program was run previously. This is so the y-axis is at the zero point
			and the claw lowering is at the zero point as well.
*/

// include the claw library
#include "EV3Servo-lib-UW.c"

// HUGE ASSUMPTION:
// on startup, the y-axis (pulley) MUST be at the same point (probably zero)
// otherwise we have to change the init function, I don't think we'll have a touch sensor there to 'zero'

// sensor constants
const int TOUCH = S4;
const int COLOR = S3;
const int X_ZERO_TOUCH = S2;
const int RED = colorRed;
const int SENSOR_WAIT_TIME = 50; // ms

// motor constants
const int X_MOTOR = motorD;
const int X_MOTOR_POWER = 30;
const int X_AXIS_OFFSET = 250; // ms
const int X_H_OFFSET = 150; // ms, offset error in the 'h' cell
const int CAPTURE_WAIT_TIME = 900; // ms

const int Y_MOTOR = motorA;
const int Y_MOTOR_POWER = 100;
const int Y_CELL_CLICKS = 1175;
const int Y_ENCODER_OFFSET = 110; // mss

// claw constants
const int CLAW_ACTUATION_MOTOR = motorB;
const int CLAW_MOTOR = S1;
const int CLAW_CLOSE = 10;
const int CLAW_OPEN = 70;
const int CLAW_WAIT_TIME = 500;
const int CLAW_LOWER_CLICKS = 280;
const int CLAW_LOWER_CLICKS_TALL = 185;
const int CLAW_ACTUATION_POWER = 10;
const int CLAW_ACTUATION_OFFSET = 30;
const int SV_GRIPPER = 4;

// display constants
const int ASCII_START = 65;
const int ASCII_END = 72;

// where the taken peices go
const int END_X = 7;
const int END_Y = 0;

// the board is 8x8
const int BOARD_SIZE = 8;
const int CHESS_CLOCK_INIT = 300; // seconds

// 2d array with the board location
// bk, wk, k, q, b, r, p, n for knight
string board[BOARD_SIZE][BOARD_SIZE];

// sensor configuration
void configureSensors()
{
	SensorType[TOUCH] = sensorEV3_Touch;
	wait1Msec(SENSOR_WAIT_TIME);
	SensorType[COLOR] = sensorEV3_Color;
	wait1Msec(SENSOR_WAIT_TIME);
	SensorMode[COLOR] = modeEV3Color_Color;
	wait1Msec(SENSOR_WAIT_TIME);
	nMotorEncoder[Y_MOTOR] = 0;
}

// get the input for a cell
void getCellInput(int &currentLetter, int &currentNumber, bool firstLine)
{
	while (true)
	{
		// the displays cast the current letter to a character via the ascii values
		if (firstLine)
			displayBigTextLine(2, "Current: %c%d", currentLetter, currentNumber);
		else
			displayBigTextLine(4, "Final: %c%d", currentLetter, currentNumber);

		// wait until the button is pressed
		while (!getButtonPress(buttonAny))
		{ }

		// if the user hits enter, we return from the function
		if (getButtonPress(buttonEnter))
		{
			while(getButtonPress(buttonEnter))
			{ }
			return;
		}

		// adjust the numbers and letters
		if (getButtonPress(buttonLeft))
			currentLetter -= 1;
		if (getButtonPress(buttonRight))
			currentLetter += 1;
		if (getButtonPress(buttonUp))
			currentNumber += 1;
		if (getButtonPress(buttonDown))
			currentNumber -= 1;
		// wait until the button is released
		while (getButtonPress(buttonAny))
		{ }

		// wrapping around the board
		// ie, if you start at 1 and hit back, it will go to position 8
		// this makes it easier, instead of scrolling through everything
		if (currentLetter < ASCII_START)
			currentLetter = ASCII_END;
		if (currentLetter > ASCII_END)
			currentLetter = ASCII_START;
		if (currentNumber < 1)
			currentNumber = BOARD_SIZE;
		if (currentNumber > BOARD_SIZE)
			currentNumber = 1;
	}
}

// getting the user input (pbr)
bool getInput(int &currentLetter, int &currentNumber, int &moveToLetter, int &moveToNumber)
{
	// first ask the user if they want to continue (or resign)
	displayBigTextLine(1,"Continue?");

	bool exit = false;
	bool doContinue = true;

	// while the user is entering a move
	while(!exit)
	{
		// if the user hits enter, then exit out of the loop
		if (getButtonPress(buttonEnter))
			exit = true;

		// if they hit left or right switch from resign / continue
		else if (getButtonPress(buttonRight) || getButtonPress(buttonLeft))
		{
			doContinue = !doContinue;
			while(getButtonPress(buttonRight) || getButtonPress(buttonLeft))
			{ }
		}

		// if we are continuing, display yes, otherwise no
		if (doContinue)
			displayBigTextLine(3,"Yes");
		else
			displayBigTextLine(3,"No");
	}

	// wait until enter is released to continue
	while(getButtonPress(buttonEnter))
	{ }

	eraseDisplay();

	// if we resign, return false
	if (!doContinue)
		return false;

	// letter stored in ascii, number stored as int
	currentLetter = ASCII_START;
	currentNumber = 1;
	moveToLetter = ASCII_START;
	moveToNumber = 1;

	// get the input for both the current and destination cells
	getCellInput(currentLetter, currentNumber, true);
	getCellInput(moveToLetter, moveToNumber, false);

	// after we get the input, we need to rectify the numbers
	// so that they can be used as indicies of the array
	currentLetter -= ASCII_START;
	moveToLetter -= ASCII_START;
	currentNumber -= 1;
	moveToNumber -= 1;

	// we also need to transpose this so it corresponds correctly with the board
	currentLetter = (BOARD_SIZE-1)-currentLetter;
	moveToLetter = (BOARD_SIZE-1)-moveToLetter;

	eraseDisplay();
	return true;
}

// zeroing function
void zero()
{
	// zero the x axis (until the touch sensor is triggered)
	motor[X_MOTOR] = X_MOTOR_POWER;
	while (!SensorValue[X_ZERO_TOUCH])
	{	}
	motor[X_MOTOR] = 0;

	// zeroing the y axis (until motor encoder is 0)
	motor[Y_MOTOR] = Y_MOTOR_POWER;
	while (nMotorEncoder[Y_MOTOR] < 0)
	{ }
	motor[Y_MOTOR] = 0;

	// default to setting the gripper to open state
	setGripperPosition(CLAW_MOTOR,SV_GRIPPER,CLAW_OPEN);
}

// move to cell
void moveToCell(int currX, int currY, int x, int y, bool firstMove)
{
	// calculate the change in y and x
	int travelX = currX - x;
	int travelY = currY - y;

	// we need these to determine the direction of travel
	int directionX = -1;
	int directionY = 1;

	// if we are going backwards, multiply by -1
	if (travelX < 0)
		directionX *= -1;
	if (travelY < 0)
		directionY *= -1;

	// if we are moving on the x axis
	if (travelX != 0)
	{
		// move using the color sensor, a specified number of times
		for (int count = 0; count < abs(travelX) + 1; count++)
		{
			// turn the motor on
			motor[X_MOTOR] = X_MOTOR_POWER * directionX;
			wait1Msec(50);
			// while the color isn't red
			while(SensorValue(COLOR) != RED)
			{ }
			// if we are not on the last pass
			if (count != abs(travelX))
			{
				// while the color is red
				while(SensorValue(COLOR) == RED)
				{ }
			}
		}

		// this is tuned for color sensor positioning, need to delay to center
		wait1Msec(X_AXIS_OFFSET);
		// turn off the motor
		motor[X_MOTOR] = 0;
	}

	// if we are not moving in the X, and it is picking up the peice, we need to offset the hardware
	else if (firstMove)
	{
		motor[X_MOTOR] = X_MOTOR_POWER;
		wait1Msec(X_H_OFFSET);
		motor[X_MOTOR] = 0;
	}

	// move the y axis forward slightly to prevent motor encoder errors
	motor[Y_MOTOR] = -Y_MOTOR_POWER;
	wait1Msec(Y_ENCODER_OFFSET);

	// move the motor encoder to the correct location
	motor[Y_MOTOR] = Y_MOTOR_POWER * directionY;
	if (directionY == 1)
	{
		while(abs(nMotorEncoder(Y_MOTOR)) > abs(Y_CELL_CLICKS * y) && nMotorEncoder(Y_MOTOR) < 0)
		{ }
	}
	if (directionY == -1)
	{
		while(abs(nMotorEncoder(Y_MOTOR)) < abs(Y_CELL_CLICKS * y) && nMotorEncoder(Y_MOTOR) < 0)
		{ }
	}

	motor[Y_MOTOR] = 0;
}

// picking up the peice when the claw is in place
void pickUpPiece(int x2, int y2)
{
	// if we have a king or queen, we need to lower the claw less
	int lowerDistance = CLAW_LOWER_CLICKS;
	if (stringFind(board[y2][x2], "K") != -1 || stringFind(board[y2][x2], "Q") != -1)
		lowerDistance = CLAW_LOWER_CLICKS_TALL;

	// make sure the claw is open
	setGripperPosition(CLAW_MOTOR,SV_GRIPPER,CLAW_OPEN);

	// lower the claw
	nMotorEncoder[CLAW_ACTUATION_MOTOR] = 0;
	motor[CLAW_ACTUATION_MOTOR] = -CLAW_ACTUATION_POWER;
	while(abs(nMotorEncoder[CLAW_ACTUATION_MOTOR]) < lowerDistance)
	{ }
	motor[CLAW_ACTUATION_MOTOR] = 0;

	// close the claw
	setGripperPosition(CLAW_MOTOR,SV_GRIPPER,CLAW_CLOSE);
	wait1Msec(CLAW_WAIT_TIME);

	// raise the claw back up
	nMotorEncoder[CLAW_ACTUATION_MOTOR] = 0;
	motor[CLAW_ACTUATION_MOTOR] = CLAW_ACTUATION_POWER;
	while(abs(nMotorEncoder[CLAW_ACTUATION_MOTOR]) < lowerDistance)
	{ }
	motor[CLAW_ACTUATION_MOTOR] = 0;
}

// putting down the peice when the claw is in place
void putDownPiece(int x2, int y2)
{
	// change lower distance depending on the peice (king / queen)
	int lowerDistance = CLAW_LOWER_CLICKS;
	if (stringFind(board[y2][x2], "K") != -1 || stringFind(board[y2][x2], "Q") != -1)
		lowerDistance = CLAW_LOWER_CLICKS_TALL;

	// lower the claw
	nMotorEncoder[CLAW_ACTUATION_MOTOR] = 0;
	motor[CLAW_ACTUATION_MOTOR] = -CLAW_ACTUATION_POWER;
	while(abs(nMotorEncoder[CLAW_ACTUATION_MOTOR]) < lowerDistance-CLAW_ACTUATION_OFFSET)
	{ }
	motor[CLAW_ACTUATION_MOTOR] = 0;

	// open the claw
	wait1Msec(CLAW_WAIT_TIME);
	setGripperPosition(CLAW_MOTOR, SV_GRIPPER, CLAW_OPEN);
	wait1Msec(CLAW_WAIT_TIME);

	// raise the claw back up
	nMotorEncoder[CLAW_ACTUATION_MOTOR] = 0;
	motor[CLAW_ACTUATION_MOTOR] = CLAW_ACTUATION_POWER;
	while(abs(nMotorEncoder[CLAW_ACTUATION_MOTOR]) < lowerDistance-CLAW_ACTUATION_OFFSET)
	{ }
	motor[CLAW_ACTUATION_MOTOR] = 0;
}

// function to capture a piece at a location
void capturePiece(int x2, int y2)
{
	// move to the cell with the captured piece
	moveToCell(0,0,x2,y2, true);

	// pick up the piece
	pickUpPiece(x2,y2);

	// move to the capture location
	moveToCell(x2,y2,END_X,END_Y, false);

	// drive a bit further, off the board
	motor[X_MOTOR] = X_MOTOR_POWER;
	wait1Msec(CAPTURE_WAIT_TIME);
	motor[X_MOTOR] = 0;

	// put the piece back down
	putDownPiece(x2,y2);
	return;
}

// execute move fucntion
bool movePiece(int x1, int y1, int x2, int y2)
{
	// if the start location and end location are the same, invalid move
	if ((x1 == x2 && y1 == y2) )
	{
		displayBigTextLine(1, "Invalid Move");
		displayBigTextLine(3, "Try again");
		wait1Msec(3000);
		return false;
	}

	// if the board has another piece here, capture it
	if (board[y2][x2] != "")
	{
		capturePiece(x2,y2);
		zero();
		wait1Msec(300);
	}

	// move to the location of the piece
	moveToCell(0,0,x1,y1, true);

	// pick it up
	pickUpPiece(x1,y1);

	// move to the destination cell
	moveToCell(x1,y1,x2,y2, false);

	// put down the piece
	putDownPiece(x1,y1);

	// update the board with the new location of the piece
	board[y2][x2] = board[y1][x1];
	board[y1][x1] = "";

	// zero the system again
	zero();

	return true;
}

// initializes the board with the starting location of all the peices
void boardInitState()
{

	// loops through every index in the board array
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {

    	// start with nothing, append to string
      string value = "";

      // row 0 and 1 are white, 6 and 7 are black
      if (row <= 1)
        value = "W";
      else if (row >= 6)
        value = "B";

      // if row 1 or 6, it is a pawn
      if (row == 1 || row == 6)
        value = value + "P";

      // otherwise...
      else if (row == 7 || row == 0)

    		// rook in corners
        if (col == 0 || col == 7)
          value = value + "R";

        // then knight
        else if (col == 1 || col == 6)
          value = value + "N";

        // then bishop
        else if (col == 2 || col == 5)
          value = value + "B";

        // queen
        else if (col == 3)
          value = value + "Q";

        // king
        else if (col == 4)
          value = value + "K";

      // update the board with these values
      board[row][col] = value;
    }
  }
}

// when the user wants to shut down
void shutDownProcedure(bool whiteLoses, int endCode)
{
	// display the winner
	if (whiteLoses)
		displayBigTextLine(2,"Black Wins!");
	else
		displayBigTextLine(2,"White Wins!");

	// display how they won (different codes)
	if (endCode == 0)
		displayBigTextLine(4,"By Resignation");
	if (endCode == 1)
		displayBigTextLine(4, "On Time");

	// zero the system
	zero();

	// wait 5 seconds before the program ends
	wait1Msec(5000);
}

// main function
task main()
{

	// configure all the sensors
	configureSensors();

	// initalize the board with the starting values
	boardInitState();

	// chess timers, seconds left
	int whiteTime = CHESS_CLOCK_INIT;
	int blackTime = CHESS_CLOCK_INIT;

	// keep track of the players turn
	bool whiteTurn = true;

	// while loop to continue making moves
	bool playing = true;
	while(playing)
	{

		// reset the timer
		clearTimer(T1);

		// display how much time the user has left
		if (whiteTurn)
			displayBigTextLine(7,"Time Left:%d:%d", whiteTime / 60, whiteTime % 60);
		else
			displayBigTextLine(7,"Time Left:%d:%d", blackTime / 60, blackTime % 60);

		// initalize the input variables
		int x1,y1,x2,y2;

		// get the input, determine if the user wants to continue
		playing = getInput(x1,y1,x2,y2);

		// update the chess clock
		if (whiteTurn)
			whiteTime -= time1[T1] / 1000;
		else
			blackTime -= time1[T1] / 1000;

		// if the user is out of time, go to shut down procedure
		if (whiteTime == 0 || blackTime == 0)
		{
			playing = false;
			shutDownProcedure(whiteTurn, 1);
		}
		else
		{
			// if the user wants to stop, shut down
			if (!playing)
				shutDownProcedure(whiteTurn, 0);
		}

		// if we are continuing to play...
		if (playing)
		{

			// zero the board
			zero();

			// move the piece to the desired location
			movePiece(x1,y1,x2,y2);

			// zero the board again
			zero();

			// update who's turn it is
			whiteTurn = !whiteTurn;
		}
	}
}
