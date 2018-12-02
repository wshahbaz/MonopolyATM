//Monopoly MAIN CODE BASE

//CONSTANTS
//cash balance
const int START_BALANCE = 1500;
const int MAX_PLAYERS = 5;
const int MAX_CASH = 13720;
const int NUM_BINS = 8;

//user interface
const int DISPLAYSTART = 4;
const int DISPLAY_WAIT = 1000;
const int NUM_TRANSFER_OPTIONS = 4;

//Bill TRAY indicies
enum BILLTRAYS {
    BILL_1, BILL_5, BILL_10, OUTPUT_TRAY_LOCATION, BILL_20,
    BILL_50, BILL_100, BILL_500
};

//Intake/outtake tray locations
const int USER_PICKUP = 0;
const int COLOUR_SENSE_LOCATION = 1;

//Motor ports
enum MOTOR_PORTS {
    GANTRY_MOTOR, VERT_ACTUATOR_MOTOR, END_EFFECTOR_MOTOR, CONVEYER_MOTOR
};
//Motor powers
enum MOTOR_POWERS {
    GANTRY_POWER = 40,
    VERT_ACTUATOR_POWER = 40,
    END_EFFECTOR_POWER_LOW = 20,
    END_EFFECTOR_POWER_HIGH = 40,
    CONVEYER_POWER = 30,
};
/*
const int GANTRY_MOTOR = 0;
const int VERT_ACTUATOR_MOTOR = 1;
const int END_EFFECTOR_MOTOR = 2;
const int CONVEYER_MOTOR = 3;*/

//Sensor ports
const int COLOUR_CARD = 0;
const int COLOUR_BILL = 1;
const int TOUCH_ENC_ZERO = 2;

//function prototypes

//FUNCTION PROTOTYPES
//*******************
//GENERAL USER INTERFACE FUNCTION PROTOTYPES
void displayText_NoWait(string *text);
void displayText_Wait(string *text);

void promptContinue(bool &continueTransaction);
void promptCancel(bool &continueTransaction);

//WITHDRAWAL FUNCTION(S)PROTOTYPES
void withdraw(int currentPlayer, int *accountBalance);
int receiveWithdrawBills(int &playerBalance, int *transactionBills, bool &isCancelled);
void getLowerOptions(int playerBalance, int *transactionBills, bool &isCancelled);
bool getHigherOptions(int playerBalance, int *transactionBills, bool &isCancelled);
bool isClearOrCancel();

void clearChosenBills(int *transactionBills);
void cancelTransaction(int *transactionBills, bool &isCancelled);
bool isValidTransaction(int playerBalance, int totalTransaction, int bill);
int calcTransactionAmount(int *transactionBills);
int calcRemainingCash(int transactionAmount, int balance);
void completeWithdrawal(int currentPlayer, int *accountBalance, int withdraw, int *transactionBills);

void displayLowerOptions(int *transactionBills, int playerBalance);
void displayHigherOptions(int *transactionBills, int playerBalance);

//DEPOSIT FUNCTION PROTOTYPES
void deposit(int currPlayer, int *accountBalance, bool isPlayerDone);
void processDeposit(int *transactionBills);

//TRANSFER FUNCTION PROTOTYPES
void transfer(int transferor, int playersInGame, int *playerBalance, bool *isPlaying);
int getTransferee(int playersInGame, int *transferOption);

void transferAmount(int transferor, int transferee, int *accountBalance, bool &isTransferCancelled);
int getTransferAmount(int playerBalance, bool &isTransferCancelled);

void waitButtonPress(int playersInGame);
void buttonPressValid(int playersInGame);

void displayTransferOptions(int transferor, int *transferOption, bool *isPlaying);

//PRE-TRANSACTION FUNCTION PROTOYPES
void setCurrPlayer(int &currPlayer, bool *isPlaying);
void displayMainMenu(int currPlayer, int *accountBalance);
void doTransaction(int currPlayer, int &numPlayers, bool *isPlaying,
	int *accountBalance, bool &continueTransaction);

//END PROGRAM FUNCTION PROTYPES
void resetPlayerBalance(int currPlayer, int *playerBalances);
int declareWinner(bool *isPlaying);
void declareBankruptcy (int currPlayer, int &numPlayers, bool *isPlaying,
	int *accountBalance, bool &continueTransaction);

//MECHANICAL FUNCTIONS (ALL FUNCTIONS)
//BILL MOVEMENT
void moveBillsOut(int *transactionBills);

void masterTransverse(int initial, int final);
void pickUpBill();
void GantryTransverse(int position);
void dropBill();
void zeroGantry();

void sendTray(int trayLocation);
void conveyorReturn();

void moveSelectMotor(int motorPort, int power, float encoderDistMult, float encoderDistLimit, int waitTime, int direction);

//COLOUR SENSING
int senseBill();
int senseCard();

//configures all EV3 sensors
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

