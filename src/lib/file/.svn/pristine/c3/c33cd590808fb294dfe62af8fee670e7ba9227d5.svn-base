/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "fileFat.h"
#include "diskio.h"
#include "ff.h"
#include "threadUtils.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/


/**************************************************************************
 *                                  Variables
 **************************************************************************/


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static BYTE FileFatModeToFlags(FOPEN_MODE openMode);
static FIL * FileFatGetFreeFile(FAT_DISK *pFs);
static void FileFatFreeFile(FIL *pFile);
static time_t FileFatInfoToTime(WORD date, WORD tim);
static FATTRIB FileFatModeFromAttribute(BYTE fattrib);
static int FileFatErrorCode(FRESULT result);
static FDISK_IOCTLS FileFatGetIoctl(BYTE controlCode);

static time_t read_fattime (DWORD fTime);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
// Disk operations:
FDISK_RESULT FileFatMount(FDISK *pDisk)
{
	FAT_DISK *pFs = (FAT_DISK *)pDisk->pFsData;
	FATFS *pFatFs = &pFs->fileSystem;
	FDISK_RESULT result = FDISK_ERROR;

	pFatFs->drv = (BYTE)pDisk->diskNum;
	memset(pFs->dirs, 0, sizeof(pFs->dirs));
	if (f_mount(pFatFs->drv, pFatFs) == FR_OK)
	{
		// ELM FatFs doesn't actually do anything when mounting, so we have to perform an operation
		// in order to ensure that disk is okay
		DWORD freeClusters;
		char drive[4];
		sprintf(drive, "%1d:/", pDisk->diskNum);
		if (f_getfree(drive, &freeClusters, &pFatFs) == FR_OK)
		{
			result = FDISK_OK;
		}
	}
	return result;
}//lint !e818

FDISK_RESULT FileFatUnmount(FDISK *pDisk)
{
	FDISK_RESULT result;
	if (f_mount((BYTE)pDisk->diskNum, NULL) != FR_OK)
	{
		result = FDISK_ERROR;
	}
	else if (pDisk && pDisk->pfnClose)
	{
		result = pDisk->pfnClose();
	}
	else
	{
		result = FDISK_OK;
	}
	return result;
}//lint !e818

FDISK_RESULT FileFatGetStats(FDISK const *pDisk, FDISK_STATS *pStats)
{
	FDISK_RESULT result;
	FATFS *pFatFs = &((FAT_DISK *)pDisk->pFsData)->fileSystem;
	DWORD freeClusters;
	char drive[4];
	sprintf(drive, "%1d:/", pDisk->diskNum);

	if (f_getfree(drive, &freeClusters, &pFatFs) != FR_OK)
	{
		result = FDISK_ERROR;
	}
	else
	{
		result = FDISK_OK;
		pStats->diskNum = pDisk->diskNum;
		pStats->isMounted = pDisk->isMounted;
		pStats->blockSize = 1;		// 1 sector erase size
#if _MAX_SS != 512
		pStats->sectorSize = ((FAT_DISK *)pDisk->pFsData)->ssize;
#else
		pStats->sectorSize = 512;
#endif
		pStats->sectorCount = (pFatFs->n_fatent - 2) * pFatFs->csize;
		pStats->sectorFreeCount = (UINT32)freeClusters * pFatFs->csize;
	}
	return result;
}


// File Operations:
int FileFatFOpen(void *pFs, char const *pFileName, FOPEN_MODE openMode)
{
	int retVal;
	FAT_DISK *pDev = (FAT_DISK *)pFs;
	FIL *pFile = FileFatGetFreeFile(pDev);
	if (!pFile)
	{
		retVal = (int)FERR_TOO_MANY;
	}
	else
	{
		FRESULT result = f_open(pFile, pFileName, FileFatModeToFlags(openMode));
		if (result == FR_OK)
		{
			retVal = (int)pFile;

			// NOTE: FatFS doesn't support "append" directly, so we have to seek to the end
			if ((openMode == FOPEN_APPEND) || (openMode == FOPEN_APPEND_PLUS))
			{
				if (f_lseek(pFile, pFile->fsize) != FR_OK)
				{
					retVal = FileFatFClose((int)pFile);
				}
			}
		}
		else
		{
			FileFatFreeFile(pFile);
			retVal = FileFatErrorCode(result);
		}
	}
	return retVal;
}

int FileFatFClose(int fd)
{
	FRESULT result = f_close((FIL *)fd);
	FileFatFreeFile((FIL *)fd);
	return FileFatErrorCode(result);
}

