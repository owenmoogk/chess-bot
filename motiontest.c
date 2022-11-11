
task main()
{
	nMotorEncoder[motorA] = 0;
	while(true)
	{
		if (getButtonPress(buttonLeft)){
			motor[motorA] = 20;
		}
		else if (getButtonPress(buttonRight)){
			motor[motorA] = -20;
		}
		else{
			motor[motorA] = 0;
		}

		displayString(5, "The encoder value is: %f", nMotorEncoder[motorA]);

	}


}
