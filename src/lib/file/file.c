/*******************************************************************************************
Top-Level FILE API

This file provides all of the file API calls for the disk(s) and file system(s). It is layered:
[file API] --> [Disk] --> [File System translation] --> [actual file system (i.e. FAT)]

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "fileSystem.h"
#include "futils.h"
#include <stdio.h>		// vsprintf()


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define FILE_DISK_OFFSET				(3)		// Reserve disks 0-2 for stdin/stdout/stderr
#define FFILE_FIND_FLAG					BIT15

#define FILE_ASSERT_RET(n, ret)		{if (!(n)) return ret;}
#define FILE_ASSERT_VALID_HANDLE(h)	FILE_ASSERT_RET(FileHandleIsValid(h), (int)FERR_INVALID)
#define FILE_ASSERT_VALID_DISK(d)	FILE_ASSERT_RET(FDISK_NUM_IS_VALID(d), (int)FERR_INVALID)


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static UINT16 fileHandleCounter = 0;
static FDISK *pFileDisks[FDISK_COUNT] = {NULL};


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static FFILE_HANDLE * FileHandleOpen(FDISK *pDisk);
static void FileHandleClose(FFILE_HANDLE *pHandle);
static FFILE FileHandleGetNew(FDISK_NUM disk, UINT16F index, BOOL find);
static FDISK_NUM FileDiskFromHandle(FFILE file);
static UINT16F FileIndexFromHandle(FFILE file);
static FFILE_HANDLE * FileFromHandle(FFILE file);

static int FileIdFromHandle(FFILE file);
static BOOL FileHandleIsValid(FFILE file);
static FDISK_NUM FileDiskNumFromStr(char const *pStr);

#ifndef FS_READONLY
static void FileCreatePath(FDISK_NUM disk, char const *pName);
#endif

#ifndef FS_NO_FIND
static FFIND_HANDLE * FileFindHandleOpen(FDISK *pDisk);
static void FileFindHandleClose(FFIND_HANDLE *pHandle);
static FFIND_HANDLE * FileFindFromHandle(FFILE file);
static BOOL FileUpDir(FFIND_HANDLE *pHandle, FFIND *pFind);
#ifndef FS_READONLY
static int FileRemoveMask(char const *pName);
#endif
#endif


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
BOOL FileDevIsMounted(FDISK_NUM disk)
{
	return FDISK_NUM_IS_VALID(disk) && pFileDisks[disk]->isMounted;
}

void FileDevInfo(FDISK_NUM disk, FDISK_STATS *pDest)
{
	if (pDest && FDISK_NUM_IS_VALID(disk))
	{
		FDISK *pDisk = pFileDisks[disk];
		pDest->diskNum = disk;
		pDest->isMounted = pDisk->isMounted;
		fsCalls[disk].pfnDiskStats(pDisk, pDest);	//lint !e534
	}
}

FFILE FileOpen(char const *pName, FOPEN_MODE openMode)
{
	FDISK_NUM disk = FileDiskNumFromStr(pName);
	FFILE_HANDLE *pFile;
	FFILE file;

	if (!FDISK_NUM_IS_VALID(disk))
	{
		file = (FFILE)FERR_INVALID;
	}
#ifdef FS_READONLY
	else if (openMode != FOPEN_READ)
	{
		file = (FFILE)FERR_INVALID;
	}
#endif
	else
	{
		FDISK *pDisk = pFileDisks[disk];
		pFile = FileHandleOpen(pDisk);
		if (pFile)
		{
#ifndef FS_READONLY
			// Feature: Automatically create directories as required
			if ((openMode != FOPEN_READ) && (openMode != FOPEN_READ_PLUS))
			{
				FileCreatePath(pDisk->diskNum, pName);
			}
#endif

			file = (FFILE)fsCalls[disk].pfnOpen(pDisk->pFsData, pName, openMode);
			if (FILE_IS_ERROR(file))
			{
				FileHandleClose(pFile);
			}
			else
			{
				pFile->fsId = (int)file;
				file = pFile->userHandle;
			}
		}
		else
		{
			file = (FFILE)FERR_TOO_MANY;
		}
	}
	return file;
}

int FileClose(FFILE file)
{
	int retVal;
	FDISK *pDisk;
	FILE_ASSERT_VALID_HANDLE(file);
	pDisk = pFileDisks[FileDiskFromHandle(file)];
 	retVal = fsCalls[pDisk->diskNum].pfnClose(FileIdFromHandle(file));
	FileHandleClose(FileFromHandle(file));
	return retVal;
}

int FileRead(FFILE file, void *pDest, int len)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnRead(FileIdFromHandle(file), pDest, len);
}

// Read until \n (and includes \n in the string)
char * FileGets(FFILE file, char *pStr, int maxLen)
{
	FILE_ASSERT_RET(FileHandleIsValid(file), NULL);
	PFN_FREAD pfnRead = fsCalls[FileDiskFromHandle(file)].pfnRead;
	int fsId = FileIdFromHandle(file);

	int readLen = 0;
	maxLen -= 1;
	while (readLen < maxLen)
	{
		if (fsCalls[FileDiskFromHandle(file)].pfnEof(fsId))
		{
			break;
		}
		else if (pfnRead(fsId, &pStr[readLen], 1) != 1)
		{
			return NULL;
		}
		else if (pStr[readLen++] == '\n')
			break;
	}
	if (readLen > 0)
	{
		pStr[readLen] = '\0';
	}
	return pStr;
}

long FileSeek(FFILE file, long offset, FSEEK_ORIGIN origin)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnSeek(FileIdFromHandle(file), offset, origin);
}

long FileTell(FFILE file)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnTell(FileIdFromHandle(file));
}

int FileEof(FFILE file)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnEof(FileIdFromHandle(file));
}

int FileFStat(FFILE file, FSTAT *pDest)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnFStat(FileIdFromHandle(file), pDest);
}

int FileStat(char const *pName, FSTAT *pDest)
{
	FILE_ASSERT_VALID_DISK(FileDiskNumFromStr(pName));
	return fsCalls[FileDiskNumFromStr(pName)].pfnStat(pName, pDest);
}

#ifndef FS_READONLY
int FileWrite(FFILE file, void const *pSrc, int len)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnWrite(FileIdFromHandle(file), pSrc, len);
}

int FilePutc(FFILE file, char character)
{
	return FileWrite(file, &character, 1);
}

int FilePuts(FFILE file, char const *pString)
{
	return FileWrite(file, pString, (int)strlen(pString));
}

int FilePrintf(FFILE file, const char *pFormat, ...)
{
	char buf[F_MAX_PRINTF_LEN];
	int len;
	va_list pArg;
	va_start(pArg, pFormat);
	len = vsnprintf(buf, sizeof(buf), pFormat, pArg);	//lint !e534
	va_end(pArg);
	return FileWrite(file, buf, len);
}

int FileFlush(FFILE file)
{
	FILE_ASSERT_VALID_HANDLE(file);
	return fsCalls[FileDiskFromHandle(file)].pfnFlush(FileIdFromHandle(file));
}

int FileRename(char const *pOld, char const *pNew)
{
	FILE_ASSERT_VALID_DISK(FileDiskNumFromStr(pOld));
	return fsCalls[FileDiskNumFromStr(pOld)].pfnRename(pOld, pNew);
}

int FileRemove(char const *pName)
{
	int retVal;
	FDISK_NUM disk = FileDiskNumFromStr(pName);
	FILE_ASSERT_VALID_DISK(FileDiskNumFromStr(pName));
	retVal = fsCalls[disk].pfnRemove(pName);
#ifndef FS_NO_FIND
	if ((retVal == FERR_ACCESS) || (retVal == FERR_INVALID))
	{
		retVal = FileRemoveMask(pName);
	}
#endif
	return retVal;
}

int FileMkDir(char const *pName)
{
	FILE_ASSERT_VALID_DISK(FileDiskNumFromStr(pName));
	return fsCalls[FileDiskNumFromStr(pName)].pfnMkDir(pName);
}
#endif

#ifndef FS_NO_FIND
int FileFindFirst(char const *pName, BOOL shouldRecurse, FFIND *pFind)
{
	FDISK_NUM disk = FileDiskNumFromStr(pName);
	FDISK *pDisk = pFileDisks[disk];
	FFIND_HANDLE *pHandle;
	char *pPathEnd;

	if (!pFind || !FDISK_NUM_IS_VALID(disk) ||
		 ((pHandle = FileFindHandleOpen(pDisk)) == NULL))
	{
		return 0;
	}
	pFind->handle = pHandle->userHandle;
	strncpy(pHandle->searchName, pName, sizeof(pHandle->searchName)-1);
	pHandle->searchName[sizeof(pHandle->searchName)-1] = '\0';

	// Set current directory
	strncpy(pHandle->currentPath, pName, sizeof(pHandle->currentPath)-1);
	pHandle->currentPath[sizeof(pHandle->currentPath)-1] = '\0';
	pPathEnd = (char *)FutilsFileName(pHandle->currentPath);
	if (pPathEnd)
	{
		// FAT library has trouble opening directory with trailing '/' - so remove it (except root dir)
		if (pPathEnd > &pHandle->currentPath[3])
			pPathEnd--;
		*pPathEnd = '\0';
	}

	pHandle->pFsDirHandle = fsCalls[disk].pfnOpenDir(pDisk->pFsData, pHandle->currentPath);
	if (pPathEnd && (pPathEnd > &pHandle->currentPath[3]))
	{
		// return the '/' to the current path
		*pPathEnd++ = '/';
		*pPathEnd = '\0';
	}

	if (pHandle->pFsDirHandle)
	{
		pHandle->shouldRecurse = shouldRecurse;
		return FileFindNext(pFind);
	}
	else
	{
		FileFindHandleClose(pHandle);
		return FERR_NO_FILE;
	}
}

int FileFindNext(FFIND *pFind)
{
	FDISK_NUM disk = FileDiskFromHandle(pFind->handle);
	FFIND_HANDLE *pHandle;
	int retVal;

	if (!pFind || !FileHandleIsValid(pFind->handle))
	{
		return 0;
	}

	pHandle = &pFileDisks[disk]->fileFind[FileIndexFromHandle(pFind->handle)];
	while (pHandle->pFsDirHandle)
	{
		strcpy(pFind->fileName, pHandle->currentPath);
		retVal = fsCalls[disk].pfnReadDir(pFind, pHandle->pFsDirHandle);
		if (retVal == FERR_NONE)
		{
			if ((*pFind->fileName != '.') &&
				 FutilsWildcardNameIsMatch(FutilsFileName(pHandle->searchName), pFind->fileName))
			{
				break; // Found a match!
			}
			else if ((pFind->foundType == FFIND_TYPE_DIR) && pHandle->shouldRecurse)
			{
				strcpy(pHandle->currentPath, pFind->fileName);
				fsCalls[disk].pfnCloseDir(pHandle->pFsDirHandle);	//lint !e534
				pHandle->pFsDirHandle = fsCalls[disk].
					pfnOpenDir(pFileDisks[disk]->pFsData, pHandle->currentPath);
				strncat(pHandle->currentPath, "/", F_MAX_NAME);
			}
		}
		else if (pHandle->shouldRecurse && FileUpDir(pHandle, pFind))
		{
			// Continue searching from just past where we first recursed into the directory
		}
		else
		{
			pHandle->pFsDirHandle = NULL;
		}
	}
	if (!pHandle->pFsDirHandle)
	{
		return FileFindClose(pFind);
	}
	return 0;
}

int FileFindClose(FFIND *pFind)
{
	int retVal;
	FDISK *pDisk;
	FFIND_HANDLE *pHandle;
	if (!FileHandleIsValid(pFind->handle))
		return 0;

	pDisk = pFileDisks[FileDiskFromHandle(pFind->handle)];
	pHandle = FileFindFromHandle(pFind->handle);
 	retVal = fsCalls[pDisk->diskNum].pfnCloseDir(pHandle->pFsDirHandle);
	FileFindHandleClose(pHandle);
	pFind->foundType = FFIND_TYPE_DONE;
	return retVal;
}
#endif


/**************************************************************************
 *                                 "Protected" Disk Functions
 **************************************************************************/