//function prompts user for total number of players, sets up each player's account
void setupPlayers(int &numPlayers, int *accountBalance, bool *isPlaying)
{
		//graphical interface
    eraseDisplay();
    displayString(2, "CHOOSE NUMBER OF PLAYER");
    displayString(5, "a) 2 PLAYERS");
    displayString(6, "b) 3 PLAYERS");
    displayString(7, "c) 4 PLAYERS");

    //wait for button press
    while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft) && !getButtonPress(buttonRight)) {}

    //receive button press, update number of players depending on button press (2, 3, 4 players)
    if (getButtonPress(buttonUp))
    {
        while (getButtonPress(buttonAny)) {}
        numPlayers = 2;
        displayText_Wait("TWO PLAYER OPTION SELECTED");
    }
    else if (getButtonPress(buttonLeft))
    {
        while (getButtonPress(buttonAny)) {}
        numPlayers = 3;
        displayText_Wait("THREE PLAYER OPTION SELECTED");
    }
    else
    {
        while (getButtonPress(buttonRight)) {}
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

//function senses colour of player card
//returns int corresponding to player number
int senseCard()
{
		enum CARD_COLOURS {
        BROWN, PINK, BLUE, RED, GREEN
    };
		//initialization variables
    long red = 0, green = 0, blue = 0;
    int numReadings = 0;
    int colorCount[MAX_PLAYERS] = {0, 0, 0, 0, 0};

    //variables to hold most common colour readings
    int maxColorCount = 0;
    int maxColorIndex = 0;

    //initial reading - discard
    getColorRGB(COLOUR_CARD, red, green, blue);

    //iterate through multiple reads and record instance of each bill read
    while (numReadings < 10)
    {
        //get colour reading
        getColorRGB(COLOUR_CARD, red, green, blue);
        //pink
        if (red <= 140 && red >= 20 && green <= 150 / 100.0 * red && green >= 36 / 100.0 * red &&
            blue <= 70 / 100.0 * red && blue >= 20 / 100.0 * red)
            colorCount[PINK]++;
        //blue
        else if (red <= 60 / 100.0 * blue && red >= 38 / 100.0 * blue && green <= 70 / 100.0 * blue &&
                 green >= 50 / 100.0 * blue && blue <= 30 && blue >= 15)
            colorCount[BLUE]++;
        //red
        else if (red <= 80 && red >= 30 && green <= 20 / 100.0 * red && green >= 3 / 100.0 * red &&
                 blue <= 20 / 100.0 * red && blue >= 5 / 100.0 * red)
            colorCount[RED]++;
        //green
        else if (red <= 70 / 100.0 * green && red >= 50 / 100.0 * green && green <= 80 && green >= 20 &&
                 blue <= 70 / 100.0 * green && blue >= 50 / 100.0 * green)
            colorCount[GREEN]++;
        //brown
        else if (red <= 15 && red >= 2 && green <= 70 / 100.0 * red && green >= 40 / 100.0 * red &&
                 blue <= 70 / 100.0 * red && blue >= 40 / 100.0 * red)
            colorCount[BROWN]++;

        numReadings++;
    }

    //get most common colour index
    for (int index = 0; index < 5; index++)
        if (colorCount[index] > maxColorCount)
        {
            maxColorCount = colorCount[index];
            maxColorIndex = index;
        }

    //return colour index or -1 if nothing is read (ie sees black)
    if (maxColorCount > 0)
        return maxColorIndex;

    return -1;
}

//reads bill colour and returns int based on colour
int senseBill()
{
		//initialize variables
    long red = 0, green = 0, blue = 0;
    getColorRGB(COLOUR_BILL, red, green, blue);

    //array to keep track of numer of instance of bill reads
    int colorCount[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};

    //max colour counts
    int maxColorCount = 0;
    int maxColorIndex = 0;

    int numReadings = 0;

    //iterate bill reads, record instance of each bill
    while (numReadings < 30) {
        getColorRGB(COLOUR_BILL, red, green, blue);
        //pink - 1s
        if (red <= 120 && red >= 15 && green <= 50 / 100.0 * red && green >= 20 / 100.0 * red &&
            blue <= 60 / 100.0 * red && blue >= 20 / 100.0 * red)
            colorCount[BILL_1]++;
        //purple - 5s
        else if (red <= 35 && red >= 9 && green <= 100 / 100.0 * red && green >= 50 / 100.0 * red &&
                 blue <= 140 / 100.0 * red && blue >= 65 / 100.0 * red)
            colorCount[BILL_5]++;
        //brown - 10s
        else if (red <= 20 && red >= 4 && green <= 100 / 100.0 * red && green >= 50 / 100.0 * red && blue <= 10 &&
                 blue >= 3)
            colorCount[BILL_10]++;
        //blue - 20s
        else if (red <= 45 / 100.0 * green && red >= 5 / 100.0 * green && green <= 100 && green >= 12 &&
                 blue <= 150 / 100.0 * green && blue >= 60 / 100.0 * green)
            colorCount[BILL_20]++;
        //orange - 50s
        else if (red <= 120 && red >= 7 && green <= 30 / 100.0 * red && green >= 5 / 100.0 * red && blue <= 10 &&
                 blue >= 3)
            colorCount[BILL_50]++;
        //yellow - 100s
        else if (red <= 130 && red >= 20 && green <= 100 / 100.0 * red && green >= 60 / 100.0 * red &&
                 blue <= 65 / 100.0 * red && blue >= 25 / 100.0 * red)
            colorCount[BILL_100]++;
        //green - 500
        else if (red <= 50 / 100.0 * green && red >= 20 / 100.0 * green && green <= 60 && green >= 6 && blue <= 15 &&
                 blue >= 3)
            colorCount[BILL_500]++;
        numReadings++;
    }

    //calculate mode of colour reads
    for (int index = 0; index < 8; index++)
        if (colorCount[index] > maxColorCount) {
            maxColorCount = colorCount[index];
            maxColorIndex = index;
        }

    if (maxColorCount > 0)
        return maxColorIndex;
    //if it sees black
    return -1;
}

//calls senseCard function and associates player number (index) based on reading
void setCurrPlayer(int &currPlayer, bool *isPlaying)
{
		//set current reads to -1 to indicate no player
    int readPlayer = -1;
    currPlayer = -1;
    //keep reading until seeing valid player
    while (currPlayer == -1)
    {
    		//read card
        readPlayer = senseCard();

        //checking for validity
        if (readPlayer != -1 && isPlaying[readPlayer])
            currPlayer = readPlayer;
    }
}

//displays main menu when player card is read
void displayMainMenu(int currPlayer, int *accountBalance)
{
    eraseDisplay();

    //displays options for regular players
    if (currPlayer != 0)
    {
        displayString(2, "MAIN MENU");
        displayString(4, "PLAYER %d, BALANCE: %d", currPlayer, accountBalance[currPlayer]);
        displayString(5, "a) WITHDRAW");
        displayString(6, "b) DEPOSIT");
        displayString(7, "c) TRANSFER");
        displayString(8, "d) DECLARE BANKUPTCY");
        displayString(9, "e) CANCEL TRANSACTION");
    }
    //displays options for Monopoly Man
    else
    {
        displayString(2, "MAIN MENU");
        displayString(4, "MONOPOLY MAN, Balance: %d", accountBalance[currPlayer]);
        displayString(5, "a) TRANSFER");
        displayString(6, "b) CANCEL TRANSFER");
    }
}

//resets the player balance to starting value for keeping track of total cash value in endgame
void resetPlayerBalance(int currPlayer, int *playerBalances)
{
		//re-balances monopoly man account and current player account
    playerBalances[0] += playerBalances[currPlayer] - START_BALANCE;
    playerBalances[currPlayer] = START_BALANCE;
}

//allows user to exit the game
void declareBankruptcy (int currPlayer, int &numPlayers, bool *isPlaying,
	int *accountBalance, bool &continueTransaction)
	{

		//graphical interface
    eraseDisplay();
    displayString(2, "Declare Bankrupt?");
    displayString(4, "1: Yes");
    displayString(5, "2: No");

    //wait for user to make decision
    while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)) {}

    if (getButtonPress(buttonUp))
    {
        while (getButtonPress(buttonAny)) {}

        displayText_Wait("BETTER LUCK NEXT TIME");

        //reset player values and balance internal cash accounts
        resetPlayerBalance(currPlayer, accountBalance);

        //recieve any remaining cash from player and add to monopoly man account
        displayText_Wait("DEPOSIT REMAINING CASH");
        deposit(currPlayer, accountBalance, true);

        //final updates to bankrupt account
        isPlaying[currPlayer] = false;
        numPlayers--;
        continueTransaction = false;
    }
    else
        while (getButtonPress(buttonAny)) {}
}

