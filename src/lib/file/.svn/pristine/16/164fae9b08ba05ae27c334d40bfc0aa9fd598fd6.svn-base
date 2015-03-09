/*******************************************************************************************
This provides a single unified API for all different file systems
Note that the interface is mostly POSIX-compliant, but in order to avoid dynamic memory,
some routines pass in the file system workspace as an argument

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "fileSystem.h"

#ifndef FS_READONLY
#define FS_WRITE_EX_FNS(name) \
extern int          name##FWrite(int fd, const void *pSrc, int len);            \
extern int          name##FFlush(int fd);                                       \
extern int          name##Rename(char const *pOld, char const *pNew);           \
extern int          name##Remove(char const *pName);                            \
extern int          name##MkDir   (char const *pName);
#else
#define FS_WRITE_EX_FNS(name)
#endif
#ifndef FS_NO_FIND
#define FS_FIND_EX_FNS(name) \
extern void *       name##OpenDir (void *pFs, char const *pName);               \
extern int          name##ReadDir (FFIND *pDest, void *pDirArg);                \
extern int          name##CloseDir(void *pDirArg);
#else
#define FS_FIND_EX_FNS(name)
#endif

#define FS_NAME(name)	\
extern FDISK_RESULT name##Mount(FDISK *pDisk);                                  \
extern FDISK_RESULT name##Unmount(FDISK *pDisk);                                \
extern FDISK_RESULT name##GetStats(FDISK const *pDisk, FDISK_STATS *pStats);    \
extern int          name##FOpen (void *pFs, char const *name, FOPEN_MODE mode); \
extern int          name##FClose(int fd);                                       \
extern int          name##FRead (int fd, void *pDest, int len);                 \
extern long         name##FSeek (int fd, long offset, FSEEK_ORIGIN origin);     \
extern long         name##FTell (int fd);                                       \
extern int          name##FEof  (int fd);                                       \
extern int          name##FStat (int fd, FSTAT *pDest);                         \
extern int          name##Stat  (char const *pName, FSTAT *pDest);              \
FS_WRITE_EX_FNS(name) \
FS_FIND_EX_FNS(name)
FILE_SYSTEMS
#undef FS_NAME


/**************************************************************************
 *                                  Constants
 **************************************************************************/
const FS_CALLS fsCalls[] =
{
#ifndef FS_READONLY
#define FS_WRITE_PFNS(name) name##FWrite, name##FFlush, name##Rename, name##Remove, name##MkDir,
#else
#define FS_WRITE_PFNS(name)
#endif
#ifndef FS_NO_FIND
#define FS_FIND_PFNS(name)	name##OpenDir, name##ReadDir, name##CloseDir,
#else
#define FS_FIND_PFNS(name)
#endif

#define FS_NAME(name)	\
	{name##Mount, name##Unmount, name##GetStats, \
	name##FOpen, name##FClose, name##FRead, name##FSeek, name##FTell, name##FEof, \
	name##FStat, name##Stat, \
	FS_WRITE_PFNS(name) \
	FS_FIND_PFNS(name) \
	},
FILE_SYSTEMS
#undef FS_NAME
};


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
FS_CALLS const * FSByName(char const *pName)
{
	static const char * pFsNames[] =
	{
		#define FS_NAME(name)	#name,
		FILE_SYSTEMS
		#undef FS_NAME
	};

	for (UINT8F i = 0; i < NELEMENTS(pFsNames); i++)
	{
		if (stricmp(pName, pFsNames[i]) == STR_CMP_MATCH)
		{
			return &fsCalls[i];
		}
	}
	return NULL;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

