/*
The gripper can be set for positions between 0 and 70.
  CAUTION:  The value to close the gripper is close to or at zero.  It
  differs based on the slight variations in gripper assembly.  If you
  hear the motor running while in the closed position, increase the
  commanded value slightly.
*/

#include "EV3Servo-lib-UW.c"

// Motor locations on interface board
const int SV_GRIPPER = 4;

// Demo types
const int CONTINUOUS = buttonUp;
const int GRIPPER = buttonEnter;
const int STANDARD = buttonDown;

int getButton()
{
    while (!getButtonPress(buttonAny))
    {}

    int buttonNum = 0;
    for (int buttonTest = 1; buttonTest <= (int)buttonLeft && buttonNum == 0; buttonTest++)
        if (getButtonPress(buttonTest))
            buttonNum = buttonTest;

    while (getButtonPress(buttonAny))
    {}

    return buttonNum;
}

void setDemoMotor(int motorChoice, int motorSetting)
{
		if (motorChoice == CONTINUOUS)
		{
			// set servo motors with desired speed
			setServoSpeed(S1,SV_CSERVO1, motorSetting);
			setServoSpeed(S1,SV_CSERVO2, motorSetting);
		}
		else if (motorChoice == STANDARD)
		{
			// set servo motor with desired position
			setServoPosition(S1,SV_STANDARD,motorSetting);
		}
		else
		{
			// set gripper with desired position
			setGripperPosition(S1,SV_GRIPPER,motorSetting);
		}
}

task main()
{
	// configure servo controller port
	SensorType[S1] = sensorI2CCustom9V;

	getButton();

	// select demo
	int choice = getButton();

	// initial setting for motors
	int motorSetting = 0;
	if (choice == GRIPPER)
		motorSetting = 40;

	// keep looping until the orange button is pressed
	while (!getButtonPress(buttonEnter))
	{
		// adjust setting up or down
		if (getButtonPress(buttonUp))
		{
			motorSetting += 1;
			setDemoMotor(choice,motorSetting);
		}
		else if (getButtonPress(buttonDown))
		{
			motorSetting -= 1;
			setDemoMotor(choice,motorSetting);
		}
		displayBigTextLine(12, "setting: %d      ", motorSetting);
		wait1Msec(300);
	}

	eraseDisplay();
	displayBigTextLine(0,"Done");

	// move motors to stop/neutral positions
	setServoPosition(S1,SV_STANDARD,0);
	resetGripper(S1,SV_GRIPPER);
	wait1Msec(10000);
}