//prompts user if they want to CONTINUE transaction and recieves input
void promptContinue(bool &continueTransaction)
{
    eraseDisplay();
    displayString(2, "CONTINUE TRANSACTION?");

    displayString(5, "a) Yes");
    displayString(6, "b) No");

    while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)) {}

    if (getButtonPress(buttonUp))
        while (getButtonPress(buttonAny)) {}
    else
    {
        while (getButtonPress(buttonAny)) {}
        continueTransaction = false;
    }
}

//prompts user if they want to CANCEL transaction and recieves input
void promptCancel(bool &continueTransaction)
{
	//graphical interface
	eraseDisplay();
	displayString(2, "CONFIRM CANCEL");
	displayString(5, "a) Yes");
  displayString(6, "b) No");

  //recieve button input, determine whether to cancel transaction or not
  while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)) {}

    if (getButtonPress(buttonUp))
  	{
      while (getButtonPress(buttonAny)) {}
      continueTransaction = false;
    }
    else
      while (getButtonPress(buttonAny)) {}
}

//recieves user input and calls respective transaction function
void doTransaction(int currPlayer, int &numPlayers, bool *isPlaying, int *accountBalance, bool &continueTransaction)
{
    //for regular players
    if (currPlayer != 0)
    {
        while (!getButtonPress(buttonAny)) {}

        if (getButtonPress(buttonUp))
        {
            while (getButtonPress(buttonAny)) {}
            displayText_Wait("WITHDRAW");
            withdraw(currPlayer, accountBalance);
        }
        else if (getButtonPress(buttonLeft))
        {
            while (getButtonPress(buttonAny)) {}
            displayText_Wait("DEPOSIT");
            deposit(currPlayer, accountBalance, false);
        }
        else if (getButtonPress(buttonRight))
        {
            while (getButtonPress(buttonAny)) {}
            displayText_Wait("TRANSFER");
            transfer(currPlayer, numPlayers, accountBalance, isPlaying);
        }
        else if (getButtonPress(buttonDown))
        {
            while (getButtonPress(buttonDown)) {}
            displayText_Wait("BANKRUPTCY");
            declareBankruptcy(currPlayer, numPlayers, isPlaying, accountBalance, continueTransaction);
        }
        else
        {
            while (getButtonPress(buttonEnter)) {}
            promptCancel(continueTransaction);
        }
    }
    //if the monopoly man is at the start screen, display his options
    else {
        while (!getButtonPress(buttonUp) && !getButtonPress(buttonLeft)) {}

        if (getButtonPress(buttonUp))
        {
            while (getButtonPress(buttonAny)) {}
            displayText_Wait("TRANSFER");
            transfer(currPlayer, numPlayers, accountBalance, isPlaying);
        }
        else
        {
            while (getButtonPress(buttonLeft)) {}
            promptCancel(continueTransaction);
        }
    }
}

