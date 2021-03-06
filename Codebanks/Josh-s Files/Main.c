const int START_BALANCE = 1500;
const int MAX_PLAYERS = 5;
const int MAX_CASH = 13720;
const int NUM_BINS = 8;
const int DISPLAYSTART = 2;

const int DISPLAY_WAIT = 1000;

//Bill indices
const int BILL_1 = 0;
const int BILL_5 = 1;
const int BILL_10 = 2;
//This is used in the specifc case of going to bin location
const int OUTPUT_TRAY_LOCATION = 3;
const int BILL_20 = 4;
const int BILL_50 = 5;
const int BILL_100 = 6;
const int BILL_500 = 7;

//Motors
const int GANTRY_MOTOR = 0;
const int VERT_ACTUATOR_MOTOR = 1;
const int END_EFFECTOR_MOTOR = 2;
const int CONVEYER_MOTOR = 3;

//Sensors
const int COLOUR_CARD = 0;	//port S1
const int COLOUR_BILL = 1;	//port S2
const int TOUCH_ENC_ZERO = 2;		//port S3

//Tray lcoations
const int USER_PICKUP = 0;
const int COLOUR_SENSE_LOCATION = 1;

//function prototypes
//need to declare function stubs so functions are declared ahead of time
void withdraw(int& playerBalance, int* transactionBills);
void resetPlayerBalance(int currPlayer, int *playerBalances);
int receiveWithdrawBills(int& playerBalance, int* transactionBills, bool& isCancelled);
//void moveBillsOut(int* transactionBills);
void cancelTransaction(int* transactionBills, bool& isCancelled);
void clearChosenBills(int* transactionBills);
// Allows users to select bill amounts and stores the bill indices in an array
void getLowerOptions(int playerBalance, int* transactionBills, bool& isCancelled);
// Displays bill values 1,5,10,20 for user to select
void displayLowerOptions(int* transactionBills, int playerBalance);
// Displays higher bill values 50,100,500 for user to select
void displayHigherOptions(int* transactionBills, int playerBalance);
// Checks whether the transaction exceeds the player's balance
bool isValidTransaction(int playerBalance, int totalTransaction, int bill);
// Calculates the transaction amount given an array with bill indices
int calcTransactionAmount(int* transactionBills);
// Allows users to select bill amounts and stores the bill indices in an array
bool getHigherOptions(int playerBalance, int* transactionBills, bool& isCancelled);
bool isClearOrCancel(int* transactionBills);
void displayText_NoWait(string* text);
void displayText_Wait(string* text);
int calcRemainingCash(int transactionAmont, int balance);
void transfer(int transferor, int playersInGame, int* playerBalance, bool* isPlaying);
void waitButtonPress(int playersInGame);
void buttonPressValid(int playersInGame);
int getTransferee(int playersInGame, int * transferOption);
void transferAmount(int transferor, int transferee, int * accountBalance, bool & isTransferCancelled);
int getTransferAmount(int playerBalance, bool & isTransferCancelled);
void displayTransferOptions(int transferor, int * transferOption, bool * isPlaying);
void moveBillsOut(int* transactionBills);
void masterTransverse(int initial, int final);
void pickUpBill();
void GantryTransverse(int position);
void	dropBill();
void zeroGantry();
void sendTray(int trayLocation);
void conveyorReturn();
int senseBill(tSensors colorsensor);
int senseCard();
void deposit(int currPlayer, int* accountBalance, bool isEndGame);
void processDeposit(int* transactionBills);

void sensorConfig()
{
	SensorType[COLOUR_CARD] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOUR_CARD] = modeEV3Color_RGB_Raw;
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
	while(!getButtonPress(buttonLeft) && !getButtonPress(buttonRight) && !getButtonPress(buttonDown)){}

	if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonAny)){}
		numPlayers = 2;
		displayText_Wait("TWO PLAYER OPTION SELECTED");
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonAny)){}
		numPlayers = 3;
		displayText_Wait("THREE PLAYER OPTION SELECTED");
	}

	else
	{
		while (getButtonPress(buttonDown)){}
		numPlayers = 4;
		displayText_Wait("FOUR PLAYER OPTION SELECTED");
	}

	//update player balance and playing arrays
	for (int index = 1; index <= numPlayers; index++)
	{
		isPlaying[index] = true;
		accountBalance[index] = START_BALANCE;
	}
}

