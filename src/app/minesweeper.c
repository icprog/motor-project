/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "minesweeper.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
char insruct[4][20] = 
{
  "instruct 1",
  "instruct 2",
  "instruct 3",
  "instruct 4"
};

char insructMark[4][20] = 
{
  "instruct 1",
  "instruct 2",
  "instruct 3",
  "instruct 4"
};
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct node
{
  bool uncovered;
  bool isMine;
  int nearMines;
}NODE;
/**************************************************************************
 *                                  Variables
 **************************************************************************/
bool firstPlay = true;

u16 entropy = 333;

NODE field[5][20];

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void tutorial(void);
void fieldInit(NODE field, u16 entropy);

/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void MinesweeperInit(void)
{
  /*
  if (firstPlay)
  {
    tutorial();
    firstPlay = false;
  } */
  
  fieldInit(field, entropy);
}

void MinesweeperUpdate(void)
{
  
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
void fieldInit(NODE field, u16 entropy)
{
  u8 x;
  u8 y;
  
  bool successful = false;
  
  for(u8 i=0;i<10;i++)
  {
    successful = false;
    while (!successful)
    {
      x = (entropy^i)%20;
      y = (entropy^i)%5;
      if (field[y][x].
    }
  }
}


