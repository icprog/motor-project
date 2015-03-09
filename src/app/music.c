/*******************************************************************************************
Simple music player

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "music.h"
#include "timer.h"
#include "beeper.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   SONG song;
   u32 sixteenthNoteDurationMs;
   u32 pauseBetweenNotesMs;
   u32 noteIndex;
   TIMER_ID timer;
}MUSIC;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static MUSIC music;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void MusicNoteOn(void);
static void MusicNoteOff(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void MusicPlaySong(SONG const *pSong, u32 sixteenthNoteMs, u32 pauseBtwNotesMs)
{
   if (music.song.pNotes != pSong->pNotes)
   {
      MusicStop();
      memcpy(&music.song, pSong, sizeof(SONG));
      music.sixteenthNoteDurationMs = sixteenthNoteMs;
      music.pauseBetweenNotesMs = pauseBtwNotesMs;
      music.noteIndex = 0;
      MusicNoteOn();
   }
}

void MusicStop(void)
{
   TimerTimeXMsCancel(music.timer);
	Beeper1Off();
}



/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void MusicNoteOn(void)
{
   MUSIC_NOTE const *pNote = &music.song.pNotes[music.noteIndex];
   Beeper1On((u16)pNote->note);
   music.timer = TimerTimeXMs(pNote->duration * music.sixteenthNoteDurationMs, MusicNoteOff);
}

static void MusicNoteOff(void)
{
   if (++music.noteIndex >= music.song.numNotes)
   {
      Beeper1Off();
      music.song.pNotes = NULL;
   }
   else
   {
      if (music.pauseBetweenNotesMs == 0)
      {
         MusicNoteOn();
      }
      else
      {
         Beeper1Off();
         music.timer = TimerTimeXMs(music.pauseBetweenNotesMs, MusicNoteOn);
      }
   }
}

