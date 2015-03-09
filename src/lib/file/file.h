/*******************************************************************************************
Top-Level FILE API

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __FILE_H__
#define __FILE_H__

#include "includes.h"
#include <time.h>


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef F_MAX_NAME
#define F_MAX_NAME				(255)
#endif
#ifndef F_MAX_FILES_PER_DISK
#define F_MAX_FILES_PER_DISK	(10)
#endif
#ifndef F_MAX_FINDS_PER_DISK
#define F_MAX_FINDS_PER_DISK	(5)
#endif
#ifndef F_MAX_PRINTF_LEN
#define F_MAX_PRINTF_LEN		(512)
#endif

#define FDISK_NUM_IS_VALID(n)	(((UINT8F)(n)) < (UINT8F)FDISK_COUNT)
#define FILE_IS_ERROR(n)		(((int)(n) < 0) && ((int)(n) >= (int)FERR_CODE_END))


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef unsigned int FFILE;
typedef unsigned int FDIR;

#define FILE_SD		"0:/"		// Must match FDISK_NUM
typedef enum
{
	FDISK_SD,
	FDISK_COUNT
}FDISK_NUM;

typedef struct
{
	FDISK_NUM	diskNum;
	BOOL			isMounted;
	UINT32		blockSize;
	UINT32		sectorSize;
	UINT32		sectorCount;
	UINT32		sectorFreeCount;
}FDISK_STATS;

typedef enum
{
	FOPEN_READ,				// "r" file must exist.
	FOPEN_WRITE,			// "w" creates/erases file.
	FOPEN_APPEND,			// "a" appends. Creates file if doesn't exist
	FOPEN_READ_PLUS,		// "r+" reading & writing. File must exist.
	FOPEN_WRITE_PLUS,		// "w+" creates/erases file for reading & writing
	FOPEN_APPEND_PLUS,	// "a+" opens file for r/w, seeks to end. Creates file if it doesn't exist
	FOPEN_END
}FOPEN_MODE;

typedef enum
{
	FSEEK_SET,		// aka "beginning of file"
	FSEEK_CUR,		// aka "current position of file"
	FSEEK_END,		// End of file
}FSEEK_ORIGIN;

typedef enum
{
	FFIND_TYPE_DIR,
	FFIND_TYPE_FILE,
	FFIND_TYPE_DONE
}FFIND_TYPE;

typedef struct
{
	FDIR		handle;
	FFIND_TYPE foundType;
	BOOL		readOnly;
	UINT32	fileSize;
	time_t	createTime;
	time_t	lastModifyTime;
	char		fileName[F_MAX_NAME];
}FFIND;


typedef int FATTRIB;
#define FA_RO	((FATTRIB) BIT0)	// read-only
#define FA_HID	((FATTRIB) BIT1)	// hidden
#define FA_SYS	((FATTRIB) BIT2)	// system
#define FA_DIR	((FATTRIB) BIT3)	// directory
#define FA_ARC	((FATTRIB) BIT4)	// archive

typedef struct
{
	FATTRIB	st_mode;		// attributes
	int		st_size;		// Total size (bytes)
	time_t	st_ctime;	// Time of creation
	time_t	st_mtime;	// Time of last modification
}FSTAT;

typedef enum
{
	FERR_NONE = 0,
	FERR_DISK = -1,
	FERR_NO_FILE = -2,
	FERR_INVALID = -3,
	FERR_ACCESS = -4,
	FERR_FORMAT = -5,
	FERR_TOO_MANY = -6,
	FERR_UNKNOWN = -7,
	FERR_CODE_END = -8,
} FERR;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
BOOL FileDevIsMounted(FDISK_NUM disk);
void FileDevInfo(FDISK_NUM disk, FDISK_STATS *pDest);

//lint -esym(534, FileClose, FileGets, FilePutc, FilePuts, FilePrintf, FileSeek, FileFlush)
FFILE FileOpen(char const *pName, FOPEN_MODE openMode);
int FileClose(FFILE file);

int FileRead(FFILE file, void *pDest, int len);
char * FileGets(FFILE file, char *pStr, int maxLen);
long FileSeek(FFILE file, long offset, FSEEK_ORIGIN origin);
long FileTell(FFILE file);
int FileEof(FFILE file);
int FileFStat(FFILE file, FSTAT *pDest);
int FileStat(char const *pName, FSTAT *pDest);

#ifndef FS_READONLY
int FileWrite(FFILE file, void const *pSrc, int len);
int FilePutc(FFILE file, char character);
int FilePuts(FFILE file, char const *pString);
int FilePrintf(FFILE file, const char *pFormat, ...);

int FileFlush(FFILE file);
int FileRename(char const *pOld, char const *pNew);
int FileRemove(char const *pName);

int FileMkDir(char const *pName);
#endif

#ifndef FS_NO_FIND
int FileFindFirst(char const *pName, BOOL shouldRecurse, FFIND *pFind);
int FileFindNext(FFIND *pFind);
int FileFindClose(FFIND *pFind);
#endif

#endif
