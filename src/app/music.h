/*******************************************************************************************
Simple music player

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "includes.h"
#include "notes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define NOTES_TO_SONG(notes)     {.numNotes = NELEMENTS((notes)), .pNotes=notes}


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   NOTE_SIXTEENTH = 1,
   NOTE_EIGHTH = 2,
   NOTE_QUARTER = 4,
   NOTE_HALF = 8,
   NOTE_WHOLE = 16,
}NOTE_DURATION;

typedef struct
{
   NOTE note;
   NOTE_DURATION duration;
}MUSIC_NOTE;

typedef struct
{
   u32 numNotes;
   MUSIC_NOTE const *pNotes;
}SONG;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void MusicPlaySong(SONG const *pSong, u32 sixteenthNoteMs, u32 pauseBtwNotesMs);
void MusicStop(void);

#endif
