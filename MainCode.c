const int START_BALANCE = 1500;
const int MAX_PLAYERS = 5;
const int MAX_CASH = 10000;
const int BILL_TYPES = 7;
const int NUMBILLS = 7;
const int DISPLAYSTART = 2;

const int DISPLAY_WAIT = 1000;

//Motors
const int GANTRY_MOTOR = 0;
const int VERT_ACTUATOR_MOTOR = 1;
const int END_EFFECTOR_MOTOR = 2;
const int CONVEYER_MOTOR = 3;

//Sensors
const int COLOUR_CARD = 0;	//port S1
const int COLOUR_BILL = 1;	//port S2
const int TOUCH_ENC_ZERO = 2;		//port S3

//function prototypes
//need to declare function stubs so functions are declared ahead of time
void withdraw(int& playerBalance, int* transactionBills, bool& doesContinue);
int receiveWithdrawBills(int& playerBalance, int* transactionBills, bool& doesContinue);
//void moveBillsOut(int* transactionBills);		josh's function
void cancelTransaction(int* transactionBills, bool& doesContinue);
void clearChosenBills(int* transactionBills);
void getLowerOptions(int playerBalance, int* transactionBills, bool& doesContinue);
void displayLowerOptions(int* transactionBills, int playerBalance);
void displayHigherOptions(int* transactionBills, int playerBalance);
//bool checkCancelOrClear(int* transactionBills, bool& doesContinue);
//void moveConveyer(int power);		josh's code
bool isValidTransaction(int playerBalance, int totalTransaction, int bill);
int calcTransactionAmount(int* transactionBills);
bool getHigherOptions(int playerBalance, int* transactionBills, bool& doesContinue);
bool isClearOrCancel(int* transactionBills);
void displayText_NoWait(string* text);
void displayText_Wait(string* text);
int calcRemainingCash(int transactionAmont, int balance);
void transfer(int transferor, int playersInGame, int* playerBalance, bool* isPlaying, bool& doesContinue);
void waitButtonPress(int playersInGame);
void buttonPressValid(int playersInGame);
int getTransferee(int playersInGame, int * transferOption);
void transferAmount(int transferor, int transferee, int * accountBalance, bool & isTransferCancelled);
int getTransferAmount(int playerBalance, bool & isTransferCancelled);
void displayTransferOptions(int transferor, int * transferOption, bool * isPlaying);

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
		while (getButtonPress(buttonAny)){}
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

int senseCard()
{
	long red, green, blue;
	getColorRGB(COLOUR_CARD, red, green, blue);
	red=blue=green=0;
	getColorRGB(COLOUR_CARD, red, green, blue);

	int colorCount[5] = {0,0,0,0,0};

	int numReadings = 0;

	while (numReadings < 10){
		//pink
			if(red<=120&&red>=2 && green<=35/100.0*red&&green>=5/100.0*red && blue<=60/100.0*red&&blue>=30/100.0*red)
				colorCount[1]++;
		//blue
			else if(red<=35/100.0*blue&&red>=5/100.0*blue && green<=100/100.0*blue&&green>=40/100.0*blue && blue<=100&&blue>=3)
				colorCount[2]++;
		//green
			else if(red<=50/100.0*green&&red>=20/100.0*green && green<=40*blue&&green>=3 && blue<=85/100.0*green&&blue>=40/100.0*green)
				colorCount[3]++;
		//yellow
			else if(red<=130&&red>=10 && green<=90/100.0*red&&green>=60/100.0*red && blue<=65/100.0*red&&blue>=30/100.0*red)
				colorCount[4]++;
	numReadings++;
	}

	int maxColorCount = 0;
	int maxColorIndex = 0;

	for (int index = 0; index < 3; index++)
		if (colorCount[index] > maxColorCount){
			maxColorCount = colorCount[index];
			maxColorIndex = index;
		}

	if (maxColorCount > 0)
		return maxColorIndex;
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
	displayString(2, "Player %d, Balance: %d", currPlayer, accountBalance[currPlayer]);
	displayString(4, "1: Withdraw");
	displayString(5, "2: Deposit");
	displayString(6, "3: Transfer");
	displayString(7, "4: Declare Bankrupcty");
	displayString(8, "5: Cancel Transaction");
}