//new card colour sensing function
int senseCard()
{
	long red, green, blue;
	getColorRGB(COLOUR_CARD, red, green, blue);
	red=blue=green=0;
	getColorRGB(COLOUR_CARD, red, green, blue);

	int colorCount[MAX_PLAYERS] = {0,0,0,0,0};

	int numReadings = 0;

	while (numReadings < 10){
		//pink
			if(red<=140&&red>=20 && green<=150/100.0*red&&green>=36/100.0*red && blue<=70/100.0*red&&blue>=20/100.0*red)
				colorCount[1]++;
		//blue
			else if(red<=60/100.0*blue&&red>=38/100.0*blue && green<=70/100.0*blue&&green>=50/100.0*blue && blue<=30&&blue>=15)
				colorCount[2]++;
		//red
			else if(red<=80&&red>=30 && green<=20/100.0*red&&green>=3/100.0*red && blue<=20/100.0*red&&blue>=5/100.0*red)
				colorCount[3]++;
		//green
			else if(red<=70/100.0*green&&red>=50/100.0*green && green<=80&&green>=20 && blue<=70/100.0*green&&blue>=50/100.0*green)
				colorCount[4]++;
		//brown
				else if(red<=15&&red>=2 && green<=70/100.0*red&&green>=40/100.0*red && blue<=70/100.0*red&&blue>=40/100.0*red)
				colorCount[0]++;
	numReadings++;
	}

	int maxColorCount = 0;
	int maxColorIndex = 0;

	for (int index = 0; index < 5; index++)
		if (colorCount[index] > maxColorCount){
			maxColorCount = colorCount[index];
			maxColorIndex = index;
		}

	if (maxColorCount > 0)
		return maxColorIndex;
	return -1;
}

int senseBill(tSensors colorsensor)
{
	long red, green, blue;
	getColorRGB(colorsensor, red, green, blue);
	red=blue=green=0;
	getColorRGB(colorsensor, red, green, blue);

	int colorCount[NUM_BINS] = {0,0,0,0,0,0,0,0};

	int numReadings = 0;

	while (numReadings < 30){
		//pink - 1s
			if(red<=120&&red>=15 && green<=50/100.0*red&&green>=20/100.0*red && blue<=60/100.0*red&&blue>=20/100.0*red)
				colorCount[0]++;
		//purple - 5s
			else if(red<=35&&red>=9 && green<=100/100.0*red&&green>=50/100.0*red && blue<=140/100.0*red&&blue>=65/100.0*red)
				colorCount[1]++;
		//brown - 10s
			else if(red<=20&&red>=3 && green<=100/100.0*red&&green>=50/100.0*red && blue<=10&&blue>=2)
				colorCount[2]++;
		//blue -
			else if(red<=45/100.0*green&&red>=5/100.0*green && green<=100&&green>=12 && blue<=150/100.0*green&&blue>=60/100.0*green)
				colorCount[4]++;
		//orange -
			else if(red<=120&&red>=7 && green<=30/100.0*red&&green>=5/100.0*red && blue<=10&&blue>=3)
				colorCount[5]++;
		//yellow -
			else if(red<=130&&red>=20 && green<=100/100.0*red&&green>=60/100.0*red && blue<=65/100.0*red&&blue>=25/100.0*red)
				colorCount[6]++;
		//green -
			else if(red<=50/100.0*green&&red>=20/100.0*green && green<=60&&green>=6 && blue<=15&&blue>=3)
				colorCount[7]++;
	numReadings++;
	}

	int maxColorCount = 0;
	int maxColorIndex = 0;

	for (int index = 0; index < 8; index++)
		if (colorCount[index] > maxColorCount){
			maxColorCount = colorCount[index];
			maxColorIndex = index;
		}

	if (maxColorCount > 0)
		return maxColorIndex;
	//if it sees black
	return -1;
}

