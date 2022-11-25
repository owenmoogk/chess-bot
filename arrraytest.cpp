#include <iostream>
#include <cstdlib>


using namespace std;

int main()
{
  string board[8][8] = {};

  for (int row = 0; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
  
        string value = "";
  
        // black or white
        if (row <= 1)
          value = "W";
        else if (row >= 6)
          value = "B";
  
        if (row == 1 || row == 6)
          value = value + "P";
  
        else if (row == 7 || row == 0)
          if (col == 0 || col == 7)
            value = value + "R";
          else if (col == 1 || col == 6)
            value = value + "N";
          else if (col == 2 || col == 5)
            value = value + "B";
          else if (col == 3)
            value = value + "Q";
          else if (col == 4)
            value = value + "K";
  
        board[row][col] = value;
      }
    }
    
    for (int i = 0; i < 8; i++)
  {
    for (int j = 0;j < 8; j++)
    {
      cout << board[i][j] << " ";
    }
    cout << endl;
  }
  
}