//displays winner on screen and prompts for end game
int declareWinner(bool *isPlaying)
{
    eraseDisplay();
		int winnerIndex = 0;
		//find final player remaining and display that they won
    for (int index = 0; index < 4; index++)
    {
        if (isPlaying[index] == 1)
        {
            displayString(4, "PLAYER %d WON MONOPOLY!", index);
            winnerIndex = index;
        }
    }

    wait1Msec(2500);
    displayText_Wait("DEPOSIT REMAINING CASH");
    return winnerIndex;
}

//main deposit function - prompts user to enter bills and calls function to process bills
void deposit(int currPlayer, int *accountBalance, bool isPlayerDone)
{
    //tracks whether transaction has been cancelled
    bool isCancelled = false;
    int depositAmount = 0;
    int transactionBills[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};

    //prompt user for bills
    conveyorReturn();
    sendTray(USER_PICKUP);
    //user interface
    eraseDisplay();
    displayString(2, "DEPOSIT");
    displayString(4, "PLACE BILLS ON TRAY");
    displayString(5, "PRESS ENTER TO CONTINUE");
    displayString(6, "HOLD RIGHT TO CANCEL");

    //wait for user to press button
    bool noButtonPress = true;
    while (noButtonPress)
    {
	    while (!getButtonPress(buttonEnter) && !getButtonPress(buttonRight)) {}
	    //if user wants to proceed with transaction, then process bills
	    if (getButtonPress(buttonEnter))
	    {
	       processDeposit(transactionBills);
	       noButtonPress = false;
	    }
	    else
	    {
	    	if(isClearOrCancel())
	    	{
	       	isCancelled = true;
	       	noButtonPress = false;
	      }
	    }
	  }

    //update accoutns if transaction proceeds
    if (!isCancelled)
    {
        depositAmount = calcTransactionAmount(transactionBills);
        if (!isPlayerDone)
            accountBalance[currPlayer] += depositAmount;
        else
        		//considering final deposits to Monopoly Man (interal account balancing)
            accountBalance[0] += depositAmount;

        displayText_Wait("DEPOSIT COMPLETE");
    }
}