void setCurrPlayer(int& currPlayer, bool* isPlaying)
{
	int readPlayer = -1;
	currPlayer = -1;
	//keep reading until seeing valid player
	while(currPlayer == -1)
	{
		readPlayer = senseCard();

		if (readPlayer != -1 && isPlaying[readPlayer] == true)
			currPlayer = readPlayer;
	}
}

void displayMainMenu(int currPlayer, int* accountBalance)
{
	eraseDisplay();
	if (currPlayer != 0){
		displayString(2, "Player %d, Balance: %d", currPlayer, accountBalance[currPlayer]);
		displayString(4, "1: Withdraw");
		displayString(5, "2: Deposit");
		displayString(6, "3: Transfer");
		displayString(7, "4: Declare Bankrupcty");
		displayString(8, "5: Cancel Transaction");
	}
	else{
		displayString(2, "MONOPOLY MAN, Balance: %d", accountBalance[currPlayer]);
		displayString(4, "1: Transfer");
		displayString(5, "2: Cancel Trancation");
	}
}

void declareBankruptcy(int currPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
{
	eraseDisplay();

	displayString(2,"Declare Bankrupt?");
	displayString(4, "1: Yes");
	displayString(5, "2: No");

	//wait for user to make decision
	while(!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp))
		{
			while(getButtonPress(buttonAny)){}

			displayText_Wait("BETTER LUCK NEXT TIME");
			/*
			run deposit function to get cash
			displayString(2, "Deposit any remaining cash at hand");
			wait1Msec(1500);
			depositCash( . . . )
			*/

			isPlaying[currPlayer] = false;
			resetPlayerBalance(currPlayer,accountBalance); // this has been changed
			numPlayers--;
			continueTransaction = false;
		}
		else
			while (getButtonPress(buttonAny)){}
	}

void promptContinue(bool& continueTransaction, bool assumeContinue)
{
	eraseDisplay();
	if (assumeContinue)
		displayString(2,"CONTINUE TRANSACTION?");
	else
		displayString(2,"CONFIRM CANCEL");
	displayString(4, "1: Yes");
	displayString(5, "2: No");

	while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)){}

	if (getButtonPress(buttonUp) && assumeContinue || getButtonPress(buttonLeft) && !assumeContinue)
		while (getButtonPress(buttonAny)){}

	else
	{
		while (getButtonPress(buttonAny)){}
		continueTransaction = false;
	}
}


void doTransaction(int currPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
{
	int transactionBills[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};

	//for regular players
	if(currPlayer != 0){
		while(!getButtonPress(buttonAny)){}

		if(getButtonPress(buttonUp))
		{
			while (getButtonPress(buttonAny)){}
			displayText_Wait("WITHDRAW");
			int playerBalance = accountBalance[currPlayer];
			withdraw(playerBalance, transactionBills);
			accountBalance[currPlayer] = playerBalance;
		}

		else if(getButtonPress(buttonLeft))
		{
			while (getButtonPress(buttonAny)){}
			displayText_Wait("DEPOSIT");
			deposit(currPlayer, accountBalance, false);
		}

		else if(getButtonPress(buttonRight))
		{
			while (getButtonPress(buttonAny)){}
			displayText_Wait("TRANSFER");
			transfer(currPlayer, numPlayers, accountBalance, isPlaying);
			//transfer(parameters);
		}

		else if(getButtonPress(buttonDown))
		{
			while (getButtonPress(buttonDown)){}
			displayText_Wait("BANKRUPTCY");
			declareBankruptcy(currPlayer, numPlayers, isPlaying, accountBalance, continueTransaction);
		}

		else
		{
			while (getButtonPress(buttonEnter)){}
			promptContinue(continueTransaction, false);
		}
	}

	//for the monopoly man
	else{
		while(!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)){}

		if(getButtonPress(buttonUp))
		{
			while (getButtonPress(buttonAny)){}
			displayText_Wait("TRANSFER");
			transfer(currPlayer, numPlayers, accountBalance, isPlaying);
			//transfer(parameters);
		}

		else{
			while (getButtonPress(buttonLeft)){}

		}
	}
}

