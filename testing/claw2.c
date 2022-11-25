#include "EV3Servo-lib-UW.c"
task main()
{
	const int SV_GRIPPER = 4;
	const int DELAY = 3000;

	while (true)
	{
	setGripperPosition(S1,SV_GRIPPER,70);
	wait1Msec(DELAY);
	setGripperPosition(S1,SV_GRIPPER,10);
	wait1Msec(DELAY);

	}


}