//intakes bills, reads them and adds them to list of bills for updating user account
void processDeposit(int *transactionBills)
{
    //set billIndex to int value that will proceed through loop first time through
    int billIndex = NUM_BINS;

    //"zero" everything before starting processing
    zeroGantry();
    conveyorReturn();
    sendTray(COLOUR_SENSE_LOCATION);

    //while the colour sensor doesn't sense black when scanning for colours
    //scan bill, move bill to tray, repeat
    while (billIndex != -1)
    {
        billIndex = senseBill();

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

//main withdraw function - prompts users to enter bills to withdraw, updates account
void withdraw(int currentPlayer, int *accountBalance)
{
    int transactionBills[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};
    //tracks whether withdraw transaction has been cancelled
    bool isCancelled = false;
    //process withdraw transaction
    int playerBalance = accountBalance[currentPlayer];
    int withdraw = receiveWithdrawBills(playerBalance, transactionBills, isCancelled);
    //if user confirms transaction, complete withdraw function
    if (!isCancelled)
        completeWithdrawal(currentPlayer, accountBalance, withdraw, transactionBills);
}

//function takes the bills out onto tray, updates account with withdraw
void completeWithdrawal(int currentPlayer, int *accountBalance, int withdraw, int *transactionBills)
{
    displayText_Wait("PROCESSING BILLS...");

    //update account with withdraw
    accountBalance[currentPlayer] -= withdraw;
    //move bills out of bins into main tray
    moveBillsOut(transactionBills);

    displayText_NoWait("WITHDRAWAL COMPELTE");

    //move bills out for user
    sendTray(USER_PICKUP);

    //prompt user to pick up cash and return tray
    displayText_NoWait("PRESS ENTER BUTTON WHEN DONE");
    while (!getButtonPress(buttonEnter)) {}
    conveyorReturn();
}

//recieves the bills the user wants to withdraw/transfer
int receiveWithdrawBills(int playerBalance, int *transactionBills, bool &isCancelled)
{
    //lower bill value options screen
    //this function will call other funcitons, updating transaction bills array
    getLowerOptions(playerBalance, transactionBills, isCancelled);

    //calculate total withdraw and return amount into main withdraw function
    int totalWithdraw = calcTransactionAmount(transactionBills);
    return totalWithdraw;
}

//moves the specified number of bills onto output tray for withdraw
void moveBillsOut(int *transactionBills)
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

//cancels withdraw/transfer transaction - clears transaction bills array
void cancelTransaction(int *transactionBills, bool &isCancelled)
{
    for (int bill = 0; bill < NUM_BINS; bill++)
        transactionBills[bill] = 0;

    displayText_Wait("TRANSACTION CANCELLED");
    //set condition that exits displayLowerOptions, modified version for displayHigherOptions
    isCancelled = true;
}

//clears the transaction bills array when user wants to clear withdraw/transfer amount
void clearChosenBills(int *transactionBills)
{
    for (int bill = 0; bill < NUM_BINS; bill++)
        transactionBills[bill] = 0;

    displayText_Wait("TRANSACTION CLEARED");
}

//prompts user for lower value bills and updates transaction bills array
void getLowerOptions(int playerBalance, int *transactionBills, bool &isCancelled)
{
    bool isDone = false;
    while (!isDone)
    {
        displayLowerOptions(transactionBills, playerBalance);
        //wait for button press
        while (!getButtonPress(buttonAny)) {}

        //check  button options
        if (getButtonPress(buttonUp) && isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 1))
            transactionBills[BILL_1]++;
        else if (getButtonPress(buttonLeft))
        {
            //check if clear
            if (isClearOrCancel())
                clearChosenBills(transactionBills);
            //update bills otherwise
            else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 5))
                transactionBills[BILL_5]++;
        }
        else if (getButtonPress(buttonRight))
        {
            //check if cancel
            if (isClearOrCancel())
            {
                cancelTransaction(transactionBills, isCancelled);
                isDone = true;
            }
            //update bills otherwise
            else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 10))
                transactionBills[BILL_10]++;
        }
        else if (getButtonPress(buttonDown) && isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 20))
            transactionBills[BILL_20]++;
            //if user wants to go to higher options screen
        else if (getButtonPress(buttonEnter))
            isDone = getHigherOptions(playerBalance, transactionBills, isCancelled);

        //wait for user to release button
        while (getButtonPress(buttonAny)) {}
    }
}

/*getHigherOptions is similar to getLowerOptions but returns true if user clicks "go back button" and
false if user clicks "confirm transaction" button. It allows users to pick higher amount bills*/
bool getHigherOptions(int playerBalance, int *transactionBills, bool &isCancelled)
{
		//display options for higher values
    displayHigherOptions(transactionBills, playerBalance);
    while (getButtonPress(buttonAny)) {}

    /*variables to check whether transaction and/or higher options is completed
    Note that if user decides to go back to lower options screen, isTranactionDone remains false
    but isHigherOptionsDone becomes true, thus breaking this higher options loop but not lower options loop*/
    bool isHigherOptionsDone = false;
    bool isTransactionDone = false;

    /*keep taking in input until the user clicks the "go back" button or
    the user clicks the "confirm" button. Returns true and false, respectively */
    while (!isHigherOptionsDone && !isTransactionDone)
    {
        displayHigherOptions(transactionBills, playerBalance);
        while (!getButtonPress(buttonAny)) {}

        //check button options
        if (getButtonPress(buttonUp) && isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 50))
            transactionBills[BILL_50]++;
        else if (getButtonPress(buttonLeft))
        {
            //check if clear
            if (isClearOrCancel())
            {
                clearChosenBills(transactionBills);
                displayHigherOptions(transactionBills, playerBalance);
            }
            //check if can add bill to withdrawal
            else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 100))
                transactionBills[BILL_100]++;
        }
        else if (getButtonPress(buttonRight))
        {
            //check if cancel
            if (isClearOrCancel())
            {
                cancelTransaction(transactionBills, isCancelled);
                isTransactionDone = true;
            }
            //check if can add bill to withdrawal
            else if (isValidTransaction(playerBalance, calcTransactionAmount(transactionBills), 500))
                transactionBills[BILL_500]++;
        }
        //this button returns back to original screen
        else if (getButtonPress(buttonEnter))
            isHigherOptionsDone = true;
        //if withdrawal is done, this button confirms withdraw (ends function)
        else if (getButtonPress(buttonDown))
            isTransactionDone = true;

        //wait for user to stop pressing button
        while (getButtonPress(buttonAny)) {}
    }
    return isTransactionDone;
}

