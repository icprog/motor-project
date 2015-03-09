/*******************************************************************************************
Twinkle Twinkle Little Star

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "songTwinkleTwinkle.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
static const MUSIC_NOTE TwinkleTwinkleNotes[] =
{
   {NOTE_C7, NOTE_QUARTER},
   {NOTE_C7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_A7, NOTE_QUARTER},
   {NOTE_A7, NOTE_QUARTER},
   {NOTE_G7, NOTE_HALF},

   {NOTE_F7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_D7, NOTE_QUARTER},
   {NOTE_D7, NOTE_QUARTER},
   {NOTE_C7, NOTE_HALF},

   {NOTE_G7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_D7, NOTE_HALF},

   {NOTE_G7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_D7, NOTE_HALF},

   {NOTE_C7, NOTE_QUARTER},
   {NOTE_C7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_G7, NOTE_QUARTER},
   {NOTE_A7, NOTE_QUARTER},
   {NOTE_A7, NOTE_QUARTER},
   {NOTE_G7, NOTE_HALF},

   {NOTE_F7, NOTE_QUARTER},
   {NOTE_F7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_E7, NOTE_QUARTER},
   {NOTE_D7, NOTE_QUARTER},
   {NOTE_D7, NOTE_QUARTER},
   {NOTE_C7, NOTE_WHOLE},

};

const SONG TwinkleTwinkle = NOTES_TO_SONG(TwinkleTwinkleNotes);


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void SongTwinkleTwinkle(void)
{
    MusicPlaySong(&TwinkleTwinkle, 50, 30);
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