void FileDiskReg(FDISK *pDisk)
{
	if (FDISK_NUM_IS_VALID(pDisk->diskNum) && (pFileDisks[pDisk->diskNum] == NULL))
	{
		pFileDisks[pDisk->diskNum] = pDisk;
		pDisk->isMounted = FALSE;
	}
}

FDISK * FileDiskFromNum(FDISK_NUM diskNum)
{
	return FDISK_NUM_IS_VALID(diskNum)? pFileDisks[diskNum] : NULL;
}

BOOL FileDiskMount(FDISK *pDisk)
{
	pDisk->isMounted = (fsCalls[pDisk->diskNum].pfnMount(pDisk) == FDISK_OK);
	return pDisk->isMounted;
}

BOOL FileDiskUnmount(FDISK *pDisk)
{
	pDisk->isMounted = (fsCalls[pDisk->diskNum].pfnUnmount(pDisk) == FDISK_OK);
	return !pDisk->isMounted;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static FFILE_HANDLE * FileHandleOpen(FDISK *pDisk)
{
	FFILE_HANDLE *pFile = pDisk->fileHandles;
	for (UINT16F i = 0; i < F_MAX_FILES_PER_DISK; i++, pFile++)
	{
		if (pFile->userHandle == 0)
		{
			pFile->userHandle = FileHandleGetNew(pDisk->diskNum, i, FALSE);
			return pFile;
		}
	}
	return NULL;
}

static void FileHandleClose(FFILE_HANDLE *pHandle)
{
	pHandle->userHandle = 0;
}

#ifndef FS_NO_FIND
static FFIND_HANDLE * FileFindHandleOpen(FDISK *pDisk)
{
	FFIND_HANDLE *pFind = pDisk->fileFind;
	for (UINT16F i = 0; i < F_MAX_FINDS_PER_DISK; i++, pFind++)
	{
		if (pFind->userHandle == 0)
		{
			pFind->userHandle = FileHandleGetNew(pDisk->diskNum, i, TRUE);
			return pFind;
		}
	}
	return NULL;
}

static void FileFindHandleClose(FFIND_HANDLE *pHandle)
{
	pHandle->userHandle = 0;
}
#endif

// File Handle encoding:
// [31] = always zero (reserved for negative error codes)
// [30:16] = rolling counter
// [15]   = file (0) or find(1) flag
// [14:4] = file index number
// [3:0]  = disk number (0-2 reserved for stdin, stdout, stderr)
static FFILE FileHandleGetNew(FDISK_NUM disk, UINT16F index, BOOL find)
{
	FFILE fil = (FFILE)(((UINT32)fileHandleCounter << 16) |
							  ((index & 0x7FF) << 4) |
							  ((UINT32)disk + FILE_DISK_OFFSET));
	if (find)
		fil |= FFILE_FIND_FLAG;
	if (++fileHandleCounter > INT16_MAX)
		fileHandleCounter = 0;
	return fil;
}


static FDISK_NUM FileDiskFromHandle(FFILE file)
{
	return (FDISK_NUM)((file & 0x0F) - FILE_DISK_OFFSET);
}

static UINT16F FileIndexFromHandle(FFILE file)
{
	return (UINT16F)((file & 0x7FF0) >> 4);
}

static FFILE_HANDLE * FileFromHandle(FFILE file)
{
	return &pFileDisks[FileDiskFromHandle(file)]->fileHandles[FileIndexFromHandle(file)];
}

#ifndef FS_NO_FIND
static FFIND_HANDLE * FileFindFromHandle(FFILE file)
{
	return &pFileDisks[FileDiskFromHandle(file)]->fileFind[FileIndexFromHandle(file)];
}
#endif

static int FileIdFromHandle(FFILE file)
{
	return FileFromHandle(file)->fsId;
}

static BOOL FileHandleIsValid(FFILE file)
{
	FDISK_NUM disk = FileDiskFromHandle(file);
	UINT16F num;
	BOOL valid;
	if (FDISK_NUM_IS_VALID(disk))
	{
		num = FileIndexFromHandle(file);
#ifndef FS_NO_FIND
		if (file & FFILE_FIND_FLAG)
		{
			valid = ((num < F_MAX_FINDS_PER_DISK) && (pFileDisks[disk]->fileFind[num].userHandle == file));
		}
		else
#endif
		{
			valid = ((num < F_MAX_FILES_PER_DISK) && (pFileDisks[disk]->fileHandles[num].userHandle == file));
		}
	}
	else
	{
		valid = FALSE;
	}
	return valid;
}

static FDISK_NUM FileDiskNumFromStr(char const *pStr)
{
	return (FDISK_NUM)(pStr[0] - '0');
}


#ifndef FS_READONLY
static void FileCreatePath(FDISK_NUM disk, char const *pName)
{
	char path[F_MAX_NAME];
	char *pCur;
	char *pNext;

	strncpy(path, pName, sizeof(path));
	pCur = strchr(path, '\\');	// skip drive letter
	if (!pCur)
		pCur = strchr(path, '/');
	if (pCur)
	{
		do
		{
			pNext = strchr(pCur+1, '\\');
			if (!pNext)
				pNext = strchr(pCur+1, '/');

			if (pNext)
			{
				*pNext = '\0';
				fsCalls[disk].pfnMkDir(path);	//lint !e534 ignore errors if dir exists
				*pNext = '/';
				pCur = pNext;
			}
		}while (pNext);
	}
}
#endif

#ifndef FS_NO_FIND
static BOOL FileUpDir(FFIND_HANDLE *pHandle, FFIND *pFind)
{
	FDISK_NUM disk;
	char *pFolder = NULL;
	int res;

	// Don't go up beyond the starting search path
	if (strlen(pHandle->currentPath) > (UINT16F)(FutilsFileName(pHandle->searchName) - pHandle->searchName))
	{
		pFolder = FutilsUpDir(pHandle->currentPath);
		if (pFolder)
		{
			disk = FileDiskFromHandle(pFind->handle);
			pHandle->currentPath[strlen(pHandle->currentPath)-1] = '\0';
			fsCalls[disk].pfnCloseDir(pHandle->pFsDirHandle);	//lint !e534
			pHandle->pFsDirHandle = fsCalls[disk].
				pfnOpenDir(pFileDisks[disk]->pFsData, pHandle->currentPath);
			strncat(pHandle->currentPath, "/", F_MAX_NAME);

			if (pHandle->pFsDirHandle)
			{
				// Loop through the current directory until we hit the find dir we recursed on
				do
				{
					pFind->fileName[0] = '\0';
					res = fsCalls[disk].pfnReadDir(pFind, pHandle->pFsDirHandle);
				}while (!res && (strcmp(pFind->fileName, pFolder) != STR_CMP_MATCH));
			}
		}
	}
	return (pFolder != NULL);
}

#ifndef FS_READONLY
static int FileRemoveMask(char const *pName)
{
	FFIND find;
	UINT8F dirLevel = 0;
	FDISK_NUM disk = FileDiskNumFromStr(pName);
	int retVal = FileFindFirst(pName, FALSE, &find);

	while (retVal == FERR_NONE)
	{
		retVal = fsCalls[disk].pfnRemove(find.fileName);
		if (retVal == FERR_NONE)
		{
			retVal = FileFindNext(&find);
			if (find.foundType == FFIND_TYPE_DONE)
			{
				if (dirLevel == 0)
				{
					break;
				}
				else if (dirLevel == 1)
				{
					retVal = FileFindFirst(pName, FALSE, &find);
				}
				else if (dirLevel > 1)
				{
					FutilsUpDir(find.fileName);
					strcat(find.fileName, "*");
					retVal = FileFindFirst(find.fileName, FALSE, &find);
				}
				dirLevel--;
			}
		}
		else if ((retVal == FERR_ACCESS) && (find.foundType == FFIND_TYPE_DIR))
		{
			// directory not empty. Delete all sub-files, then try again
			dirLevel++;
			FileFindClose(&find);	//lint !e534
			strcat(find.fileName, "/*");
			retVal = FileFindFirst(find.fileName, FALSE, &find);
		}
		else
		{
			// error
			break;
		}
	}
	return retVal;
}
#endif
#endif

