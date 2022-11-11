#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace std;

//Cloud Integration Assembly Test


const int BOARD_SIZE = 8;

// sensor configuration

// function to get input

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
int movePeice(int x1, int y1, int x2, int y2, char[BOARD_SIZE][BOARD_SIZE])
{
  
}
	// moves to cell
	// pick up peice
	// place peice
	// update game board

void boardInitState(string board[BOARD_SIZE][BOARD_SIZE])
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
  
  return;
}

void printBoard(string board[BOARD_SIZE][BOARD_SIZE])
{
  for (int row = 7; row >= 0; row--)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {
      cout << board[row][col];
    }
    cout << endl;
  }
  return;
}

int main()
{
	// 2d array with the board location
	// bk, wk, k, q, b, r, p, n for knight
	string board[BOARD_SIZE][BOARD_SIZE] = { };
  
  boardInitState(board);
  printBoard(board);
	
	// pass by reference to functions

}