void zeroGantry()
{
	motor[motorA] = -40;
	while(SensorValue(S1)!=1){}
	motor[motorA]=0;
}

void GantryTransverse(int position)
{
	zeroGantry();
	nMotorEncoder[motorA]=0;
	if (position==1)
	{
		motor[motorA]=30;
		while(nMotorEncoder[motorA]*PI*3/180.0<6.25){}
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==2)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<12.5){}
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==3)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<22.5){}
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==4)
	{
		motor[motorA]=50;
		while(nMotorEncoder[motorA]*PI*3/180.0<32.1){}//21.5+9.6+1.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==5)
	{
		motor[motorA]=50;
		while(nMotorEncoder[motorA]*PI*3/180.0<38.6){}//21.5+9.6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==6)
	{
		motor[motorA]=50;
		while(nMotorEncoder[motorA]*PI*3/180.0<44.6){}//21.5+9.6+6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==7)
	{
		motor[motorA]=50;
		while(nMotorEncoder[motorA]*PI*3/180.0<50.6){}//21.5+9.6+6+6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
}

void pickUpBill()
//motora=suction
//motorb= end effector

{
	nMotorEncoder[motorB]=0;

	motor[motorB]=-20;
	while(nMotorEncoder[motorB]*PI*3.2/180.0>-14){}
	motor[motorB]=0;
	nMotorEncoder[motorC]=0;
	motor[motorC] = -30;
	while(nMotorEncoder[motorC]*PI/180.0>-1.2){}
	motor[motorC]=0;
	motor[motorB]=20;
	while(nMotorEncoder[motorB]*PI*3.2/180.0<0.6){}
	motor[motorB]=0;
	wait10Msec(300);
	motor[motorC]=30;
	while(nMotorEncoder[motorC]*PI/180.0<0){}
	motor[motorC]=0;




}



task main()
{
for (int count=1;count<8;count++)
	GantryTransverse(count);
	pickUpBill();



}
