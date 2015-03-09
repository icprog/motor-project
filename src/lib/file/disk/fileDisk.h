/*******************************************************************************************
Provides a disk-layer in the file system architecture

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __FILE_DISK_H__
#define __FILE_DISK_H__

#include "file.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
	int fsId;
	FFILE	userHandle;
} FFILE_HANDLE;

typedef struct
{
	BOOL	shouldRecurse;
	void*	pFsDirHandle;
	FDIR	userHandle;
	char	searchName[F_MAX_NAME];
	char	currentPath[F_MAX_NAME];
}FFIND_HANDLE;

typedef enum
{
	FDISK_OK = 0,
	FDISK_ERROR,
	FDISK_WRITE_PROTECTED,
	FDISK_NOT_READY,
	FDISK_ARG_ERROR,
} FDISK_RESULT;

typedef enum
{
	FDISK_SYNC,
	FDISK_GET_SECTOR_COUNT,
	FDISK_GET_SECTOR_SIZE,
	FDISK_GET_BLOCK_SIZE,
	FDISK_ERASE_SECTOR,
	FDISK_STATUS
} FDISK_IOCTLS;

typedef FDISK_RESULT	(*PFN_DISK_OPEN)(void);
typedef FDISK_RESULT	(*PFN_DISK_CLOSE)(void);
typedef FDISK_RESULT (*PFN_DISK_READ)(void *, UINT32, UINT32);				// *pDest, sectAddress, sectCount
typedef FDISK_RESULT (*PFN_DISK_WRITE)(void const *, UINT32, UINT32);	// *pSrc, sectAddress, sectCount
typedef FDISK_RESULT (*PFN_DISK_IOCTL)(FDISK_IOCTLS, void *);				// control code, tx/rx arg/buf

// static memory allocation for each disk drive. Note that the file system's memory
// must be allocated by the disk itself, depending on which file system is being used
// (set pFileSystemData, and fileSystemType appropriately)
typedef struct
{
	FFILE_HANDLE		fileHandles[F_MAX_FILES_PER_DISK];
#ifndef FS_NO_FIND
	FFIND_HANDLE		fileFind[F_MAX_FINDS_PER_DISK];
#endif

	FDISK_NUM			diskNum;
	BOOL					isMounted;
	void *				pFsData;		// Dependent on file system (see fileSystem.h)

	// Each disk must provide the following functions:
	PFN_DISK_OPEN		pfnOpen;
	PFN_DISK_CLOSE		pfnClose;
	PFN_DISK_READ		pfnRead;
	PFN_DISK_WRITE		pfnWrite;
	PFN_DISK_IOCTL		pfnIoctl;	// must support all FDISK_IOCTLS
}FDISK;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void FileDiskReg(FDISK *pDisk);
FDISK * FileDiskFromNum(FDISK_NUM diskNum);

BOOL FileDiskMount(FDISK *pDisk);
BOOL FileDiskUnmount(FDISK *pDisk);

#endif