int FileFatFRead(int fd, void *pDest, int len)
{
	UINT readLen;
	FRESULT res = f_read((FIL *)fd, pDest, (UINT)len, &readLen);
	return (res == FR_OK)? (int)readLen : FileFatErrorCode(res);
}

int FileFatFWrite(int fd, const void *pSrc, int len)
{
	UINT writeLen;
	FRESULT res = f_write((FIL *)fd, pSrc, (UINT32)len, &writeLen);
	return (res == FR_OK)? (int)writeLen : FileFatErrorCode(res);
}

long FileFatFSeek (int fd, long offset, FSEEK_ORIGIN origin)
{
	FIL *pFile = (FIL *)fd;
	FRESULT result;
	DWORD fileOffset = (origin == FSEEK_CUR)? pFile->fptr : (origin == FSEEK_END)? pFile->fsize : 0;

	if (offset < 0)
	{
		offset = -offset;
		if (offset > (long)fileOffset)
		{
			fileOffset = 0;
		}
		else
		{
			fileOffset -= (DWORD)offset;
		}
	}
	else
	{
		fileOffset += (DWORD)offset;
	}

	result = f_lseek(pFile, fileOffset);
	return (result == FR_OK)? FileFatFTell(fd) : (long)FileFatErrorCode(result);
}

long FileFatFTell(int fd)
{
	return (long)((FIL *)fd)->fptr;
}

int FileFatFEof(int fd)
{
	return f_eof((FIL *)fd);
}

int FileFatFFlush(int fd)
{
	FRESULT result = f_sync((FIL *)fd);
	return FileFatErrorCode(result);
}

int FileFatRename(char const *pOld, char const *pNew)
{
	FRESULT result = f_rename(pOld, pNew);
	return FileFatErrorCode(result);
}

int FileFatRemove(char const *pName)
{
	FRESULT result = f_unlink(pName);	// File or empty directory
	return FileFatErrorCode(result);
}

int FileFatFStat(int fd, FSTAT *pDest)
{
	FIL *pFile = (FIL *)fd;
	pDest->st_mode = FileFatModeFromAttribute(pFile->flag);
	pDest->st_size = (int)pFile->fsize;
	pDest->st_mtime = pDest->st_ctime =
		FileFatInfoToTime(LD_WORD(pFile->dir_ptr+/*DIR_WrtDate*/24),
								LD_WORD(pFile->dir_ptr+/*DIR_WrtTime*/22));
	return FERR_NONE;
}

int FileFatStat(char const *pName, FSTAT *pDest)
{
	FILINFO fInfo;
	FRESULT result;

#if _USE_LFN
	fInfo.lfname = NULL;
	fInfo.lfsize = 0;
#endif
	result = f_stat(pName, &fInfo);
	if (result == FR_OK)
	{
		pDest->st_mode = FileFatModeFromAttribute(fInfo.fattrib);
		pDest->st_size = (int)fInfo.fsize;
		pDest->st_mtime = pDest->st_ctime = FileFatInfoToTime(fInfo.fdate, fInfo.ftime);
	}
	return FileFatErrorCode(result);
}


// Directory operations:
int FileFatMkDir(char const *pName)
{
	FRESULT result = f_mkdir(pName);
	return FileFatErrorCode(result);
}

void * FileFatOpenDir(void *pFs, char const *pName)
{
	FAT_DISK *pDev = (FAT_DISK *)pFs;
	FRESULT result = FR_TOO_MANY_OPEN_FILES;
	FAT_DIR *pDir = NULL;

	for (UINT8F i = 0; i < F_MAX_FINDS_PER_DISK; i++)
	{
		if (!pDev->dirs[i].isOpen)
		{
			result = f_opendir(&pDev->dirs[i].dir, pName);
			if (result == FR_OK)
			{
				pDev->dirs[i].isOpen = TRUE;
				pDir = &pDev->dirs[i];
			}
			break;
		}
	}
	return pDir;
}

