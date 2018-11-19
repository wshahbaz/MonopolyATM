const int START_BALANCE = 1500;
const int MAX_PLAYERS = 4;
const int COLOUR_CARD = 0;
const int COLOUR_BILL = 1;
const int TOUCH_ENC_ZERO = 2;


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

	while(!getButtonPress(buttonUp) && !getButtonPress(buttonLeft) && !getButtonPress(buttonRight) && !getButtonPress(buttonDown)){}

	if(getButtonPress(buttonUp))
	{
		while (getButtonPress(buttonUp)){}
		numPlayers = 1;
	}

	else if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonLeft)){}
		numPlayers = 2;
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonRight)){}
		numPlayers = 3;
	}

	else
	{
		while (getButtonPress(buttonDown)){}
		numPlayers = 4;
	}

	for (int index = 0; index < numPlayers; index++)
	{
		isPlaying[index] = true;
		accountBalance[index] = START_BALANCE;
	}
}

void setCurrPlayer(int & currPlayer, bool * isPlaying)
{
	int readPlayer = -1;
	currPlayer = -1;
	while(currPlayer == -1)
	{
		if (SensorValue[COLOUR_CARD] == (int)colorRed)
			readPlayer = 0;
		else if (SensorValue[COLOUR_CARD] == (int)colorBlue)
			readPlayer = 1;
		else if (SensorValue[COLOUR_CARD] == (int)colorYellow)
			readPlayer = 2;
		else if (SensorValue[COLOUR_CARD] == (int)colorGreen)
			readPlayer = 3;

		if (readPlayer != -1 && isPlaying[readPlayer] == 1)
			currPlayer = readPlayer;
	}
}

void displayMainMenu(int currPlayer, int * accountBalance)
{
	eraseDisplay();
	displayString(2, "Currently processing transactions for player %d, account Balance: %d", currPlayer, accountBalance[currPlayer]);
	displayString(4, "1: Withdraw");
	displayString(5, "2: Deposit");
	displayString(6, "3: Transfer");
	displayString(7, "4: Declare Bankrupcty");
	displayString(8, "5: Cancel Transaction");
}

void declareBankruptcy(int currentPlayer, int & numPlayers, bool * isPlaying, int * accountBalance, bool & continueTransaction)
{
	eraseDisplay();

	displayString(2,"Are you sure you want to declare bankruptcy and leave the game?");
	displayString(4, "1: Yes, my friends have screwed me over...");
	displayString(5, "2: No, get me out of here!");

	while(!getButtonPress(buttonUp) || getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp))
	{
		while(getButtonPress(buttonUp)){}

		eraseDisplay();
		displayString(2,"You have fallen to the almighty hand of capitalism!");

		isPlaying[currentPlayer] = false;
		accountBalance[currentPlayer] = 0;
		numPlayers--;
		continueTransaction = false;
	}

	else
		while (getButtonPress(buttonLeft)){}
}

void promptContinue(bool & continueTransaction)
{
	eraseDisplay();
	displayString(2,"Do you want to continue with your transactions?");
	displayString(4, "1: Yes");
	displayString(5, "2: No");

	while (!getButtonPress(buttonUp) || getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp))
		while (getButtonPress(buttonUp)){}

	else
	{
		while (getButtonPress(buttonLeft)){}
		continueTransaction = false;
	}
}

void doTransaction(int currentPlayer, int & numPlayers, bool * isPlaying, int * accountBalance, bool & continueTransaction)
{
	while(!getButtonPress(buttonAny)){}

	if(getButtonPress(buttonUp))
	{
		while (getButtonPress(buttonUp)){}
		//withdraw(parameters);
	}

	else if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonLeft)){}
		//deposit(parameters);
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonRight)){}
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

void declareWinner(bool * isPlaying)
{
	eraseDisplay();

	for (int index = 0; index < 4; index++)
		if (isPlaying[index] == 1)
			displayString(2, "Player %d has won the Monopoly!", index);

	wait10Msec(1000);
}

task main()
{
	sensorConfig();

	int numPlayers = 0;
	bool isPlaying[MAX_PLAYERS] = {false, false, false, false};
	int accountBalance[MAX_PLAYERS] = {0,0,0,0};

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
		} while (continueTransaction == 1);
	}

	declareWinner(isPlaying);
}