//dusplays lower options for user to choose
void displayLowerOptions(int *transactionBills, int playerBalance) {
    eraseDisplay();
    displayString(2, "WITHDRAWAL LOWER OPTIONS");
    displayString(4, "a) $1 bills");
    displayString(5, "b) $5 bills");
    displayString(6, "c) $10 bills");
    displayString(7, "d) $20 bills");
    displayString(8, "e) GO TO HIGHER OPTIONS");
    displayString(10, "TOTAL WITHDRAW: %d", calcTransactionAmount(transactionBills));
    displayString(11, "CASH REMAINING: %d", calcRemainingCash(calcTransactionAmount(transactionBills), playerBalance));
}

//dusplays higher options for user to choose
void displayHigherOptions(int *transactionBills, int playerBalance) {
    eraseDisplay();
    displayString(2, "WITHDRAWAL HIGHER OPTIONS");
    displayString(4, "a) $50 BILLS");
    displayString(5, "b) $100 BILLS");
    displayString(6, "c) $500 BILLS");
    displayString(7, "d) CONFIRM TRANSACTION");
    displayString(8, "e) GO TO LOWER OPTIONS");
    displayString(10, "TOTAL WITHDRAW: %d", calcTransactionAmount(transactionBills));
    displayString(11, "CASH REMAINING: %d", calcRemainingCash(calcTransactionAmount(transactionBills), playerBalance));
}

//calculates user's remaining cash during withdrawal/transfer
int calcRemainingCash(int transactionAmount, int balance)
{
    return balance - transactionAmount;
}

//checks whether a user can withdraw a bill or not, returns boolean, (true means valid, false is invalid)
bool isValidTransaction(int playerBalance, int totalTransaction, int bill)
{
    if (playerBalance < totalTransaction + bill)
        return false;
    else
        return true;
}

//checks whether user wants to cancel or clear bill selections
bool isClearOrCancel()
{
    time1[T1] = 0;
    //check if one of possible buttons are clicked, and check if they're pressed long enough
    while (getButtonPress(buttonLeft) || getButtonPress(buttonRight))
    {
        if (time1[T1] > 2000)
            displayString(14, "RELEASE BUTTON TO CONTINUE");
    }
    if (time1[T1] > 2000)
        return true;
    else
        return false;
}

//calculates the amount of a transaction based on selection of bills
int calcTransactionAmount(int *transactionBills) {
    int totalTransaction = 0;
    int billValues[NUM_BINS] = {1, 5, 10, 0, 20, 50, 100, 500};

    for (int bill = 0; bill < NUM_BINS; bill++)
        totalTransaction += transactionBills[bill] * billValues[bill];

    return totalTransaction;
}

//displays text with a wait
void displayText_Wait(string *text)
{
    eraseDisplay();
    displayString(5, "%s", text);
    wait1Msec(DISPLAY_WAIT);
}

//displays text with no wait
void displayText_NoWait(string *text)
{
    eraseDisplay();
    displayString(5, "%s", text);
}

//main transfer function
/*displays transfer options based on number of players in game
user is prompted to enter transferee based on button input
if user presses Enter button, transfer function is exited
if user presses valid transferee, user is prompted for transfer amount */
void transfer(int transferor, int playersInGame, int *playerBalance, bool *isPlaying)
{
    //tracks whether transfer transaction has been cancelled
    bool isCancelled = false;
    //initialize variable
    int transferee = 0;
    //initialize array for transfer options
    int transferOption[MAX_PLAYERS] = {0, 0, 0, 0, 0};

		//calls functions to display transfer options
    displayTransferOptions(transferor, transferOption, isPlaying);
    //calls functions to get valid user input for transferee
    transferee = getTransferee(playersInGame, transferOption);

    //checks if Enter button is pressed
    if (transferee != -1)
    {
        //if Enter button is not pressed, calls function to transfer inputted amount to tranferee
        transferAmount(transferor, transferee, playerBalance, isCancelled);
        displayText_Wait("TRANSFER COMPLETE");
    }

    if (isCancelled || transferee == -1)
        displayText_Wait("TRANSFER CANCELLED");
}

/*checks if button press is valid in transfer function.
After button press, checks if input is valid based on number of players in game
if number of players < 3, Right button and Down button are invalid
if number of players < 4, Down button is invalid.
Up button and Left button are always valid since atleast two options will always be available*/
void buttonPressValid(int playersInGame)
{
    //checking if number of players in game is less than 3 and Right button is pressed
    if (playersInGame < 3 && getButtonPress(buttonRight))
    {
        while (getButtonPress(buttonAny)) {}
        //calls function to wait for button press and then check validity again
        waitButtonPress(playersInGame);
    }
    //checking if number of players in game is less than 3 and Down button is pressed
    else if (playersInGame < 4 && getButtonPress(buttonDown))
    {
        while (getButtonPress(buttonAny)) {}
        //calls function to wait for button press and then check validity again
        waitButtonPress(playersInGame);
    }
}