int FileFatReadDir(FFIND *pDest, void *pDirArg)
{
	FAT_DIR *pDir = (FAT_DIR *)pDirArg;
	FRESULT result;
	FILINFO fInfo;
#if _USE_LFN
	fInfo.lfname = &pDest->fileName[strlen(pDest->fileName)];
	fInfo.lfsize = sizeof(pDest->fileName) - (UINT32)(fInfo.lfname - pDest->fileName);
#endif

	result = f_readdir(&pDir->dir, &fInfo);
	if ((result == FR_OK) && (fInfo.fname[0] != '\0'))
	{
		pDest->foundType = (fInfo.fattrib & AM_DIR)? FFIND_TYPE_DIR: FFIND_TYPE_FILE;
		pDest->readOnly = (fInfo.fattrib & AM_RDO)? TRUE : FALSE;
		pDest->createTime = pDest->lastModifyTime = FileFatInfoToTime(fInfo.fdate, fInfo.ftime);
#if _USE_LFN
		if (*fInfo.lfname == '\0')
#endif
		{
			strcat(pDest->fileName, fInfo.fname);
		}
	}
	else
	{
		pDest->foundType = FFIND_TYPE_DONE;
		result = FR_NO_FILE;
		if (pDir)
			pDir->isOpen = FALSE;
	}
	return FileFatErrorCode(result);
}