void declareWinner(bool* isPlaying)
{
	eraseDisplay();

	for (int index = 0; index < 4; index++)
		if (isPlaying[index] == 1)
			displayString(2, "PLAYER %d WON MONOPOLY!", index);
	wait1Msec(2000);
	displayText_Wait("DEPOSIT REMAINING CASH");
	//####deposit (parameters)
}

void deposit(int currPlayer, int* accountBalance, bool playerIsDone)
{
	//tracks whether deposit transaction has been cancelled
	bool isCancelled = false;
	int depositAmount = 0;
	int transactionBills[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};

	//prompt user for bills
	conveyorReturn();
	sendTray(USER_PICKUP);
	displayString(2, "DEPOSIT");
	displayString(4, "PLACE BILLS ON TRAY");
	displayString(5, "PRESS ENTER TO CONTINUE");
	displayString(6, "PRESS DOWN TO CANCEL");

	//wait for user
	while (!getButtonPress(buttonEnter) && !getButtonPress(buttonDown)){}
	//if wants to continue
	if (getButtonPress(buttonEnter))
	{
		processDeposit(transactionBills);
	}
	else
	{
		isCancelled = true;
	}

	if (!isCancelled)
	{
		depositAmount = calcTransactionAmount(transactionBills);
		if(!playerIsDone)
			accountBalance[currPlayer] += depositAmount;
		else
			accountBalance[0] += depositAmount;

		displayText_Wait("DEPOSIT COMPLETE");
	}
}

void processDeposit(int* transactionBills)
{
	//set billIndex to int value that will proceed through loop
	int billIndex = 8;

	//"zero" everything before starting processing
	zeroGantry();
	conveyorReturn();
	sendTray(COLOUR_SENSE_LOCATION);

	//while the colour sensor doesn't sense black when scanning for colours
	//scan bill, move bill to tray, repeat
	while (billIndex != -1)
	{
		billIndex = senseBill(COLOUR_BILL);
		eraseDisplay();
		displayString(2, "BIll sensed %d", billIndex);
		wait1Msec(1500);
		if (billIndex != -1)
		{
			transactionBills[billIndex]++;
			//proceed with picking up bill and moving
			conveyorReturn();
			//account for intake/outtake tray taking up a spot
			masterTransverse(OUTPUT_TRAY_LOCATION, billIndex);
			sendTray(COLOUR_SENSE_LOCATION);
		}
	}
	conveyorReturn();
}

void withdraw(int& playerBalance, int* transactionBills)
{
	//tracks whether withdraw transaction has been cancelled
	bool isCancelled = false;
	//process withdraw transaction
	int withdraw = receiveWithdrawBills(playerBalance, transactionBills, isCancelled);
	//if user confirms transaction
	if (!isCancelled)
	{
	  displayText_Wait("WITHDRAWAL COMPLETE");
		//update account
		playerBalance -= withdraw;
		//move bills out of bins into main tray
		moveBillsOut(transactionBills);
		//move bills out for user
		sendTray(USER_PICKUP);
	}
}

int receiveWithdrawBills(int playerBalance, int* transactionBills, bool& isCancelled)
{
	//lower options screen
	getLowerOptions(playerBalance, transactionBills, isCancelled);

	int totalWithdraw = calcTransactionAmount(transactionBills);
	return totalWithdraw;
}

void moveBillsOut(int* transactionBills)
{
	for (int bill = 0; bill < NUM_BINS; bill++)
	{
		if (transactionBills[bill] > 0)
		{
			for (int numBills = 0; numBills < transactionBills[bill]; numBills++)
			{
				masterTransverse(bill, OUTPUT_TRAY_LOCATION);
			}
		}
	}
}

void cancelTransaction(int* transactionBills, bool& isCancelled)
{
	for (int bill = 0; bill < NUM_BINS; bill++)
	{
		transactionBills[bill] = 0;
	}
	displayText_Wait("TRANSACTION CANCELLED");
	//set condition that exits displayLowerOptions, modified version for displayHigherOptions
	isCancelled = true;
}