void declareBankruptcy(int currentPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
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
	displayString(2,"Continue Transaction?");
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

void updateBankAccount(int* accountBalance, int currentPlayer, int playerBalance)
{
	accountBalance[currentPlayer] = playerBalance;
}

void doTransaction(int currentPlayer, int& numPlayers, bool* isPlaying, int* accountBalance, bool& continueTransaction)
{
	int transactionBills[7] = {0, 0, 0, 0, 0, 0, 0};

	while(!getButtonPress(buttonAny)){}

	if(getButtonPress(buttonUp))
	{
		while (getButtonPress(buttonAny)){}
		displayText_Wait("WITHDRAW");
		int playerBalance = accountBalance[currentPlayer];
		withdraw(playerBalance, transactionBills, continueTransaction);
		accountBalance[currentPlayer] = playerBalance;
	}

	else if(getButtonPress(buttonLeft))
	{
		while (getButtonPress(buttonAny)){}
		displayText_Wait("DEPOSIT");
		//deposit(parameters);
	}

	else if(getButtonPress(buttonRight))
	{
		while (getButtonPress(buttonAny)){}
		displayText_Wait("TRANSFER");
		transfer(currentPlayer, numPlayers, accountBalance, isPlaying, continueTransaction);
		//transfer(parameters);
	}

	else if(getButtonPress(buttonDown))
	{
		while (getButtonPress(buttonDown)){}
		displayText_Wait("BANKRUPTCY");
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
			displayString(2, "PLAYER %d WON MONOPOLY!", index);
	wait1Msec(2000);
	displayText_Wait("DEPOSIT REMAINING CASH");
	//####deposit (parameters)
}


void withdraw(int& playerBalance, int* transactionBills, bool& doesContinue)
{
	doesContinue = false;
	//process withdraw transaction
	int withdraw = receiveWithdrawBills(playerBalance, transactionBills, doesContinue);
	//if user confirms transaction
	if (!doesContinue)
	{
	  displayText_Wait("WITHDRAWAL COMPLETE");
		//update account
		playerBalance -= withdraw;
		//move bills out for user
		//###moveConveyer(50);		//POSITIVE_CONVEYER = 50
		//###moveBillsOut(transactionBills);
	}
}

int receiveWithdrawBills(int playerBalance, int* transactionBills, bool& doesContinue)
{
	//lower options screen
	getLowerOptions(playerBalance, transactionBills, doesContinue);

	int totalWithdraw = calcTransactionAmount(transactionBills);

	//ie moveBillsOut function right here
	for (int bill = 0; bill < BILL_TYPES; bill++)
	{
		if (transactionBills[bill] > 0)
		{
			for (int numBills = 0; numBills < transactionBills[bill]; numBills++)
			{
				/*moveArm(bill);
				pickupBill();
				centerArm();
				numBills++;*/
			}
		}
	}
	return totalWithdraw;
}
/*
void moveBillsOut(int* transactionBills)
{
	for (int bill = 0; bill < BILL_TYPES; bill++)
	{
		if (transactionBills[bill] > 0)
		{
			for (int numBills =0; numBills < transactionBills[bill]; numBills++)
			{
				moveArm(bill);
				pickupBill();
				centerArm();
				numBills++;
			}
		}
	}
}*/

void cancelTransaction(int* transactionBills, bool& doesContinue)
{
	for (int bill = 0; bill < BILL_TYPES; bill++)
	{
		transactionBills[bill] = 0;
	}
	displayText_Wait("TRANSACTION CANCELLED");
	//set condition that exits displayLowerOptions, modified version for displayHigherOptions
	doesContinue = true;
}

void clearChosenBills(int* transactionBills)
{
	for (int bill = 0; bill < BILL_TYPES; bill++)
	{
		transactionBills[bill] = 0;
	}
	displayText_Wait("TRANSACTION CLEARED");
}

void getLowerOptions(int playerBalance, int* transactionBills, bool& doesContinue)
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
					cancelTransaction(transactionBills, doesContinue);
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
				transactionBills[3]++;
			}
			//if user wants to go to higher options screen
			else if (getButtonPress(buttonDown))
			{
				/*getHigherOptions is similar to getLowerOptions
				but returns true if user clicks "go back button" and
				false if user clicks "confirm transaction" button*/
				isDone = getHigherOptions(playerBalance, transactionBills, doesContinue);
			}
			//wait for user to release button
			while(getButtonPress(buttonAny)){}
		}
}

