const int START_BALANCE = 1500;
const int MAX_PLAYERS = 5;
const int MAX_CASH = 10000;

//Motors
const int GANTRY_MOTOR = 0;
const int VERT_ACTUATOR_MOTOR = 1;
const int END_EFFECTOR_MOTOR = 2;
const int CONVEYER_MOTOR = 3;

//Sensors
const int COLOUR_CARD = 0;	//port S1
const int COLOUR_BILL = 1;	//port S2
const int TOUCH_ENC_ZERO = 2;		//port S3


void sensorConfig()
{
	SensorType[COLOUR_CARD] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOUR_CARD] = modeEV3Color_Color;
	wait1Msec(50);

	SensorType[COLOUR_BILL] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOUR_BILL] = modeEV3Color_RGB_Raw;
	wait1Msec(50);

	SensorType[TOUCH_ENC_ZERO] = sensorEV3_Touch;
}

void setupPlayers(int & numPlayers, int * accountBalance, bool * isPlaying)
{
	eraseDisplay();

	displayString(2,"Enter number of players");
	displayString(4,"2 Player");
	displayString(5,"3 Player");
	displayString(6,"4 Player");


	//wait for button press
	while(!getButtonPress(buttonUp) && !getButtonPress(buttonLeft) && !getButtonPress(buttonRight) && !getButtonPress(buttonDown)){}


	if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonAny)){}
		numPlayers = 2;
		eraseDisplay();
		displayString(2, "TWO PLAYER OPTION SELECTED");
		wait1Msec(1000);
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonAny)){}
		numPlayers = 3;
		eraseDisplay();
		displayString(2, "THREE PLAYER OPTION SELECTED");
		wait1Msec(1000);
	}

	else
	{
		while (getButtonPress(buttonAny)){}
		numPlayers = 4;
		eraseDisplay();
		displayString(2, "FOUR PLAYER OPTION SELECTED");
		wait1Msec(1000);
	}

	//update player balance and playing arrays
	for (int index = 1; index < numPlayers; index++)
	{
		isPlaying[index] = true;
		accountBalance[index] = START_BALANCE;
	}
}

void setCurrPlayer(int& currPlayer, bool* isPlaying)
{
	int readPlayer = -1;
	currPlayer = -1;
	//keep reading until seeing valid player
	while(currPlayer == -1)
	{
		if (SensorValue[COLOUR_CARD] == (int)colorRed)
			readPlayer = 1;
		else if (SensorValue[COLOUR_CARD] == (int)colorBlue)
			readPlayer = 2;
		else if (SensorValue[COLOUR_CARD] == (int)colorYellow)
			readPlayer = 3;
		else if (SensorValue[COLOUR_CARD] == (int)colorGreen)
			readPlayer = 4;

		if (readPlayer != -1 && isPlaying[readPlayer] == true)
			currPlayer = readPlayer;
	}
}

void displayMainMenu(int currPlayer, int* accountBalance)
{
	eraseDisplay();
	displayString(2, "Currently processing transactions for player %d, account Balance: %d", currPlayer, accountBalance[currPlayer]);
	displayString(4, "1: Withdraw");
	displayString(5, "2: Deposit");
	displayString(6, "3: Transfer");
	displayString(7, "4: Declare Bankrupcty");
	displayString(8, "5: Cancel Transaction");
}

void declareBankruptcy(int currentPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
{
	eraseDisplay();

	displayString(2,"Are you sure you want to declare bankruptcy and leave the game?");
	displayString(4, "1: Yes");
	displayString(5, "2: No");

	//wait for user to make decision
	while(!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp))
	{
		while(getButtonPress(buttonAny)){}

		eraseDisplay();
		/*
		run deposit function to get cash
		displayString(2, "Deposit any remaining cash at hand");
		wait1Msec(1500);
		depositCash( . . . )
		*/
		displayString(2,"Better luck next time");
		wait1Msec(1500);

		isPlaying[currentPlayer] = false;
		accountBalance[currentPlayer] = 0;
		numPlayers--;
		continueTransaction = false;
	}
	else
		while (getButtonPress(buttonAny)){}
}

void promptContinue(bool& continueTransaction)
{
	eraseDisplay();
	displayString(2,"Do you want to continue with your transactions?");
	displayString(4, "1: Yes");
	displayString(5, "2: No");

	while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp))
		while (getButtonPress(buttonAny)){}

	else
	{
		while (getButtonPress(buttonAny)){}
		continueTransaction = false;
	}
}

void doTransaction(int currentPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
{
	while(!getButtonPress(buttonAny)){}

	if(getButtonPress(buttonUp))
	{
		while (getButtonPress(buttonAny)){}
		//withdraw(parameters);
	}

	else if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonAny)){}
		//deposit(parameters);
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonAny)){}
		//transfer(parameters);
	}

	else if(getButtonPress(buttonDown))
	{
		while (getButtonPress(buttonDown)){}
		declareBankruptcy(currentPlayer, numPlayers, isPlaying, accountBalance, continueTransaction);
	}

	else
	{
		while (getButtonPress(buttonEnter)){}
		promptContinue(continueTransaction);
	}
}

void declareWinner(bool* isPlaying)
{
	eraseDisplay();

	for (int index = 0; index < 4; index++)
		if (isPlaying[index] == 1)
			displayString(2, "Player %d has won the Monopoly!", index);
	wait10Msec(2000);
	eraseDisplay();
	displayString(2, "Now please deposit any remaining cash into the atm");
	//deposit (parameters)
}

task main()
{
	sensorConfig();

	int numPlayers = 0;
	bool isPlaying[MAX_PLAYERS] = {true, false, false, false, false};
	int accountBalance[MAX_PLAYERS] = {MAX_CASH, 0,0,0,0};

	setupPlayers(numPlayers, accountBalance, isPlaying);

	while(numPlayers > 1)
	{
		eraseDisplay();
		displayString(2, "Please swipe your card to start a transaction");

		int currPlayer = -1;
		setCurrPlayer(currPlayer,isPlaying);

		bool continueTransaction = true;
		do
		{
			displayMainMenu(currPlayer, accountBalance);
			doTransaction(currPlayer, numPlayers, isPlaying, accountBalance, continueTransaction);
			if(continueTransaction)
				promptContinue(continueTransaction);
		} while (continueTransaction);
	}

	declareWinner(isPlaying);
}