void clearChosenBills(int* transactionBills)
{
	for (int bill = 0; bill < NUM_BINS; bill++)
	{
				transactionBills[bill] = 0;
	}
	displayText_Wait("TRANSACTION CLEARED");
}

void getLowerOptions(int playerBalance, int* transactionBills, bool& isCancelled)
{
		bool isDone = false;
		while (!isDone)
		{
			displayLowerOptions(transactionBills, playerBalance);
			//wait for button press
			while (!getButtonPress(buttonAny)){}

			//check  button options
			if (getButtonPress(buttonUp) &&  isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 1)) {
				transactionBills[0]++;
			}
			else if (getButtonPress(buttonLeft))
			{
				//check if clear
				if (isClearOrCancel(transactionBills))
				{
					clearChosenBills(transactionBills);
				}
				//update bills otherwise
				else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 5))
				{
					transactionBills[1]++;
				}
			}
			else if (getButtonPress(buttonRight))
			{

				//check if cancel
				if (isClearOrCancel(transactionBills))
				{
					cancelTransaction(transactionBills, isCancelled);
					isDone = true;
				}
				//update bills otherwise
				else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 10))
				{
					transactionBills[2]++;
				}
			}
			else if (getButtonPress(buttonEnter) && isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 20))
			{
				transactionBills[4]++;
			}
			//if user wants to go to higher options screen
			else if (getButtonPress(buttonDown))
			{
				/*getHigherOptions is similar to getLowerOptions
				but returns true if user clicks "go back button" and
				false if user clicks "confirm transaction" button*/
				isDone = getHigherOptions(playerBalance, transactionBills, isCancelled);
			}
			//wait for user to release button
			while(getButtonPress(buttonAny)){}
		}
}

bool getHigherOptions(int playerBalance, int* transactionBills, bool& isCancelled)
{
	displayHigherOptions(transactionBills, playerBalance);
	while (getButtonPress(buttonAny)){}

	bool isHigherOptionsDone = false;
	bool isTransactionDone = false;
	while (!isHigherOptionsDone && !isTransactionDone)
		{
			displayHigherOptions(transactionBills, playerBalance);
			while (!getButtonPress(buttonAny)){}

			//check button options
		  if (getButtonPress(buttonUp) &&  isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 50)) {
				transactionBills[5]++;
			}
			else if (getButtonPress(buttonLeft))
			{
				//check if clear
				if (isClearOrCancel(transactionBills))
				{
					clearChosenBills(transactionBills);
					displayHigherOptions(transactionBills, playerBalance);
				}
				//check if can add bill to withdrawal
				else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 100))
				{
					transactionBills[6]++;
				}
			}
			else if (getButtonPress(buttonRight))
			{
				//check if cancel
				if (isClearOrCancel(transactionBills))
				{
					cancelTransaction(transactionBills, isCancelled);
		      isTransactionDone = true;
				}
				//check if can add bill to withdrawal
				else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 500))
				{
					transactionBills[7]++;
				}
			}
			//this button returns back to original screen
			else if (getButtonPress(buttonDown))
			{
				isHigherOptionsDone = true;
			}
			//if withdrawal is done
			else if (getButtonPress(buttonEnter))
			{
				isTransactionDone = true;
			}
			//wait for user to stop pressing button
			while(getButtonPress(buttonAny)){}
		}
		return isTransactionDone;
}

void displayLowerOptions(int* transactionBills, int playerBalance)
{
	eraseDisplay();
	displayString(1, "WITHDRAWAL LOWER OPTIONS");
	displayString(3, "$1 bills");
	displayString(4, "$5 bills");
	displayString(5, "$10 bills");
	displayString(6, "$20 bills");
	displayString(7, "GO TO HIGHER OPTIONS");
	displayString(9, "TOTAL WITHDRAW: %d", calcTransactionAmount(transactionBills));
	displayString(10, "CASH REMAINING: %d", calcRemainingCash(calcTransactionAmount(transactionBills), playerBalance));
}

