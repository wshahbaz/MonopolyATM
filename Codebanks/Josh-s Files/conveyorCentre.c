void conveyorSend(int position)
{
	if (position)// send to middle and color sensor
	{
		motor[motorA]=30;
		while(nMotorEncoder[motorA]*PI*1.75/180.0<6.5){}
		motor[motorA]=0;
		wait10Msec(100);
	}
	else
	{
		motor[motorA]=30;
		while(nMotorEncoder[motorA]*PI*1.75/180.0<26){}
		motor[motorA]=0;
		wait10Msec(100);
	}
}

void conveyorReturn()
{
	motor[motorA]=-30;
	while(nMotorEncoder[motorA]*PI*1.75/180.0>0){}
	motor[motorA]=0;
	wait10Msec(100);
	}





task main()
{
	conveyorSend(1);
	conveyorReturn();
	conveyorSend(0);
	conveyorReturn();



}