bool getHigherOptions(int playerBalance, int* transactionBills, bool& doesContinue)
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
				transactionBills[4]++;
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
					transactionBills[5]++;
				}
			}
			else if (getButtonPress(buttonRight))
			{
				//check if cancel
				if (isClearOrCancel(transactionBills))
				{
					cancelTransaction(transactionBills, doesContinue);
		      isTransactionDone = true;
				}
				//check if can add bill to withdrawal
				else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 500))
				{
					transactionBills[6]++;
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

/*
bool checkCancelOrClear(int* transactionBills, bool& doesContinue)
{
	bool isOptionChosen = false;
	//check if want to quit
	if (getButtonPress(buttonLeft) || getButtonPress(buttonRight) || getButtonPress(buttonUp) || getButtonPress(buttonDown))
	{
		//check if they quit  ie top and bot buttons
		while(getButtonPress(buttonUp) && !isOptionChosen)
		{
			if (getButtonPress(buttonDown))
			{
				cancelTransaction(transactionBills, doesContinue);
				isOptionChosen = true;
			}
			displayString(10, "up button stuck");
		}
		while(getButtonPress(buttonDown) && !isOptionChosen)
		{
			if (getButtonPress(buttonUp))
			{
				cancelTransaction(transactionBills, doesContinue);
				isOptionChosen = true;
			}
			displayString(10, "down button stuck");
		}

		//check if they want to clear withdraw amount
		while(getButtonPress(buttonRight) && !isOptionChosen)
		{
			if (getButtonPress(buttonLeft))
			{
				clearChosenBills(transactionBills);
				isOptionChosen = true;
			}
			displayString(10, "right button stuck");
		}
		while(getButtonPress(buttonLeft) && !isOptionChosen)
		{
			if (getButtonPress(buttonRight))
			{
				clearChosenBills(transactionBills);
				isOptionChosen = true;
			}
			displayString(10, "left button stuck");
		}
	}
	return isOptionChosen;
}
*/

bool isClearOrCancel(int* transactionBills)
{
	time1[T1] = 0;
	while (getButtonPress(buttonLeft) || getButtonPress(buttonRight))
	{
		if (time1[T1] > 3000)
		{
			displayString(10, "RELEASE BUTTON");
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
	int billValues[BILL_TYPES] = {1, 5, 10, 20, 50, 100, 500};
	for (int bill = 0; bill < 7; bill ++)
		totalTransaction += transactionBills[bill] * billValues[bill];
	return totalTransaction;
}

void displayText_Wait(string* text)
{
	eraseDisplay();
	displayString(2, "Do this %s", text);
	wait1Msec(DISPLAY_WAIT);
}

void displayText_NoWait(string* text)
{
	eraseDisplay();
	displayString(2, "Do this %s", text);
}

//main transfer function
/*displays transfer options based on number of players in game
user is prompted to enter transferee based on button input
if user presses Enter button, transfer function is exited
if user presses valid transferee, user is prompted for transfer amount
...
*/
void transfer(int transferor, int playersInGame, int * playerBalance, bool * isPlaying, bool& doesContinue)
{
	doesContinue = false;
	int transferee = 0;		//initialize variable
	int transferOption[MAX_PLAYERS];		//declare array for transfer options

	for (int index = 0; index < MAX_PLAYERS; index++)		//initialize array for transfer options
	{
		transferOption[index] = 0;
	}

	displayTransferOptions(transferor, transferOption, isPlaying);		//calls functions to display transfer options
	transferee = getTransferee(playersInGame, transferOption);		//calls functions to get valid user input for transferee

	if (transferee != -1)		//checks if Enter button is pressed
		transferAmount(transferor, transferee, playerBalance, doesContinue);		//if Enter button is not pressed, calls function to transfer inputted amount to tranferee

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
	int transferorBalance = accountBalance[transferor];
	int transferAmount = getTransferAmount(transferorBalance, isTransferCancelled);		//calls function to get transfer amount
	accountBalance[transferor] -= transferAmount;		//subtracts amount from transferer balance
	accountBalance[transferee] += transferAmount;		//adds amount to transfer recipient balance
}

int getTransferAmount(int playerBalance, bool & isTransferCancelled)
//gets user input for transfer amount
{
	int transferBills[NUMBILLS];		//declare array for bills

	for (int index = 0; index < NUMBILLS; index++)		//initialize array for transfer bills
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

task main()
{
	sensorConfig();

	int numPlayers = 0;
	bool isPlaying[MAX_PLAYERS] = {true, false, false, false, false};
	int accountBalance[MAX_PLAYERS] = {MAX_CASH, 0,0,0,0};

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
				promptContinue(continueTransaction);
		} while (continueTransaction);
	}

	declareWinner(isPlaying);
}