void displayHigherOptions(int* transactionBills, int playerBalance)
{
	eraseDisplay();
	displayString(1, "WITHDRAWAL HIGHER OPTIONS");
	displayString(3, "$50 bills");
	displayString(4, "$100 bills");
	displayString(5, "$500 bills");
	displayString(6, "CONFIRM TRANSACTION");
	displayString(7, "GO TO LOWER OPTIONS");
	displayString(9, "TOTAL WITHDRAW: %d", calcTransactionAmount(transactionBills));
	displayString(10, "CASH REMAINING: %d", calcRemainingCash(calcTransactionAmount(transactionBills), playerBalance));
}

int calcRemainingCash(int transactionAmont, int balance)
{
	return balance - transactionAmont;
}

bool isValidTransaction(int playerBalance, int totalTransaction, int bill)
{
	if (playerBalance < totalTransaction + bill)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool isClearOrCancel(int* transactionBills)
{
	time1[T1] = 0;
	while (getButtonPress(buttonLeft) || getButtonPress(buttonRight))
	{
		if (time1[T1] > 3000)
		{
			displayString(12, "RELEASE BUTTON TO CONTINUE");
		}
	}
	if (time1[T1] > 3000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int calcTransactionAmount(int* transactionBills)
{
	int totalTransaction = 0;
	int billValues[NUM_BINS] = {1, 5, 10, 0, 20, 50, 100, 500};
	for (int bill = 0; bill < NUM_BINS; bill ++)
		totalTransaction += transactionBills[bill] * billValues[bill];
	return totalTransaction;
}

void displayText_Wait(string* text)
{
	eraseDisplay();
	displayString(2, "%s", text);
	wait1Msec(DISPLAY_WAIT);
}

void displayText_NoWait(string* text)
{
	eraseDisplay();
	displayString(2, "%s", text);
}

//main transfer function
/*displays transfer options based on number of players in game
user is prompted to enter transferee based on button input
if user presses Enter button, transfer function is exited
if user presses valid transferee, user is prompted for transfer amount
...
*/
void transfer(int transferor, int playersInGame, int * playerBalance, bool * isPlaying)
{
	//tracks whether transfer transaction has been cancelled
	bool isCancelled = false;
	int transferee = 0;		//initialize variable
	int transferOption[MAX_PLAYERS];		//declare array for transfer options

	for (int index = 0; index < MAX_PLAYERS; index++)		//initialize array for transfer options
	{
		transferOption[index] = 0;
	}

	displayTransferOptions(transferor, transferOption, isPlaying);		//calls functions to display transfer options
	transferee = getTransferee(playersInGame, transferOption);		//calls functions to get valid user input for transferee

	//checks if Enter button is pressed
	if (transferee != -1)
	{
		//if Enter button is not pressed, calls function to transfer inputted amount to tranferee
		transferAmount(transferor, transferee, playerBalance, isCancelled);
	}
	if (!isCancelled)
	{
		displayText_Wait("TRANSFER COMPLETE");
	}

}

void buttonPressValid(int playersInGame)
/*after button press, checks if input is valid based on number of players in game
if number of players < 3, Right button and Down button are invalid
if number of players < 4, Down button is invalid
Up button and Left button are always valid since atleast two options will always be available*/
{
	//checking if number of players in game is less than 3 and Right button is pressed
	if(playersInGame < 3 && getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonAny)){}		//waits for button release
		waitButtonPress(playersInGame);		//calls function to wait for button press and then check validity again
	}
	//checking if number of players in game is less than 3 and Down button is pressed
	else if (playersInGame < 4 && getButtonPress(buttonDown))
	{
		while (getButtonPress(buttonAny)){}		//waits for button release
		waitButtonPress(playersInGame);		//calls function to wait for button press and then check validity again
	}
}

void waitButtonPress(int playersInGame)
{
	while(!getButtonPress(buttonAny)){}//waits for button press
	buttonPressValid(playersInGame);//calls function to check for validity
}

int getTransferee(int playersInGame, int * transferOption)
{
	int transferee = -1;		//initializes variable

	waitButtonPress(playersInGame);		//waits for valid button press
	eraseDisplay();

	if (getButtonPress(buttonUp))
		transferee = transferOption[0];		//Up button is designated to transfer option at index 0
	else if (getButtonPress(buttonLeft))
		transferee = transferOption[1];		//Left button is designated to transfer option at index 1
	else if (getButtonPress(buttonRight))
		transferee = transferOption[2];		//Right button is designated to transfer option at index 2
	else if (getButtonPress(buttonDown))
		transferee = transferOption[3];		//Down button is designated to transfer option at index 3
	while(getButtonPress(buttonAny)){}		//waits for button release

	return transferee;//returns option picked based on button input
}

//updates appropriate balances based on transfer amount
void transferAmount(int transferor, int transferee, int * accountBalance, bool & isTransferCancelled)
{
	displayText_Wait("PICK YOUR BILLS");
	int transferorBalance = accountBalance[transferor];
	int transferAmount = getTransferAmount(transferorBalance, isTransferCancelled);		//calls function to get transfer amount
	accountBalance[transferor] -= transferAmount;		//subtracts amount from transferer balance
	accountBalance[transferee] += transferAmount;		//adds amount to transfer recipient balance

}

int getTransferAmount(int playerBalance, bool & isTransferCancelled)
//gets user input for transfer amount
{
	int transferBills[NUM_BINS];		//declare array for bills

	for (int index = 0; index < NUM_BINS; index++)		//initialize array for transfer bills
	{
		transferBills[index] = 0;
	}

	getLowerOptions(playerBalance, transferBills, isTransferCancelled);		//calls function to display bill options, get user input, check validity of input
	int transferAmount = calcTransactionAmount(transferBills);		//calls function to calculate transfer amount

	return transferAmount;
}

void displayTransferOptions(int transferor, int * transferOption, bool * isPlaying)
{
	int index = 0, row = DISPLAYSTART;		//initialize variables

	for(int option = 0; option < MAX_PLAYERS; option++)
	{
		if(option != transferor && isPlaying[option])
		{
			if(option == 0)
			{
				displayString(row, "Monopoly");
				row ++;
				transferOption[index] = option;
				index ++;
			}
			else
			{
				displayString(row, "Player %d", option);
				row ++;
				transferOption[index] = option;
				index ++;
			}
		}
	}
}

void zeroGantry()
{
	motor[GANTRY_MOTOR] = -40;
	while(SensorValue(TOUCH_ENC_ZERO)!=1){}
	motor[GANTRY_MOTOR]=0;
}

void GantryTransverse(int position)
{
	zeroGantry();
	nMotorEncoder[motorA]=0;
	if (position==1)
	{
		motor[motorA]=40;
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
		while(nMotorEncoder[motorA]*PI*3/180.0<22.7){}
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==4)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<32.6){}//21.5+9.6+1.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==5)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<38.8){}//21.5+9.6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==6)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<44.7){}//21.5+9.6+6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
	else if (position==7)
	{
		motor[motorA]=40;
		while(nMotorEncoder[motorA]*PI*3/180.0<50.5){}//21.5+9.6+6+6+7.5
		motor[motorA]=0;
		wait10Msec(50);
	}
}

