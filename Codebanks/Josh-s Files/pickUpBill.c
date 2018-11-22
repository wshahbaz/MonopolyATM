void pickUpBill()
//motora=suction
//motorb= end effector

{
	int DUM = 0;
	motor[motorB]=10;
	while(nMotorEncoder[motorB]*PI*3.2/180.0<3){}
	motor[motorB]=0;
	motor[motorA] = 10;
	while(nMotorEncoder[motorA]*PI*4/180.0<DUM){}
	motor[motorA]=0;
	motor[motorB]=-10;
	while(nMotorEncoder[motorB]*PI*3.2/180.0>0){}
	motor[motorB]=0;
}

task main()
{
	motor[motorB]=10;
	while(nMotorEncoder[motorB]*PI*3.2/180.0<3){}
	motor[motorB]=0;


}