int FileFatCloseDir(void *pDirArg)
{
	if (pDirArg)
	{
		((FAT_DIR *)pDirArg)->isOpen = FALSE;
	}
	return (int)FERR_NONE;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static BYTE FileFatModeToFlags(FOPEN_MODE openMode)
{
	static const BYTE openFlags[FOPEN_END] =
	{
		[FOPEN_READ]			= (FA_READ | FA_OPEN_EXISTING),
		[FOPEN_WRITE]			= (FA_WRITE | FA_CREATE_ALWAYS),
		[FOPEN_APPEND]			= (FA_WRITE | FA_OPEN_ALWAYS),
		[FOPEN_READ_PLUS]		= (FA_READ | FA_WRITE | FA_OPEN_EXISTING),
		[FOPEN_WRITE_PLUS]	= (FA_READ | FA_WRITE | FA_CREATE_ALWAYS),
		[FOPEN_APPEND_PLUS]	= (FA_READ | FA_WRITE | FA_OPEN_ALWAYS)
	};
	return (openMode < FOPEN_END)? openFlags[openMode] : 0;
}

static FIL * FileFatGetFreeFile(FAT_DISK *pFs)
{
	FIL *pFile = NULL;
	for (int i = 0; i < F_MAX_FILES_PER_DISK; i++)
	{
		if (pFs->files[i].fs == NULL)
		{
			pFile = &pFs->files[i];
			pFile->fs = &pFs->fileSystem;
			break;
		}
	}
	return pFile;
}

static void FileFatFreeFile(FIL *pFile)
{
	if (pFile)
	{
		pFile->fs = NULL;
	}
}

static time_t FileFatInfoToTime(WORD fdate, WORD ftime)
{
	return read_fattime(((DWORD)fdate << 16) | (DWORD)ftime);
}

static FATTRIB FileFatModeFromAttribute(BYTE fattrib)
{
	FATTRIB attrib = 0;
	if (fattrib & AM_RDO)	attrib |= FA_RO;
	if (fattrib & AM_HID)	attrib |= FA_HID;
	if (fattrib & AM_SYS)	attrib |= FA_SYS;
	if (fattrib & AM_DIR)	attrib |= FA_DIR;
	if (fattrib & AM_ARC)	attrib |= FA_ARC;
	return attrib;
}

static int FileFatErrorCode(FRESULT result)
{
	FERR err;
	switch (result)
	{
	case FR_OK:							err = FERR_NONE;		break;
	case FR_DISK_ERR:					err = FERR_DISK;		break;
	case FR_INT_ERR:					err = FERR_UNKNOWN;	break;
	case FR_NOT_READY:				err = FERR_DISK;		break;
	case FR_NO_FILE:					err = FERR_NO_FILE;	break;
	case FR_NO_PATH:					err = FERR_NO_FILE;	break;
	case FR_INVALID_NAME:			err = FERR_INVALID;	break;
	case FR_DENIED:					err = FERR_ACCESS;	break;
	case FR_EXIST:						err = FERR_ACCESS;	break;
	case FR_INVALID_OBJECT:			err = FERR_INVALID;	break;
	case FR_WRITE_PROTECTED:		err = FERR_ACCESS;	break;
	case FR_INVALID_DRIVE:			err = FERR_DISK;		break;
	case FR_NOT_ENABLED:				err = FERR_DISK;		break;
	case FR_NO_FILESYSTEM:			err = FERR_FORMAT;	break;
	case FR_MKFS_ABORTED:			err = FERR_FORMAT;	break;
	case FR_TIMEOUT:					err = FERR_DISK;		break;
	case FR_LOCKED:					err = FERR_ACCESS;	break;
	case FR_NOT_ENOUGH_CORE:		err = FERR_DISK;		break;
	case FR_TOO_MANY_OPEN_FILES:	err = FERR_TOO_MANY;	break;
	default:								err = FERR_UNKNOWN;	break;
	}
	return (int)err;
}

static FDISK_IOCTLS FileFatGetIoctl(BYTE controlCode)
{
	FDISK_IOCTLS ioctl;
	switch (controlCode)
	{
	default:
	case CTRL_SYNC:			ioctl = FDISK_SYNC;					break;
	case GET_SECTOR_SIZE:	ioctl = FDISK_GET_SECTOR_SIZE;	break;
	case GET_SECTOR_COUNT:	ioctl = FDISK_GET_SECTOR_COUNT;	break;
	case GET_BLOCK_SIZE:		ioctl = FDISK_GET_BLOCK_SIZE;		break;
	case CTRL_ERASE_SECTOR:	ioctl = FDISK_ERASE_SECTOR;		break;
	}
	return ioctl;
}



/**************************************************************************
 *    									  FAT Driver Hooks (diskio.h)
 *
 * These functions are called by ELM-Chan FatFs to read/write to the disk
 **************************************************************************/
DWORD get_fattime (void)
{
#ifdef FAT_TIME_DIS
   return 0;
#else
   struct tm timeStruct;
   time_t tm = time(NULL);
   localtime_r(&tm, &timeStruct);

   /* Pack date and time into a DWORD variable */
   return ( ((DWORD)(timeStruct.tm_year - 80) << 25)	// Bits 31:25 Set years since 1980 (tm_year is since 1900)
            |((DWORD)(timeStruct.tm_mon + 1) << 21)	// Bits 24:21 Month 1-12 (tm_mon is 0-11)
            |((DWORD)timeStruct.tm_mday << 16)			// Bits 20:16 Day of month (tm_mday matches)
            |((DWORD)timeStruct.tm_hour << 11)			// Bits 15:11 Hour 0-23
            |((DWORD)timeStruct.tm_min << 5)				// Bits 10:5  Minute 0-59
            |((DWORD)timeStruct.tm_sec >> 1));			// Bits 4:0   Seconds / 2
#endif
}

static time_t read_fattime (DWORD fTime)
{
#ifdef FAT_TIME_DIS
   return 0;
#else
   struct tm timeStruct;
   timeStruct.tm_year =  (fTime >> 25) + 80;
   timeStruct.tm_hour = ((fTime & (BIT21 | BIT22 | BIT23 | BIT24)) >> 21) - 1;
   timeStruct.tm_mday = ((fTime & (BIT20 | BIT19 | BIT18 | BIT17 | BIT16)) >> 16);
   timeStruct.tm_hour = ((fTime & (BIT15 | BIT14 | BIT13 | BIT12 | BIT11)) >> 11);
   timeStruct.tm_min  = ((fTime & (BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5)) >> 5);
   timeStruct.tm_sec  = ((fTime & (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)) << 1);
   return mktime(&timeStruct);
#endif
}//lint !e715


DSTATUS disk_initialize(BYTE drvNum)
{
	FDISK *pDisk = FileDiskFromNum((FDISK_NUM)drvNum);
	return pDisk? (DSTATUS)pDisk->pfnOpen() : STA_NOINIT;
}

DSTATUS disk_status(BYTE drvNum)
{
	FDISK *pDisk = FileDiskFromNum((FDISK_NUM)drvNum);
	return pDisk? (DSTATUS)pDisk->pfnIoctl(FDISK_STATUS, NULL) : STA_NOINIT;
}

DRESULT disk_read(BYTE drvNum, BYTE *buff, DWORD sectorAddress, BYTE sectorCount)
{
	FDISK *pDisk = FileDiskFromNum((FDISK_NUM)drvNum);
	return pDisk?
		(DRESULT)pDisk->pfnRead(buff, (UINT32)sectorAddress, (UINT32)sectorCount) :
		RES_NOTRDY;
}

DRESULT disk_write(BYTE drvNum, const BYTE *buff, DWORD sectorAddress, BYTE sectorCount)
{
	FDISK *pDisk = FileDiskFromNum((FDISK_NUM)drvNum);
	return pDisk?
		(DRESULT)pDisk->pfnWrite(buff, (UINT32)sectorAddress, (UINT32)sectorCount) :
		RES_NOTRDY;
}

DRESULT disk_ioctl(BYTE drvNum, BYTE controlCode, void *pTxRxDataBuf)
{
	FDISK *pDisk = FileDiskFromNum((FDISK_NUM)drvNum);
	return pDisk?
		(DRESULT)pDisk->pfnIoctl(FileFatGetIoctl(controlCode), pTxRxDataBuf) :
		RES_NOTRDY;
}