void pickUpBill()
//motora=suction
//VERT_ACTUATOR_MOTOR= end effector

{
	nMotorEncoder[VERT_ACTUATOR_MOTOR]=0;

	motor[VERT_ACTUATOR_MOTOR]=-30;
	while(nMotorEncoder[VERT_ACTUATOR_MOTOR]*PI*3.2/180.0>-14){}
	motor[VERT_ACTUATOR_MOTOR]=0;
	nMotorEncoder[END_EFFECTOR_MOTOR]=0;
	motor[END_EFFECTOR_MOTOR] = -40;
	while(nMotorEncoder[END_EFFECTOR_MOTOR]*PI/180.0>-1.8){}
	motor[END_EFFECTOR_MOTOR]=0;
	motor[VERT_ACTUATOR_MOTOR]=30;
	while(nMotorEncoder[VERT_ACTUATOR_MOTOR]*PI*3.2/180.0<0){}
	motor[VERT_ACTUATOR_MOTOR]=0;
	wait10Msec(50);
}

void dropBill(){
	nMotorEncoder[VERT_ACTUATOR_MOTOR]=0;
	motor[VERT_ACTUATOR_MOTOR]=-30;
	while(nMotorEncoder[VERT_ACTUATOR_MOTOR]*PI*3.2/180.0>-7){}
	motor[VERT_ACTUATOR_MOTOR]=0;
	motor[END_EFFECTOR_MOTOR]=40;
	while(nMotorEncoder[END_EFFECTOR_MOTOR]*PI/180.0<1.5){}
	motor[END_EFFECTOR_MOTOR]=0;
	wait10Msec(30);

	motor[VERT_ACTUATOR_MOTOR]=30;
	while(nMotorEncoder[VERT_ACTUATOR_MOTOR]*PI*3.2/180.0<0){}
	motor[VERT_ACTUATOR_MOTOR]=0;
	motor[END_EFFECTOR_MOTOR] = -20;
	while(nMotorEncoder[END_EFFECTOR_MOTOR]*PI/180.0>0){}
	motor[END_EFFECTOR_MOTOR]=0;
	wait10Msec(50);
}

