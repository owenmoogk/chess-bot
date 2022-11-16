
task main()
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	const int RED = colorRed;
	while(true)
	{
		if(SensorValue(S1) == RED)
		{
			displayBigTextLine(1,"RED");
		}
		else
		{
			displayBigTextLine(1,"NOT RED");
		}
	}


}
