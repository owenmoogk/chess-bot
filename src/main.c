const int BOARD_SIZE = 8;
string board [BOARD_SIZE][BOARD_SIZE];

// sensor configuration

// function to get input
// returns current letter and number by pbr
void getInput(int &currentLetter, int &currentNumber)
{
	// ascii
	currentLetter = 65;
	currentNumber = 1;
	while (true)
	{
		while (!getButtonPress(buttonAny))
		{ }

		if (getButtonPress(buttonEnter)) return;

		if (getButtonPress(buttonLeft)) currentLetter -= 1;
		if (getButtonPress(buttonRight)) currentLetter += 1;
		if (getButtonPress(buttonUp)) currentNumber += 1;
		if (getButtonPress(buttonDown)) currentNumber -= 1;
		while (getButtonPress(buttonAny))
		{ }

		// wrapping around the board
		// ie, if you start at 1 and hit back, it will go to position 8
		if (currentLetter < 65) currentLetter = 72;
		if (currentLetter > 72) currentLetter = 65;
		if (currentNumber < 1) currentNumber = 8;
		if (currentNumber > 8) currentNumber = 1;

		// will have to test if this ascii casting works
		displayString(2, "Current Position: %c%i", currentLetter, currentNumber);
	}
}

// zeroing function

// lower claw and pick up peice

// move to cell
	// calculate distance to cell
	// move x axis
	// move z axis

// file input

// file output

// replay the game

// execute move fucntion
void movePeice(int x1, int y1, int x2, int y2)
{
	displayString(1, "owen");
	return;
}
// moves to cell
// pick up peice
// place peice
// update game board

void boardInitState()
{
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {

      string value = "";

      // black or white
      if (row <= 1)
      {
        value = "W";
      }
      else if (row >= 6)
      {
        value = "B";
      }

      if (row == 1 || row == 6)
      {
        value = value + "P";
      }
      else if (row == 7 || row == 0)
      {
        if (col == 0 || col == 7)
        {
          value = value + "R";
        }
        else if (col == 1 || col == 6)
        {
          value = value + "N";
        }
        else if (col == 2 || col == 5)
        {
          value = value + "B";
        }
        else if (col == 3)
        {
          value = value + "Q";
        }
        else if (col == 4)
        {
          value = value + "K";
        }
      }

      board[row][col] = value;
    }
  }

  //return;
}

task main()
{
	// 2d array with the board location
	// bk, wk, k, q, b, r, p, n for knight

	//string board[BOARD_SIZE][BOARD_SIZE];

  boardInitState();

	// pass by reference to functions

}
