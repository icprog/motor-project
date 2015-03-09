/*******************************************************************************************
This provides a single unified API for all different file systems
Note that the interface is mostly POSIX-compliant, but in order to avoid dynamic memory,
some routines pass in the file system workspace as an argument

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "fileDisk.h"

#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__


/*******************************************************************************************
 *                                  Constants
********************************************************************************************/
// NOTE: File Systems are defined like:
// #define FILE_SYSTEMS FS_NAME(name) FS_NAME(name2)
// (where "name" and "name2" are the names of the file system. Then this file will automatically
//  create a table to all of the required prototypes (i.e. "nameMount", "nameUnmount", etc.)
#ifndef FILE_SYSTEMS
#error "FILE_SYSTEMS must be defined as FS_NAME(name) FS_NAME(name2)"
#endif

// Compile options to limit code size:
//#define FS_READONLY	// Removes write, flush, rename, remove, mkdir
//#define FS_NO_FIND		// Removes openDir, readDir and closeDir


/*******************************************************************************************
 *                                  Types
********************************************************************************************/
typedef FDISK_RESULT (*PFN_FS_MOUNT)(FDISK *);
typedef FDISK_RESULT (*PFN_FS_UNMOUNT)(FDISK *);
typedef FDISK_RESULT (*PFN_FS_DISK_STATS)(FDISK const *, FDISK_STATS *);

typedef int  (*PFN_FOPEN)   (void *, char const *, FOPEN_MODE);	// file system data, name, openMode
typedef int	 (*PFN_FCLOSE)  (int);
typedef int  (*PFN_FREAD)   (int, void *, int);
typedef int  (*PFN_FWRITE)  (int, void const *, int);
typedef long (*PFN_FSEEK)   (int, long, FSEEK_ORIGIN);
typedef long (*PFN_FTELL)   (int);
typedef int  (*PFN_FEOF)    (int);
typedef int  (*PFN_FLUSH)   (int);
typedef int  (*PFN_RENAME)  (char const *, const char *);
typedef int  (*PFN_REMOVE)  (char const *);
typedef int  (*PFN_FSTAT)   (int, FSTAT *);
typedef int  (*PFN_STAT)    (char const *, FSTAT *);

typedef int  (*PFN_MK_DIR)(char const *);
typedef void * (*PFN_OPEN_DIR)(void *, char const *);	// file system data, dir
typedef int  (*PFN_READ_DIR)(FFIND *, void *);
typedef int  (*PFN_CLOSE_DIR)(void *);

// Put all function prototypes into one struct so that we can create an array of file systems
typedef struct
{
	PFN_FS_MOUNT		pfnMount;
	PFN_FS_UNMOUNT		pfnUnmount;
	PFN_FS_DISK_STATS	pfnDiskStats;

	PFN_FOPEN		pfnOpen;
	PFN_FCLOSE		pfnClose;
	PFN_FREAD		pfnRead;
	PFN_FSEEK		pfnSeek;
	PFN_FTELL		pfnTell;
	PFN_FEOF			pfnEof;
	PFN_FSTAT		pfnFStat;
	PFN_STAT			pfnStat;
#ifndef FS_READONLY
	PFN_FWRITE		pfnWrite;
	PFN_FLUSH		pfnFlush;
	PFN_RENAME		pfnRename;
	PFN_REMOVE		pfnRemove;
	PFN_MK_DIR		pfnMkDir;
#endif
#ifndef FS_NO_FIND
	PFN_OPEN_DIR	pfnOpenDir;
	PFN_READ_DIR	pfnReadDir;
	PFN_CLOSE_DIR	pfnCloseDir;
#endif
}FS_CALLS;

extern const FS_CALLS fsCalls[];
#define FS_COUNT		NELEMENTS(fsCalls)


/*******************************************************************************************
 *                                  Prototypes
********************************************************************************************/
FS_CALLS const * FSByName(char const *pName);

#endif