//waits for a button to be pressed in transfer, then calls function to check validity
void waitButtonPress(int playersInGame)
{
    while (!getButtonPress(buttonAny)) {}
    //calls function to check for validity
    buttonPressValid(playersInGame);
}

//gets the transferee of the transfer function
int getTransferee(int playersInGame, int *transferOption)
{
		//initializes variable
    int transferee = -1;
    //waits for valid button press
    waitButtonPress(playersInGame);

    eraseDisplay();

    //Up button is designated to transfer option at index 0
    if (getButtonPress(buttonUp))
        transferee = transferOption[0];
    //Left button is designated to transfer option at index 1
    else if (getButtonPress(buttonLeft))
        transferee = transferOption[1];
    //Right button is designated to transfer option at index 2
    else if (getButtonPress(buttonRight))
        transferee = transferOption[2];
    //Down button is designated to transfer option at index 3
    else if (getButtonPress(buttonDown))
        transferee = transferOption[3];

    while (getButtonPress(buttonAny)) {}

    //returns option picked based on button input
    return transferee;
}

//updates appropriate balances based on transfer amount
void transferAmount(int transferor, int transferee, int *accountBalance, bool &isTransferCancelled)
{
		//graphical interface
    eraseDisplay();
    displayString(4, "TRANSFER TO PLAYER %d", transferee);
    wait1Msec(1200);
    displayText_Wait("PICK YOUR BILLS");


    int transferorBalance = accountBalance[transferor];
    //calls function to get transfer amount
    int transferAmount = getTransferAmount(transferorBalance, isTransferCancelled);
    //updates player accoutns
    accountBalance[transferor] -= transferAmount;
    accountBalance[transferee] += transferAmount;
}

//finds the amoutn of a transfer; calls getLowerOptions (used in withdrawal function). returns value of transfer
int getTransferAmount(int playerBalance, bool &isTransferCancelled)
{
		//declare array for bills
    int transferBills[NUM_BINS] = {0, 0, 0, 0, 0, 0, 0, 0};

    //calls function to display bill options, get user input, check validity of input
    getLowerOptions(playerBalance, transferBills, isTransferCancelled);
    //calls function to calculate transfer amount
    int transferAmount = calcTransactionAmount(transferBills);

    return transferAmount;
}

//displays transferee options based on if they're playing
void displayTransferOptions(int transferor, int *transferOption, bool *isPlaying)
{
		const string OPTIONS[NUM_TRANSFER_OPTIONS] = { "a)", "b)", "c)", "d)" };

		//initialize variables
		int index = 0, row = DISPLAYSTART, optionNum = 0;

    eraseDisplay();
		displayString(2, "TRANSFER");

    //find players that are valid transferees and display them
    for (int option = 0; option < MAX_PLAYERS; option++)
    {
        if (option != transferor && isPlaying[option])
        {
            if (option == 0)
            {
                displayString(row, "%s Monopoly", OPTIONS[optionNum]);
                row++;
                transferOption[index] = option;
                index++;
                optionNum++;
            }
            else
            {
                displayString(row, "%s Player %d", OPTIONS[optionNum], option);
                row++;
                transferOption[index] = option;
                index++;
                optionNum++;
            }
        }
    }
}

//zeros gantry at end closer to brick
void zeroGantry()
{
    motor[GANTRY_MOTOR] = -GANTRY_POWER;
    while (SensorValue(TOUCH_ENC_ZERO) != 1) {}
    motor[GANTRY_MOTOR] = 0;
}

//based off colour the senseBill function returns, it will move gantry to corresponding bill location
void GantryTransverse(int position)
{
    zeroGantry();
    nMotorEncoder[GANTRY_MOTOR] = 0;

    //check position and move bill accordingly
    if (position == 1)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 6.25, 150, 0);
    else if (position == 2)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 12.5, 150, 0);
    else if (position == 3)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 22.7, 150, 0);
    else if (position == 4)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 32.6, 150, 0);
    else if (position == 5)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 38.8, 150, 0);
    else if (position == 6)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 44.7, 150, 0);
    else if (position == 7)
        moveSelectMotor(GANTRY_MOTOR, GANTRY_POWER, PI * 3 / 180.0, 50.5, 150, 0);
}

//picks up bills with combination of motor tasks
void pickUpBill()
{
    nMotorEncoder[VERT_ACTUATOR_MOTOR] = 0;

    //lower arm
    moveSelectMotor(VERT_ACTUATOR_MOTOR, -VERT_ACTUATOR_POWER, PI * 3.2 / 180.0, -14, 0, 1);
    //activate suction
    moveSelectMotor(END_EFFECTOR_MOTOR, -END_EFFECTOR_POWER_HIGH, PI / 180.0, -1.8, 0, 1);
		//raise arm
    moveSelectMotor(VERT_ACTUATOR_MOTOR, VERT_ACTUATOR_POWER, PI * 3.2 / 180.0, 0, 1000, 0);
}