void masterTransverse(int initial, int final)
{
	GantryTransverse(initial);
	pickUpBill();
	GantryTransverse(final);
	dropBill();
}

void sendTray(int trayLocation)
{
	if (trayLocation)
	{
		//moving tray to colour sensor
		motor[CONVEYER_MOTOR]=30;
		while(nMotorEncoder[CONVEYER_MOTOR]*PI*1.75/180.0<7){}
		motor[CONVEYER_MOTOR]=0;
		wait10Msec(5);
	}
	else
	{
		//moving tray to user
		motor[CONVEYER_MOTOR]=30;
		while(nMotorEncoder[CONVEYER_MOTOR]*PI*1.75/180.0<26){}
		motor[CONVEYER_MOTOR]=0;
		wait10Msec(5);
	}
}

//this function brings tray back into enclosure
void conveyorReturn()
{
	motor[CONVEYER_MOTOR]=-30;
	while(nMotorEncoder[CONVEYER_MOTOR]*PI*2.75/180.0>0){}
	motor[CONVEYER_MOTOR]=0;
	wait10Msec(100);
}

void resetPlayerBalance(int currPlayer, int *playerBalances)
{
		playerBalances[0] += playerBalances[currPlayer] - 1500;
		playerBalances[currPlayer] = 1500;
}

void endGame(int currPlayer, int *playerBalances)         // Request all bills be deposited into the tray, redistribute into slots, state any discrepancies in the count
{
	resetPlayerBalance(currPlayer, playerBalances);
	displayText_NoWait("Return maximum 20 bills to tray");
	sendTray(USER_PICKUP);
	displayText_NoWait("Press Enter when all money has be placed in the tray");
	while(!getButtonPress(buttonAny));
	while(getButtonPress(buttonEnter));
	//conveyorSend(COLOUR_SENSE_LOCATION);
	bool keepCollecting =true;
	while(playerBalances[0]<MAX_CASH&&keepCollecting)
	{
		deposit(currPlayer, playerBalances, true);
	}
	displayText_NoWait("Thank you for playing");
	displayString(3,"%d dollars have not been returned to the game", MAX_CASH-playerBalances[0]);


}

task main()
{
	sensorConfig();

	int numPlayers = 0;
	bool isPlaying[MAX_PLAYERS] = {true, false, false, false, false};
	int accountBalance[MAX_PLAYERS] = {MAX_CASH,0,0,0,0};

	setupPlayers(numPlayers, accountBalance, isPlaying);

	while(numPlayers > 1)
	{
		displayText_NoWait("PLEASE SWIPE CARD");

		int currPlayer = -1;
		setCurrPlayer(currPlayer,isPlaying);

		bool continueTransaction = true;
		do
		{
			displayMainMenu(currPlayer, accountBalance);
			doTransaction(currPlayer, numPlayers, isPlaying, accountBalance, continueTransaction);
			if(continueTransaction)
				promptContinue(continueTransaction, true);
		} while (continueTransaction);
	}

	declareWinner(isPlaying);
}
