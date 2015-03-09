/*******************************************************************************************
Provides the Elm-Chan FatFs interface to hook into the fileSystem API (disk-independent)

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __FILE_FAT_H__
#define __FILE_FAT_H__

#include "ff.h"
#include "fileSystem.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
	DIR	dir;
	BOOL	isOpen;
} FAT_DIR;

typedef struct
{
	FATFS		fileSystem;
	FIL 		files[F_MAX_FILES_PER_DISK];
	FAT_DIR	dirs[F_MAX_FINDS_PER_DISK];
} FAT_DISK;



/**************************************************************************
 *                                  Prototypes
 **************************************************************************/

// Disk operations:
FDISK_RESULT FileFatMount(FDISK *pDisk);
FDISK_RESULT FileFatUnmount(FDISK *pDisk);
FDISK_RESULT FileFatGetStats(FDISK const *pDisk, FDISK_STATS *pStats);

// File Operations:
int  FileFatFOpen (void *pFs, char const *name, FOPEN_MODE mode);
int  FileFatFClose(int fd);
int  FileFatFRead (int fd, void *pDest, int len);
int  FileFatFWrite(int fd, const void *pSrc, int len);
long FileFatFSeek (int fd, long offset, FSEEK_ORIGIN origin);
long FileFatFTell (int fd);
int  FileFatFEof  (int fd);
int  FileFatFFlush(int fd);
int  FileFatRename(char const *pOld, char const *pNew);
int  FileFatRemove(char const *pName);
int  FileFatFStat (int fd, FSTAT *pDest);
int  FileFatStat  (char const *pName, FSTAT *pDest);

// Directory operations:
int    FileFatMkDir   (char const *pName);
void * FileFatOpenDir (void *pFs, char const *pName);
int    FileFatReadDir (FFIND *pDest, void *pDirArg);
int    FileFatCloseDir(void *pDirArg);

#endif