//drops a bill with combination of motor tasks
void dropBill()
{
		//lower arm
    moveSelectMotor(VERT_ACTUATOR_MOTOR, -VERT_ACTUATOR_POWER, PI * 3.2 / 180.0, -5, 0, 1);
		//deactivate suction
    moveSelectMotor(END_EFFECTOR_MOTOR, END_EFFECTOR_POWER_HIGH, PI / 180.0, 1.5, 150, 0);
		//raise arm
    moveSelectMotor(VERT_ACTUATOR_MOTOR, VERT_ACTUATOR_POWER, PI * 3.2 / 180.0, 0, 0, 0);
		//reset end effector motor
    moveSelectMotor(END_EFFECTOR_MOTOR, -END_EFFECTOR_POWER_LOW, PI / 180.0, 0, 150, 1);
}

//main mechanical movement function. calls other mechanical functions to complete tasks for bill transffers
void masterTransverse(int initial, int final)
{
		//moves gantry to initial bill location
    GantryTransverse(initial);
    //picks up bill
    pickUpBill();
    //moves gantry to final bill location
    GantryTransverse(final);
    //drops bill
    dropBill();
}

//Send the bill tray to a given location from its intial position
void sendTray(int trayLocation)
{
    if (trayLocation)
        //moving tray to colour sensor
        moveSelectMotor(CONVEYER_MOTOR, CONVEYER_POWER, PI * 1.75 / 180.0, 7, 150, 0);
    else
        //moving tray to user
        moveSelectMotor(CONVEYER_MOTOR, CONVEYER_POWER, PI * 1.75 / 180.0, 26, 150, 0);
}

//brings tray back into enclosure
void conveyorReturn()
{
    moveSelectMotor(CONVEYER_MOTOR, -CONVEYER_POWER, PI * 2.75 / 180.0, 0, 200, 1);
}


/*this funciton moves a motor by a specified amount
Parameters:
	int motorPort: port of motor being moved
	int power: power of motor
	float encoderDistMult: factor that's multiplied by motor encoder to get distance (PI * ####)
	float encoderDistLimit: sets the distane that the motor will cover
	int waitTime: sets the time delay after a motor movement
	int direction: indicates whether a motor will be approaching a distance limit from the positive (ie greater than)
		or negative direction (ie less than)
	*/
void moveSelectMotor(int motorPort, int power, float encoderDistMult, float encoderDistLimit, int waitTime, int direction)
{
    motor[motorPort] = power;
    if (direction)
        while (nMotorEncoder[motorPort] * encoderDistMult > encoderDistLimit) {}
    else
        while (nMotorEncoder[motorPort] * encoderDistMult < encoderDistLimit) {}
    motor[motorPort] = 0;
    wait1Msec(waitTime);
}

// Request all bills be deposited into the tray,
//redistribute into slots, state any discrepancies in the count
void endGame(int currPlayer, int *playerBalances)
{
		//Balances virtual currency in player and monopoly man accounts
    resetPlayerBalance(currPlayer, playerBalances);
    //Reuturns all cash to monopoly man
    deposit(currPlayer, playerBalances, true);

    displayString(3, "MISSING MONEY: $%d", MAX_CASH - playerBalances[0]);
    displayString(4, "GAME OVER");
    wait1Msec(5000);
}

task main()
{
    sensorConfig();

    //initiates variables containingnecessary player information
    int numPlayers = 0;
    bool isPlaying[MAX_PLAYERS] = {true, false, false, false, false};
    int accountBalance[MAX_PLAYERS] = {MAX_CASH, 0, 0, 0, 0};

    setupPlayers(numPlayers, accountBalance, isPlaying);

    //game continues until one player remains
    while (numPlayers > 1)
    {
    	//graphical interface
    	eraseDisplay();
        displayString(4, "PLEASE INSERT CARD");
        displayString(7, "PRESS ENTER TO SCAN CARD");

        //wait for player to confirm for program to proceed with scanning colour
        while (!getButtonPress(buttonEnter)) {}
        while (getButtonPress(buttonEnter)) {}
        int currPlayer = -1;

        //gets colour reading from card sensor and sets player
        setCurrPlayer(currPlayer, isPlaying);
        //sets control variable for player to continue doing transactions
        bool continueTransaction = true;

        do
        {
        //player selects transactions
            displayMainMenu(currPlayer, accountBalance);
            doTransaction(currPlayer, numPlayers, isPlaying, accountBalance, false);

            //prompts user to continue transactions unless aborted by bankruptcy or cancel
            if (continueTransaction)
                promptContinue(continueTransaction);
        } while (continueTransaction);
    }
    //declares winner and ends game
    int winner = 0;
    winner = declareWinner(isPlaying);
    endGame(winner, accountBalance);
}
