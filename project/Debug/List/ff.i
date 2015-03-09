/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.09                  (C)ChaN, 2011
/-----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------/
/ Feb 26,'06 R0.00  Prototype.
/
/ Apr 29,'06 R0.01  First stable version.
/
/ Jun 01,'06 R0.02  Added FAT12 support.
/                   Removed unbuffered mode.
/                   Fixed a problem on small (<32M) partition.
/ Jun 10,'06 R0.02a Added a configuration option (_FS_MINIMUM).
/
/ Sep 22,'06 R0.03  Added f_rename().
/                   Changed option _FS_MINIMUM to _FS_MINIMIZE.
/ Dec 11,'06 R0.03a Improved cluster scan algorithm to write files fast.
/                   Fixed f_mkdir() creates incorrect directory on FAT32.
/
/ Feb 04,'07 R0.04  Supported multiple drive system.
/                   Changed some interfaces for multiple drive system.
/                   Changed f_mountdrv() to f_mount().
/                   Added f_mkfs().
/ Apr 01,'07 R0.04a Supported multiple partitions on a physical drive.
/                   Added a capability of extending file size to f_lseek().
/                   Added minimization level 3.
/                   Fixed an endian sensitive code in f_mkfs().
/ May 05,'07 R0.04b Added a configuration option _USE_NTFLAG.
/                   Added FSInfo support.
/                   Fixed DBCS name can result FR_INVALID_NAME.
/                   Fixed short seek (<= csize) collapses the file object.
/
/ Aug 25,'07 R0.05  Changed arguments of f_read(), f_write() and f_mkfs().
/                   Fixed f_mkfs() on FAT32 creates incorrect FSInfo.
/                   Fixed f_mkdir() on FAT32 creates incorrect directory.
/ Feb 03,'08 R0.05a Added f_truncate() and f_utime().
/                   Fixed off by one error at FAT sub-type determination.
/                   Fixed btr in f_read() can be mistruncated.
/                   Fixed cached sector is not flushed when create and close without write.
/
/ Apr 01,'08 R0.06  Added fputc(), fputs(), fprintf() and fgets().
/                   Improved performance of f_lseek() on moving to the same or following cluster.
/
/ Apr 01,'09 R0.07  Merged Tiny-FatFs as a configuration option. (_FS_TINY)
/                   Added long file name feature.
/                   Added multiple code page feature.
/                   Added re-entrancy for multitask operation.
/                   Added auto cluster size selection to f_mkfs().
/                   Added rewind option to f_readdir().
/                   Changed result code of critical errors.
/                   Renamed string functions to avoid name collision.
/ Apr 14,'09 R0.07a Separated out OS dependent code on reentrant cfg.
/                   Added multiple sector size feature.
/ Jun 21,'09 R0.07c Fixed f_unlink() can return FR_OK on error.
/                   Fixed wrong cache control in f_lseek().
/                   Added relative path feature.
/                   Added f_chdir() and f_chdrive().
/                   Added proper case conversion to extended char.
/ Nov 03,'09 R0.07e Separated out configuration options from ff.h to ffconf.h.
/                   Fixed f_unlink() fails to remove a sub-dir on _FS_RPATH.
/                   Fixed name matching error on the 13 char boundary.
/                   Added a configuration option, _LFN_UNICODE.
/                   Changed f_readdir() to return the SFN with always upper case on non-LFN cfg.
/
/ May 15,'10 R0.08  Added a memory configuration option. (_USE_LFN = 3)
/                   Added file lock feature. (_FS_SHARE)
/                   Added fast seek feature. (_USE_FASTSEEK)
/                   Changed some types on the API, XCHAR->TCHAR.
/                   Changed fname member in the FILINFO structure on Unicode cfg.
/                   String functions support UTF-8 encoding files on Unicode cfg.
/ Aug 16,'10 R0.08a Added f_getcwd(). (_FS_RPATH = 2)
/                   Added sector erase feature. (_USE_ERASE)
/                   Moved file lock semaphore table from fs object to the bss.
/                   Fixed a wrong directory entry is created on non-LFN cfg when the given name contains ';'.
/                   Fixed f_mkfs() creates wrong FAT32 volume.
/ Jan 15,'11 R0.08b Fast seek feature is also applied to f_read() and f_write().
/                   f_lseek() reports required table size on creating CLMP.
/                   Extended format syntax of f_printf function.
/                   Ignores duplicated directory separators in given path names.
/
/ Sep 06,'11 R0.09  f_mkfs() supports multiple partition to finish the multiple partition feature.
/                   Added f_fdisk(). (_MULTI_PARTITION = 2)
/---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.09     (C)ChaN, 2011
/----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following trems.
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/



/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/



/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef char			CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;


/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.09  (C)ChaN, 2011
/----------------------------------------------------------------------------/
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------/
/ Functions and Buffer Configurations
/----------------------------------------------------------------------------*/

/* When _FS_TINY is set to 1, FatFs uses the sector buffer in the file system
/  object instead of the sector buffer in the individual file object for file
/  data transfer. This reduces memory consumption 512 bytes each file object. */


/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */


/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/
/   0: Full function.
/   1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename
/      are removed.
/   2: f_opendir and f_readdir are removed in addition to 1.
/   3: f_lseek is removed in addition to 2. */


/* To enable string functions, set _USE_STRFUNC to 1 or 2. */


/* To enable f_mkfs function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */


/* To enable f_forward function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


/* To enable fast seek feature, set _USE_FASTSEEK to 1. */



/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/----------------------------------------------------------------------------*/

/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   932  - Japanese Shift-JIS (DBCS, OEM, Windows)
/   936  - Simplified Chinese GBK (DBCS, OEM, Windows)
/   949  - Korean (DBCS, OEM, Windows)
/   950  - Traditional Chinese Big5 (DBCS, OEM, Windows)
/   1250 - Central Europe (Windows)
/   1251 - Cyrillic (Windows)
/   1252 - Latin 1 (Windows)
/   1253 - Greek (Windows)
/   1254 - Turkish (Windows)
/   1255 - Hebrew (Windows)
/   1256 - Arabic (Windows)
/   1257 - Baltic (Windows)
/   1258 - Vietnam (OEM, Windows)
/   437  - U.S. (OEM)
/   720  - Arabic (OEM)
/   737  - Greek (OEM)
/   775  - Baltic (OEM)
/   850  - Multilingual Latin 1 (OEM)
/   858  - Multilingual Latin 1 + Euro (OEM)
/   852  - Latin 2 (OEM)
/   855  - Cyrillic (OEM)
/   866  - Russian (OEM)
/   857  - Turkish (OEM)
/   862  - Hebrew (OEM)
/   874  - Thai (OEM, Windows)
/	1    - ASCII only (Valid for non LFN cfg.)
*/


/* The _USE_LFN option switches the LFN support.
/
/   0: Disable LFN feature. _MAX_LFN and _LFN_UNICODE have no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. To enable LFN,
/  Unicode handling functions ff_convert() and ff_wtoupper() must be added
/  to the project. When enable to use heap, memory control functions
/  ff_memalloc() and ff_memfree() must be added to the project. */


/* To switch the character code set on FatFs API to Unicode,
/  enable LFN feature and set _LFN_UNICODE to 1. */


/* The _FS_RPATH option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path. f_chdrive() and f_chdir() are available.
/   2: f_getcwd() is available in addition to 1.
/
/  Note that output of the f_readdir fnction is affected by this option. */



/*---------------------------------------------------------------------------/
/ Physical Drive Configurations
/----------------------------------------------------------------------------*/

/* Number of volumes (logical drives) to be used. */


/* Maximum sector size to be handled.
/  Always set 512 for memory card and hard disk but a larger value may be
/  required for on-board flash memory, floppy disk and optical disk.
/  When _MAX_SS is larger than 512, it configures FatFs to variable sector size
/  and GET_SECTOR_SIZE command must be implememted to the disk_ioctl function. */


/* When set to 0, each volume is bound to the same physical drive number and
/ it can mount only first primaly partition. When it is set to 1, each volume
/ is tied to the partitions listed in VolToPart[]. */


/* To enable sector erase feature, set _USE_ERASE to 1. CTRL_ERASE_SECTOR command
/  should be added to the disk_ioctl functio. */



/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

/* Set 0 first and it is always compatible with all platforms. The _WORD_ACCESS
/  option defines which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access.
/   1: Word access. Do not choose this unless following condition is met.
/
/  When the byte order on the memory is big-endian or address miss-aligned word
/  access results incorrect behavior, the _WORD_ACCESS must be set to 0.
/  If it is not the case, the value can also be set to 1 to improve the
/  performance and code size.
*/


/* A header file that defines sync object types on the O/S, such as
/  windows.h, ucos_ii.h and semphr.h, must be included prior to ff.h. */


/* The _FS_REENTRANT option switches the reentrancy (thread safe) of the FatFs module.
/
/   0: Disable reentrancy. _SYNC_t and _FS_TIMEOUT have no effect.
/   1: Enable reentrancy. Also user provided synchronization handlers,
/      ff_req_grant, ff_rel_grant, ff_del_syncobj and ff_cre_syncobj
/      function must be added to the project. */


/* To enable file shareing feature, set _FS_SHARE to 1 or greater. The value
   defines how many files can be opened simultaneously. */






/* Definitions of volume management */





/* Type of path name strings on FatFs API */

typedef char TCHAR;




/* File system object structure (FATFS) */

typedef struct {
	BYTE	fs_type;		/* FAT sub-type (0:Not mounted) */
	BYTE	drv;			/* Physical drive number */
	BYTE	csize;			/* Sectors per cluster (1,2,4...128) */
	BYTE	n_fats;			/* Number of FAT copies (1,2) */
	BYTE	wflag;			/* win[] dirty flag (1:must be written back) */
	BYTE	fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	WORD	id;				/* File system mount ID */
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) */
	DWORD	last_clust;		/* Last allocated cluster */
	DWORD	free_clust;		/* Number of free clusters */
	DWORD	fsi_sector;		/* fsinfo sector (FAT32) */
	DWORD	n_fatent;		/* Number of FAT entries (= number of clusters + 2) */
	DWORD	fsize;			/* Sectors per FAT */
	DWORD	fatbase;		/* FAT start sector */
	DWORD	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	DWORD	database;		/* Data start sector */
	DWORD	winsect;		/* Current sector appearing in the win[] */
	BYTE	win[512];	/* Disk access window for Directory, FAT (and Data on tiny cfg) */
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	WORD	id;				/* Owner file system mount ID */
	BYTE	flag;			/* File status flags */
	BYTE	pad1;
	DWORD	fptr;			/* File read/write pointer (0 on file open) */
	DWORD	fsize;			/* File size */
	DWORD	sclust;			/* File start cluster (0 when fsize==0) */
	DWORD	clust;			/* Current cluster */
	DWORD	dsect;			/* Current data sector */
	DWORD	dir_sect;		/* Sector containing the directory entry */
	BYTE*	dir_ptr;		/* Ponter to the directory entry in the window */
	BYTE	buf[512];	/* File data read/write buffer */
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	WORD	id;				/* Owner file system mount ID */
	WORD	index;			/* Current read/write index number */
	DWORD	sclust;			/* Table start cluster (0:Root dir) */
	DWORD	clust;			/* Current cluster */
	DWORD	sect;			/* Current sector */
	BYTE*	dir;			/* Pointer to the current SFN entry in the win[] */
	BYTE*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
} DIR;



/* File status structure (FILINFO) */

typedef struct {
	DWORD	fsize;			/* File size */
	WORD	fdate;			/* Last modified date */
	WORD	ftime;			/* Last modified time */
	BYTE	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Acces denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_mount (BYTE, FATFS*);						/* Mount/Unmount a logical drive */
FRESULT f_open (FIL*, const TCHAR*, BYTE);			/* Open or create a file */
FRESULT f_read (FIL*, void*, UINT, UINT*);			/* Read data from a file */
FRESULT f_lseek (FIL*, DWORD);						/* Move file pointer of a file object */
FRESULT f_close (FIL*);								/* Close an open file object */
FRESULT f_opendir (DIR*, const TCHAR*);				/* Open an existing directory */
FRESULT f_readdir (DIR*, FILINFO*);					/* Read a directory item */
FRESULT f_stat (const TCHAR*, FILINFO*);			/* Get file status */
FRESULT f_write (FIL*, const void*, UINT, UINT*);	/* Write data to a file */
FRESULT f_getfree (const TCHAR*, DWORD*, FATFS**);	/* Get number of free clusters on the drive */
FRESULT f_truncate (FIL*);							/* Truncate file */
FRESULT f_sync (FIL*);								/* Flush cached data of a writing file */
FRESULT f_unlink (const TCHAR*);					/* Delete an existing file or directory */
FRESULT	f_mkdir (const TCHAR*);						/* Create a new directory */
FRESULT f_chmod (const TCHAR*, BYTE, BYTE);			/* Change attriburte of the file/dir */
FRESULT f_utime (const TCHAR*, const FILINFO*);		/* Change timestamp of the file/dir */
FRESULT f_rename (const TCHAR*, const TCHAR*);		/* Rename/Move a file or directory */
FRESULT f_chdrive (BYTE);							/* Change current drive */
FRESULT f_chdir (const TCHAR*);						/* Change current directory */
FRESULT f_getcwd (TCHAR*, UINT);					/* Get current directory */
FRESULT f_forward (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);	/* Forward data to the stream */
FRESULT f_mkfs (BYTE, BYTE, UINT);					/* Create a file system on the drive */
FRESULT	f_fdisk (BYTE, const DWORD[], void*);		/* Divide a physical drive into some partitions */
int f_putc (TCHAR, FIL*);							/* Put a character to the file */
int f_puts (const TCHAR*, FIL*);					/* Put a string to the file */
int f_printf (FIL*, const TCHAR*, ...);				/* Put a formatted string to the file */
TCHAR* f_gets (TCHAR*, int, FIL*);					/* Get a string from the file */






/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
DWORD get_fattime (void);

/* Unicode support functions */

/* Sync functions */




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */




/* FAT sub type (FATFS.fs_type) */



/* File attribute bits for directory entry */



/* Fast seek feature */



/*--------------------------------*/
/* Multi-byte word access macros  */



/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file
/-----------------------------------------------------------------------*/





/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

int assign_drives (int, int);
DSTATUS disk_initialize (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);
DRESULT disk_ioctl (BYTE, BYTE, void*);



/* Disk Status Bits (DSTATUS) */



/* Command code for disk_ioctrl fucntion */

/* Generic command (defined for FatFs) */

/* Generic command */

/* MMC/SDC specific ioctl command */

/* ATA/CF specific ioctl command */

/* NAND specific ioctl command */




/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/



/* Definitions on sector size */


/* Reentrancy related */



/* File shareing feature */


/* Misc definitions */


/* DBCS code ranges and SBCS extend char conversion table */




/* Character code support macros */





/* Name status flags */


/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */




/*------------------------------------------------------------*/
/* Module private work area                                   */
/*------------------------------------------------------------*/
/* Note that uninitialized variables with static duration are
/  zeroed/nulled at start-up. If not, the compiler or start-up
/  routine is out of ANSI-C standard.
*/

static
FATFS *FatFs[1];	/* Pointer to the file system objects (logical drives) */

static
WORD Fsid;				/* File system mount ID */








/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Copy memory to memory */
static
void mem_cpy (void* dst, const void* src, UINT cnt) {
	BYTE *d = (BYTE*)dst;
	const BYTE *s = (const BYTE*)src;

	while (cnt--)
		*d++ = *s++;
}

/* Fill memory */
static
void mem_set (void* dst, int val, UINT cnt) {
	BYTE *d = (BYTE*)dst;

	while (cnt--)
		*d++ = (BYTE)val;
}

/* Compare memory to memory */
static
int mem_cmp (const void* dst, const void* src, UINT cnt) {
	const BYTE *d = (const BYTE *)dst, *s = (const BYTE *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}

/* Check if chr is contained in the string */
static
int chk_chr (const char* str, int chr) {
	while (*str && *str != chr) str++;
	return *str;
}



/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* File shareing control functions                                       */
/*-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Change window offset                                                  */
/*-----------------------------------------------------------------------*/

static
FRESULT move_window (
	FATFS *fs,		/* File system object */
	DWORD sector	/* Sector number to make appearance in the fs->win[] */
)					/* Move to zero only writes back dirty window */
{
	DWORD wsect;


	wsect = fs->winsect;
	if (wsect != sector) {	/* Changed current window */
		if (fs->wflag) {	/* Write back dirty window if needed */
			if (disk_write(fs->drv, fs->win, wsect, 1) != RES_OK)
				return FR_DISK_ERR;
			fs->wflag = 0;
			if (wsect < (fs->fatbase + fs->fsize)) {	/* In FAT area */
				BYTE nf;
				for (nf = fs->n_fats; nf > 1; nf--) {	/* Reflect the change to all FAT copies */
					wsect += fs->fsize;
					disk_write(fs->drv, fs->win, wsect, 1);
				}
			}
		}
		if (sector) {
			if (disk_read(fs->drv, fs->win, sector, 1) != RES_OK)
				return FR_DISK_ERR;
			fs->winsect = sector;
		}
	}

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Clean-up cached data                                                  */
/*-----------------------------------------------------------------------*/
static
FRESULT sync (	/* FR_OK: successful, FR_DISK_ERR: failed */
	FATFS *fs	/* File system object */
)
{
	FRESULT res;


	res = move_window(fs, 0);
	if (res == FR_OK) {
		/* Update FSInfo sector if needed */
		if (fs->fs_type == 3 && fs->fsi_flag) {
			fs->winsect = 0;
			/* Create FSInfo structure */
			mem_set(fs->win, 0, 512);
			*(BYTE*)(fs->win+510)=(BYTE)(0xAA55); *((BYTE*)(fs->win+510)+1)=(BYTE)((WORD)(0xAA55)>>8);
			*(BYTE*)(fs->win+0)=(BYTE)(0x41615252); *((BYTE*)(fs->win+0)+1)=(BYTE)((WORD)(0x41615252)>>8); *((BYTE*)(fs->win+0)+2)=(BYTE)((DWORD)(0x41615252)>>16); *((BYTE*)(fs->win+0)+3)=(BYTE)((DWORD)(0x41615252)>>24);
			*(BYTE*)(fs->win+484)=(BYTE)(0x61417272); *((BYTE*)(fs->win+484)+1)=(BYTE)((WORD)(0x61417272)>>8); *((BYTE*)(fs->win+484)+2)=(BYTE)((DWORD)(0x61417272)>>16); *((BYTE*)(fs->win+484)+3)=(BYTE)((DWORD)(0x61417272)>>24);
			*(BYTE*)(fs->win+488)=(BYTE)(fs->free_clust); *((BYTE*)(fs->win+488)+1)=(BYTE)((WORD)(fs->free_clust)>>8); *((BYTE*)(fs->win+488)+2)=(BYTE)((DWORD)(fs->free_clust)>>16); *((BYTE*)(fs->win+488)+3)=(BYTE)((DWORD)(fs->free_clust)>>24);
			*(BYTE*)(fs->win+492)=(BYTE)(fs->last_clust); *((BYTE*)(fs->win+492)+1)=(BYTE)((WORD)(fs->last_clust)>>8); *((BYTE*)(fs->win+492)+2)=(BYTE)((DWORD)(fs->last_clust)>>16); *((BYTE*)(fs->win+492)+3)=(BYTE)((DWORD)(fs->last_clust)>>24);
			/* Write it into the FSInfo sector */
			disk_write(fs->drv, fs->win, fs->fsi_sector, 1);
			fs->fsi_flag = 0;
		}
		/* Make sure that no pending write process in the physical drive */
		if (disk_ioctl(fs->drv, 0, 0) != RES_OK)
			res = FR_DISK_ERR;
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/


DWORD clust2sect (	/* !=0: Sector number, 0: Failed - invalid cluster# */
	FATFS *fs,		/* File system object */
	DWORD clst		/* Cluster# to be converted */
)
{
	clst -= 2;
	if (clst >= (fs->n_fatent - 2)) return 0;		/* Invalid cluster# */
	return clst * fs->csize + fs->database;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/


DWORD get_fat (	/* 0xFFFFFFFF:Disk error, 1:Internal error, Else:Cluster status */
	FATFS *fs,	/* File system object */
	DWORD clst	/* Cluster# to get the link information */
)
{
	UINT wc, bc;
	BYTE *p;


	if (clst < 2 || clst >= fs->n_fatent)	/* Chack range */
		return 1;

	switch (fs->fs_type) {
	case 1 :
		bc = (UINT)clst; bc += bc / 2;
		if (move_window(fs, fs->fatbase + (bc / 512U))) break;
		wc = fs->win[bc % 512U]; bc++;
		if (move_window(fs, fs->fatbase + (bc / 512U))) break;
		wc |= fs->win[bc % 512U] << 8;
		return (clst & 1) ? (wc >> 4) : (wc & 0xFFF);

	case 2 :
		if (move_window(fs, fs->fatbase + (clst / (512U / 2)))) break;
		p = &fs->win[clst * 2 % 512U];
		return (WORD)(((WORD)*((BYTE*)(p)+1)<<8)|(WORD)*(BYTE*)(p));

	case 3 :
		if (move_window(fs, fs->fatbase + (clst / (512U / 4)))) break;
		p = &fs->win[clst * 4 % 512U];
		return (DWORD)(((DWORD)*((BYTE*)(p)+3)<<24)|((DWORD)*((BYTE*)(p)+2)<<16)|((WORD)*((BYTE*)(p)+1)<<8)| *(BYTE*)(p)) & 0x0FFFFFFF;
	}

	return 0xFFFFFFFF;	/* An error occurred at the disk I/O layer */
}




/*-----------------------------------------------------------------------*/
/* FAT access - Change value of a FAT entry                              */
/*-----------------------------------------------------------------------*/

FRESULT put_fat (
	FATFS *fs,	/* File system object */
	DWORD clst,	/* Cluster# to be changed in range of 2 to fs->n_fatent - 1 */
	DWORD val	/* New value to mark the cluster */
)
{
	UINT bc;
	BYTE *p;
	FRESULT res;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		switch (fs->fs_type) {
		case 1 :
			bc = clst; bc += bc / 2;
			res = move_window(fs, fs->fatbase + (bc / 512U));
			if (res != FR_OK) break;
			p = &fs->win[bc % 512U];
			*p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val;
			bc++;
			fs->wflag = 1;
			res = move_window(fs, fs->fatbase + (bc / 512U));
			if (res != FR_OK) break;
			p = &fs->win[bc % 512U];
			*p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F));
			break;

		case 2 :
			res = move_window(fs, fs->fatbase + (clst / (512U / 2)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 2 % 512U];
			*(BYTE*)(p)=(BYTE)((WORD)val); *((BYTE*)(p)+1)=(BYTE)((WORD)((WORD)val)>>8);
			break;

		case 3 :
			res = move_window(fs, fs->fatbase + (clst / (512U / 4)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 4 % 512U];
			val |= (DWORD)(((DWORD)*((BYTE*)(p)+3)<<24)|((DWORD)*((BYTE*)(p)+2)<<16)|((WORD)*((BYTE*)(p)+1)<<8)| *(BYTE*)(p)) & 0xF0000000;
			*(BYTE*)(p)=(BYTE)(val); *((BYTE*)(p)+1)=(BYTE)((WORD)(val)>>8); *((BYTE*)(p)+2)=(BYTE)((DWORD)(val)>>16); *((BYTE*)(p)+3)=(BYTE)((DWORD)(val)>>24);
			break;

		default :
			res = FR_INT_ERR;
		}
		fs->wflag = 1;
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/
static
FRESULT remove_chain (
	FATFS *fs,			/* File system object */
	DWORD clst			/* Cluster# to remove a chain from */
)
{
	FRESULT res;
	DWORD nxt;

	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		res = FR_OK;
		while (clst < fs->n_fatent) {			/* Not a last link? */
			nxt = get_fat(fs, clst);			/* Get cluster status */
			if (nxt == 0) break;				/* Empty cluster? */
			if (nxt == 1) { res = FR_INT_ERR; break; }	/* Internal error? */
			if (nxt == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }	/* Disk error? */
			res = put_fat(fs, clst, 0);			/* Mark the cluster "empty" */
			if (res != FR_OK) break;
			if (fs->free_clust != 0xFFFFFFFF) {	/* Update FSInfo */
				fs->free_clust++;
				fs->fsi_flag = 1;
			}
			clst = nxt;	/* Next cluster */
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch or Create a cluster chain                      */
/*-----------------------------------------------------------------------*/
static
DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
	FATFS *fs,			/* File system object */
	DWORD clst			/* Cluster# to stretch. 0 means create a new chain. */
)
{
	DWORD cs, ncl, scl;
	FRESULT res;


	if (clst == 0) {		/* Create a new chain */
		scl = fs->last_clust;			/* Get suggested start point */
		if (!scl || scl >= fs->n_fatent) scl = 1;
	}
	else {					/* Stretch the current chain */
		cs = get_fat(fs, clst);			/* Check the cluster status */
		if (cs < 2) return 1;			/* It is an invalid cluster */
		if (cs < fs->n_fatent) return cs;	/* It is already followed by next cluster */
		scl = clst;
	}

	ncl = scl;				/* Start cluster */
	for (;;) {
		ncl++;							/* Next cluster */
		if (ncl >= fs->n_fatent) {		/* Wrap around */
			ncl = 2;
			if (ncl > scl) return 0;	/* No free cluster */
		}
		cs = get_fat(fs, ncl);			/* Get the cluster status */
		if (cs == 0) break;				/* Found a free cluster */
		if (cs == 0xFFFFFFFF || cs == 1)/* An error occurred */
			return cs;
		if (ncl == scl) return 0;		/* No free cluster */
	}

	res = put_fat(fs, ncl, 0x0FFFFFFF);	/* Mark the new cluster "last link" */
	if (res == FR_OK && clst != 0) {
		res = put_fat(fs, clst, ncl);	/* Link it to the previous one if needed */
	}
	if (res == FR_OK) {
		fs->last_clust = ncl;			/* Update FSINFO */
		if (fs->free_clust != 0xFFFFFFFF) {
			fs->free_clust--;
			fs->fsi_flag = 1;
		}
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
	}

	return ncl;		/* Return new cluster number or error code */
}



/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_sdi (
	DIR *dj,		/* Pointer to directory object */
	WORD idx		/* Directory index number */
)
{
	DWORD clst;
	WORD ic;


	dj->index = idx;
	clst = dj->sclust;
	if (clst == 1 || clst >= dj->fs->n_fatent)	/* Check start cluster range */
		return FR_INT_ERR;
	if (!clst && dj->fs->fs_type == 3)	/* Replace cluster# 0 with root cluster# if in FAT32 */
		clst = dj->fs->dirbase;

	if (clst == 0) {	/* Static table (root-dir in FAT12/16) */
		dj->clust = clst;
		if (idx >= dj->fs->n_rootdir)		/* Index is out of range */
			return FR_INT_ERR;
		dj->sect = dj->fs->dirbase + idx / (512U / 32);	/* Sector# */
	}
	else {				/* Dynamic table (sub-dirs or root-dir in FAT32) */
		ic = 512U / 32 * dj->fs->csize;	/* Entries per cluster */
		while (idx >= ic) {	/* Follow cluster chain */
			clst = get_fat(dj->fs, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= dj->fs->n_fatent)	/* Reached to end of table or int error */
				return FR_INT_ERR;
			idx -= ic;
		}
		dj->clust = clst;
		dj->sect = clust2sect(dj->fs, clst) + idx / (512U / 32);	/* Sector# */
	}

	dj->dir = dj->fs->win + (idx % (512U / 32)) * 32;	/* Ptr to the entry in the sector */

	return FR_OK;	/* Seek succeeded */
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory index next                        */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_next (	/* FR_OK:Succeeded, FR_NO_FILE:End of table, FR_DENIED:EOT and could not stretch */
	DIR *dj,		/* Pointer to directory object */
	int stretch		/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD clst;
	WORD i;


	stretch = stretch;		/* To suppress warning on read-only cfg. */
	i = dj->index + 1;
	if (!i || !dj->sect)	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;

	if (!(i % (512U / 32))) {	/* Sector changed? */
		dj->sect++;					/* Next sector */

		if (dj->clust == 0) {	/* Static table */
			if (i >= dj->fs->n_rootdir)	/* Report EOT when end of table */
				return FR_NO_FILE;
		}
		else {					/* Dynamic table */
			if (((i / (512U / 32)) & (dj->fs->csize - 1)) == 0) {	/* Cluster changed? */
				clst = get_fat(dj->fs, dj->clust);				/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
				if (clst >= dj->fs->n_fatent) {					/* When it reached end of dynamic table */
					BYTE c;
					if (!stretch) return FR_NO_FILE;			/* When do not stretch, report EOT */
					clst = create_chain(dj->fs, dj->clust);		/* Stretch cluster chain */
					if (clst == 0) return FR_DENIED;			/* No free cluster */
					if (clst == 1) return FR_INT_ERR;
					if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
					/* Clean-up stretched table */
					if (move_window(dj->fs, 0)) return FR_DISK_ERR;	/* Flush active window */
					mem_set(dj->fs->win, 0, 512U);			/* Clear window buffer */
					dj->fs->winsect = clust2sect(dj->fs, clst);	/* Cluster start sector */
					for (c = 0; c < dj->fs->csize; c++) {		/* Fill the new cluster with 0 */
						dj->fs->wflag = 1;
						if (move_window(dj->fs, 0)) return FR_DISK_ERR;
						dj->fs->winsect++;
					}
					dj->fs->winsect -= c;						/* Rewind window address */
				}
				dj->clust = clst;				/* Initialize data for new cluster */
				dj->sect = clust2sect(dj->fs, clst);
			}
		}
	}

	dj->index = i;
	dj->dir = dj->fs->win + (i % (512U / 32)) * 32;

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* LFN handling - Test/Pick/Fit an LFN segment from/to directory entry   */
/*-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Create numbered name                                                  */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/* Calculate sum of an SFN                                               */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_find (
	DIR *dj			/* Pointer to the directory object linked to the file name */
)
{
	FRESULT res;
	BYTE c, *dir;

	res = dir_sdi(dj, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;

	do {
		res = move_window(dj->fs, dj->sect);
		if (res != FR_OK) break;
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[0];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
		if (!(dir[11] & 0x08) && !mem_cmp(dir, dj->fn, 11)) /* Is it a valid entry? */
			break;
		res = dir_next(dj, 0);		/* Next entry */
	} while (res == FR_OK);

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/
static
FRESULT dir_read (
	DIR *dj			/* Pointer to the directory object that pointing the entry to be read */
)
{
	FRESULT res;
	BYTE c, *dir;

	res = FR_NO_FILE;
	while (dj->sect) {
		res = move_window(dj->fs, dj->sect);
		if (res != FR_OK) break;
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[0];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
		if (c != 0xE5 && (0 || c != '.') && !(dir[11] & 0x08))	/* Is it a valid entry? */
			break;
		res = dir_next(dj, 0);				/* Next entry */
		if (res != FR_OK) break;
	}

	if (res != FR_OK) dj->sect = 0;

	return res;
}



/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/
static
FRESULT dir_register (	/* FR_OK:Successful, FR_DENIED:No free entry or too many SFN collision, FR_DISK_ERR:Disk error */
	DIR *dj				/* Target directory with object name to be created */
)
{
	FRESULT res;
	BYTE c, *dir;
	res = dir_sdi(dj, 0);
	if (res == FR_OK) {
		do {	/* Find a blank entry for the SFN */
			res = move_window(dj->fs, dj->sect);
			if (res != FR_OK) break;
			c = *dj->dir;
			if (c == 0xE5 || c == 0) break;	/* Is it a blank entry? */
			res = dir_next(dj, 1);			/* Next entry with table stretch */
		} while (res == FR_OK);
	}

	if (res == FR_OK) {		/* Initialize the SFN entry */
		res = move_window(dj->fs, dj->sect);
		if (res == FR_OK) {
			dir = dj->dir;
			mem_set(dir, 0, 32);	/* Clean the entry */
			mem_cpy(dir, dj->fn, 11);	/* Put SFN */
			dj->fs->wflag = 1;
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/
static
FRESULT dir_remove (	/* FR_OK: Successful, FR_DISK_ERR: A disk error */
	DIR *dj				/* Directory object pointing the entry to be removed */
)
{
	FRESULT res;
	res = dir_sdi(dj, dj->index);
	if (res == FR_OK) {
		res = move_window(dj->fs, dj->sect);
		if (res == FR_OK) {
			*dj->dir = 0xE5;			/* Mark the entry "deleted" */
			dj->fs->wflag = 1;
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/

static
FRESULT create_name (
	DIR *dj,			/* Pointer to the directory object */
	const TCHAR **path	/* Pointer to pointer to the segment in the path string */
)
{
	static const BYTE excvt[] = {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, 0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF};	/* Upper conversion table for extended chars */

	BYTE b, c, d, *sfn;
	UINT ni, si, i;
	const char *p;

	/* Create file name in directory form */
	for (p = *path; *p == '/' || *p == '\\'; p++) ;	/* Strip duplicated separator */
	sfn = dj->fn;
	mem_set(sfn, ' ', 11);
	si = i = b = 0; ni = 8;
	for (;;) {
		c = (BYTE)p[si++];
		if (c <= ' ' || c == '/' || c == '\\') break;	/* Break on end of segment */
		if (c == '.' || i >= ni) {
			if (ni != 8 || c != '.') return FR_INVALID_NAME;
			i = 8; ni = 11;
			b <<= 2; continue;
		}
		if (c >= 0x80) {				/* Extended char? */
			b |= 3;						/* Eliminate NT flag */
			c = excvt[c-0x80];			/* Upper conversion (SBCS) */
		}
		if (0) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			d = (BYTE)p[si++];			/* Get 2nd byte */
			if (!0 || i >= ni - 1)	/* Reject invalid DBC */
				return FR_INVALID_NAME;
			sfn[i++] = c;
			sfn[i++] = d;
		} else {						/* Single byte code */
			if (chk_chr("\"*+,:;<=>\?[]|\x7F", c))	/* Reject illegal chrs for SFN */
				return FR_INVALID_NAME;
			if ((((c)>= 'A')&&((c)<= 'Z'))) {			/* ASCII large capital? */
				b |= 2;
			} else {
				if ((((c)>= 'a')&&((c)<= 'z'))) {		/* ASCII small capital? */
					b |= 1; c -= 0x20;
				}
			}
			sfn[i++] = c;
		}
	}
	*path = &p[si];						/* Return pointer to the next segment */
	c = (c <= ' ') ? 0x04 : 0;		/* Set last segment flag if end of path */

	if (!i) return FR_INVALID_NAME;		/* Reject nul string */
	if (sfn[0] == 0xE5) sfn[0] = 0x05;	/* When first char collides with DDE, replace it with 0x05 */

	if (ni == 8) b <<= 2;
	if ((b & 0x03) == 0x01) c |= 0x10;	/* NT flag (Name extension has only small capital) */
	if ((b & 0x0C) == 0x04) c |= 0x08;	/* NT flag (Name body has only small capital) */

	sfn[11] = c;		/* Store NT flag, File name is created */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/
static
void get_fileinfo (		/* No return code */
	DIR *dj,			/* Pointer to the directory object */
	FILINFO *fno	 	/* Pointer to the file information to be filled */
)
{
	UINT i;
	BYTE nt, *dir;
	TCHAR *p, c;


	p = fno->fname;
	if (dj->sect) {
		dir = dj->dir;
		nt = dir[12];		/* NT flag */
		for (i = 0; i < 8; i++) {	/* Copy name body */
			c = dir[i];
			if (c == ' ') break;
			if (c == 0x05) c = (TCHAR)0xE5;
			if (0 && (nt & 0x08) && (((c)>= 'A')&&((c)<= 'Z'))) c += 0x20;
			*p++ = c;
		}
		if (dir[8] != ' ') {		/* Copy name extension */
			*p++ = '.';
			for (i = 8; i < 11; i++) {
				c = dir[i];
				if (c == ' ') break;
				if (0 && (nt & 0x10) && (((c)>= 'A')&&((c)<= 'Z'))) c += 0x20;
				*p++ = c;
			}
		}
		fno->fattrib = dir[11];				/* Attribute */
		fno->fsize = (DWORD)(((DWORD)*((BYTE*)(dir+28)+3)<<24)|((DWORD)*((BYTE*)(dir+28)+2)<<16)|((WORD)*((BYTE*)(dir+28)+1)<<8)| *(BYTE*)(dir+28));	/* Size */
		fno->fdate = (WORD)(((WORD)*((BYTE*)(dir+24)+1)<<8)|(WORD)*(BYTE*)(dir+24));		/* Date */
		fno->ftime = (WORD)(((WORD)*((BYTE*)(dir+22)+1)<<8)|(WORD)*(BYTE*)(dir+22));		/* Time */
	}
	*p = 0;		/* Terminate SFN str by a \0 */

}




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static
FRESULT follow_path (	/* FR_OK(0): successful, !=0: error code */
	DIR *dj,			/* Directory object to return last directory and found object */
	const TCHAR *path	/* Full-path string to find a file or directory */
)
{
	FRESULT res;
	BYTE *dir, ns;


	if (*path == '/' || *path == '\\')	/* Strip heading separator if exist */
		path++;
	dj->sclust = 0;						/* Start from the root dir */

	if ((UINT)*path < ' ') {			/* Nul path means the start directory itself */
		res = dir_sdi(dj, 0);
		dj->dir = 0;

	} else {							/* Follow path */
		for (;;) {
			res = create_name(dj, &path);	/* Get a segment */
			if (res != FR_OK) break;
			res = dir_find(dj);				/* Find it */
			ns = *(dj->fn+11);
			if (res != FR_OK) {				/* Failed to find the object */
				if (res != FR_NO_FILE) break;	/* Abort if any hard error occured */
				/* Object not found */
				if (0 && (ns & 0x20)) {	/* If dot entry is not exit */
					dj->sclust = 0; dj->dir = 0;	/* It is the root dir */
					res = FR_OK;
					if (!(ns & 0x04)) continue;
				} else {							/* Could not find the object */
					if (!(ns & 0x04)) res = FR_NO_PATH;
				}
				break;
			}
			if (ns & 0x04) break;			/* Last segment match. Function completed. */
			dir = dj->dir;						/* There is next segment. Follow the sub directory */
			if (!(dir[11] & 0x10)) {	/* Cannot follow because it is a file */
				res = FR_NO_PATH; break;
			}
			dj->sclust = (((DWORD)(WORD)(((WORD)*((BYTE*)(dir+20)+1)<<8)|(WORD)*(BYTE*)(dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dir+26)+1)<<8)|(WORD)*(BYTE*)(dir+26)));
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT Volume Boot Record            */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (	/* 0:FAT-VBR, 1:Valid BR but not FAT, 2:Not a BR, 3:Disk error */
	FATFS *fs,	/* File system object */
	DWORD sect	/* Sector# (lba) to check if it is an FAT boot record or not */
)
{
	if (disk_read(fs->drv, fs->win, sect, 1) != RES_OK)	/* Load boot record */
		return 3;
	if ((WORD)(((WORD)*((BYTE*)(&fs->win[510])+1)<<8)|(WORD)*(BYTE*)(&fs->win[510])) != 0xAA55)		/* Check record signature (always placed at offset 510 even if the sector size is >512) */
		return 2;

	if (((DWORD)(((DWORD)*((BYTE*)(&fs->win[54])+3)<<24)|((DWORD)*((BYTE*)(&fs->win[54])+2)<<16)|((WORD)*((BYTE*)(&fs->win[54])+1)<<8)| *(BYTE*)(&fs->win[54])) & 0xFFFFFF) == 0x544146)	/* Check "FAT" string */
		return 0;
	if (((DWORD)(((DWORD)*((BYTE*)(&fs->win[82])+3)<<24)|((DWORD)*((BYTE*)(&fs->win[82])+2)<<16)|((WORD)*((BYTE*)(&fs->win[82])+1)<<8)| *(BYTE*)(&fs->win[82])) & 0xFFFFFF) == 0x544146)
		return 0;

	return 1;
}




/*-----------------------------------------------------------------------*/
/* Check if the file system object is valid or not                       */
/*-----------------------------------------------------------------------*/

static
FRESULT chk_mounted (	/* FR_OK(0): successful, !=0: any error occurred */
	const TCHAR **path,	/* Pointer to pointer to the path name (drive number) */
	FATFS **rfs,		/* Pointer to pointer to the found file system object */
	BYTE chk_wp			/* !=0: Check media write protection for write access */
)
{
	BYTE fmt, b, pi, *tbl;
	UINT vol;
	DSTATUS stat;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat;
	WORD nrsv;
	const TCHAR *p = *path;
	FATFS *fs;

	/* Get logical drive number from the path name */
	vol = p[0] - '0';					/* Is there a drive number? */
	if (vol <= 9 && p[1] == ':') {		/* Found a drive number, get and strip it */
		p += 2; *path = p;				/* Return pointer to the path name */
	} else {							/* No drive number is given */
		vol = 0;						/* Use drive 0 */
	}

	/* Check if the file system object is valid or not */
	if (vol >= 1) 				/* Is the drive number valid? */
		return FR_INVALID_DRIVE;
	*rfs = fs = FatFs[vol];				/* Return pointer to the corresponding file system object */
	if (!fs) return FR_NOT_ENABLED;		/* Is the file system object available? */

	;						/* Lock file system */

	if (fs->fs_type) {					/* If the logical drive has been mounted */
		stat = disk_status(fs->drv);
		if (!(stat & 0x01)) {		/* and the physical drive is kept initialized (has not been changed), */
			if (!0 && chk_wp && (stat & 0x04))	/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
			return FR_OK;				/* The file system object is valid */
		}
	}

	/* The file system object is not valid. */
	/* Following code attempts to mount the volume. (analyze BPB and initialize the fs object) */

	fs->fs_type = 0;					/* Clear the file system object */
	fs->drv = (vol);				/* Bind the logical drive and a physical drive */
	stat = disk_initialize(fs->drv);	/* Initialize low level disk I/O layer */
	if (stat & 0x01)				/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no media or hard error */
	if (!0 && chk_wp && (stat & 0x04))	/* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
	/* Search FAT partition on the drive. Supports only generic partitionings, FDISK and SFD. */
	fmt = check_fs(fs, bsect = 0);		/* Load sector 0 and check if it is an FAT-VBR (in SFD) */
	if (0 && !fmt) fmt = 1;	/* Force non-SFD if the volume is forced partition */
	if (fmt == 1) {						/* Not an FAT-VBR, the physical drive can be partitioned */
		/* Check the partition listed in the partition table */
		pi = 0;
		if (pi) pi--;
		tbl = &fs->win[446 + pi * 16];/* Partition table */
		if (tbl[4]) {						/* Is the partition existing? */
			bsect = (DWORD)(((DWORD)*((BYTE*)(&tbl[8])+3)<<24)|((DWORD)*((BYTE*)(&tbl[8])+2)<<16)|((WORD)*((BYTE*)(&tbl[8])+1)<<8)| *(BYTE*)(&tbl[8]));		/* Partition offset in LBA */
			fmt = check_fs(fs, bsect);		/* Check the partition */
		}
	}
	if (fmt == 3) return FR_DISK_ERR;
	if (fmt) return FR_NO_FILESYSTEM;		/* No FAT volume is found */

	/* An FAT volume is found. Following code initializes the file system object */

	if ((WORD)(((WORD)*((BYTE*)(fs->win+11)+1)<<8)|(WORD)*(BYTE*)(fs->win+11)) != 512U)		/* (BPB_BytsPerSec must be equal to the physical sector size) */
		return FR_NO_FILESYSTEM;

	fasize = (WORD)(((WORD)*((BYTE*)(fs->win+22)+1)<<8)|(WORD)*(BYTE*)(fs->win+22));				/* Number of sectors per FAT */
	if (!fasize) fasize = (DWORD)(((DWORD)*((BYTE*)(fs->win+36)+3)<<24)|((DWORD)*((BYTE*)(fs->win+36)+2)<<16)|((WORD)*((BYTE*)(fs->win+36)+1)<<8)| *(BYTE*)(fs->win+36));
	fs->fsize = fasize;

	fs->n_fats = b = fs->win[16];				/* Number of FAT copies */
	if (b != 1 && b != 2) return FR_NO_FILESYSTEM;		/* (Must be 1 or 2) */
	fasize *= b;										/* Number of sectors for FAT area */

	fs->csize = b = fs->win[13];			/* Number of sectors per cluster */
	if (!b || (b & (b - 1))) return FR_NO_FILESYSTEM;	/* (Must be power of 2) */

	fs->n_rootdir = (WORD)(((WORD)*((BYTE*)(fs->win+17)+1)<<8)|(WORD)*(BYTE*)(fs->win+17));	/* Number of root directory entries */
	if (fs->n_rootdir % (512U / 32)) return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must be sector aligned) */

	tsect = (WORD)(((WORD)*((BYTE*)(fs->win+19)+1)<<8)|(WORD)*(BYTE*)(fs->win+19));				/* Number of sectors on the volume */
	if (!tsect) tsect = (DWORD)(((DWORD)*((BYTE*)(fs->win+32)+3)<<24)|((DWORD)*((BYTE*)(fs->win+32)+2)<<16)|((WORD)*((BYTE*)(fs->win+32)+1)<<8)| *(BYTE*)(fs->win+32));

	nrsv = (WORD)(((WORD)*((BYTE*)(fs->win+14)+1)<<8)|(WORD)*(BYTE*)(fs->win+14));				/* Number of reserved sectors */
	if (!nrsv) return FR_NO_FILESYSTEM;					/* (BPB_RsvdSecCnt must not be 0) */

	/* Determine the FAT sub type */
	sysect = nrsv + fasize + fs->n_rootdir / (512U / 32);	/* RSV+FAT+DIR */
	if (tsect < sysect) return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
	nclst = (tsect - sysect) / fs->csize;				/* Number of clusters */
	if (!nclst) return FR_NO_FILESYSTEM;				/* (Invalid volume size) */
	fmt = 1;
	if (nclst >= 4086) fmt = 2;
	if (nclst >= 65526) fmt = 3;

	/* Boundaries and Limits */
	fs->n_fatent = nclst + 2;							/* Number of FAT entries */
	fs->database = bsect + sysect;						/* Data start sector */
	fs->fatbase = bsect + nrsv; 						/* FAT start sector */
	if (fmt == 3) {
		if (fs->n_rootdir) return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
		fs->dirbase = (DWORD)(((DWORD)*((BYTE*)(fs->win+44)+3)<<24)|((DWORD)*((BYTE*)(fs->win+44)+2)<<16)|((WORD)*((BYTE*)(fs->win+44)+1)<<8)| *(BYTE*)(fs->win+44));	/* Root directory start cluster */
		szbfat = fs->n_fatent * 4;						/* (Required FAT size) */
	} else {
		if (!fs->n_rootdir)	return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
		fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
		szbfat = (fmt == 2) ?					/* (Required FAT size) */
			fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
	}
	if (fs->fsize < (szbfat + (512U - 1)) / 512U)	/* (BPB_FATSz must not be less than required) */
		return FR_NO_FILESYSTEM;

	/* Initialize cluster allocation information */
	fs->free_clust = 0xFFFFFFFF;
	fs->last_clust = 0;

	/* Get fsinfo if available */
	if (fmt == 3) {
	 	fs->fsi_flag = 0;
		fs->fsi_sector = bsect + (WORD)(((WORD)*((BYTE*)(fs->win+48)+1)<<8)|(WORD)*(BYTE*)(fs->win+48));
		if (disk_read(fs->drv, fs->win, fs->fsi_sector, 1) == RES_OK &&
			(WORD)(((WORD)*((BYTE*)(fs->win+510)+1)<<8)|(WORD)*(BYTE*)(fs->win+510)) == 0xAA55 &&
			(DWORD)(((DWORD)*((BYTE*)(fs->win+0)+3)<<24)|((DWORD)*((BYTE*)(fs->win+0)+2)<<16)|((WORD)*((BYTE*)(fs->win+0)+1)<<8)| *(BYTE*)(fs->win+0)) == 0x41615252 &&
			(DWORD)(((DWORD)*((BYTE*)(fs->win+484)+3)<<24)|((DWORD)*((BYTE*)(fs->win+484)+2)<<16)|((WORD)*((BYTE*)(fs->win+484)+1)<<8)| *(BYTE*)(fs->win+484)) == 0x61417272) {
				fs->last_clust = (DWORD)(((DWORD)*((BYTE*)(fs->win+492)+3)<<24)|((DWORD)*((BYTE*)(fs->win+492)+2)<<16)|((WORD)*((BYTE*)(fs->win+492)+1)<<8)| *(BYTE*)(fs->win+492));
				fs->free_clust = (DWORD)(((DWORD)*((BYTE*)(fs->win+488)+3)<<24)|((DWORD)*((BYTE*)(fs->win+488)+2)<<16)|((WORD)*((BYTE*)(fs->win+488)+1)<<8)| *(BYTE*)(fs->win+488));
		}
	}
	fs->fs_type = fmt;		/* FAT sub-type */
	fs->id = ++Fsid;		/* File system mount ID */
	fs->winsect = 0;		/* Invalidate sector cache */
	fs->wflag = 0;

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/dir object is valid or not                          */
/*-----------------------------------------------------------------------*/

static
FRESULT validate (	/* FR_OK(0): The object is valid, !=0: Invalid */
	FATFS *fs,		/* Pointer to the file system object */
	WORD id			/* Member id of the target object to be checked */
)
{
	if (!fs || !fs->fs_type || fs->id != id)
		return FR_INVALID_OBJECT;

	;		/* Lock file system */

	if (disk_status(fs->drv) & 0x01)
		return FR_NOT_READY;

	return FR_OK;
}




/*--------------------------------------------------------------------------

   Public Functions

--------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	BYTE vol,		/* Logical drive number to be mounted/unmounted */
	FATFS *fs		/* Pointer to new file system object (NULL for unmount)*/
)
{
	FATFS *rfs;


	if (vol >= 1)		/* Check if the drive number is valid */
		return FR_INVALID_DRIVE;
	rfs = FatFs[vol];			/* Get current fs object */

	if (rfs) {
		rfs->fs_type = 0;		/* Clear old fs object */
	}

	if (fs) {
		fs->fs_type = 0;		/* Clear new fs object */
	}
	FatFs[vol] = fs;			/* Register new fs object */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL *fp,			/* Pointer to the blank file object */
	const TCHAR *path,	/* Pointer to the file name */
	BYTE mode			/* Access mode and file open mode flags */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	BYTE sfn[12];


	fp->fs = 0;			/* Clear file object */

	mode &= 0x01 | 0x02 | 0x08 | 0x10 | 0x04;
	res = chk_mounted(&path, &dj.fs, (BYTE)(mode & ~0x01));
	(dj). fn = sfn;
	if (res == FR_OK)
		res = follow_path(&dj, path);	/* Follow the file path */
	dir = dj.dir;

	if (res == FR_OK) {
		if (!dir)	/* Current dir itself */
			res = FR_INVALID_NAME;
	}
	/* Create or Open a file */
	if (mode & (0x08 | 0x10 | 0x04)) {
		DWORD dw, cl;

		if (res != FR_OK) {					/* No file, create new */
			if (res == FR_NO_FILE)			/* There is no file to open, create a new entry */
				res = dir_register(&dj);
			mode |= 0x08;		/* File is created */
			dir = dj.dir;					/* New entry */
		}
		else {								/* Any object is already existing */
			if (dir[11] & (0x01 | 0x10)) {	/* Cannot overwrite it (R/O or DIR) */
				res = FR_DENIED;
			} else {
				if (mode & 0x04)	/* Cannot create as new file */
					res = FR_EXIST;
			}
		}
		if (res == FR_OK && (mode & 0x08)) {	/* Truncate it if overwrite mode */
			dw = get_fattime();					/* Created time */
			*(BYTE*)(dir+14)=(BYTE)(dw); *((BYTE*)(dir+14)+1)=(BYTE)((WORD)(dw)>>8); *((BYTE*)(dir+14)+2)=(BYTE)((DWORD)(dw)>>16); *((BYTE*)(dir+14)+3)=(BYTE)((DWORD)(dw)>>24);
			dir[11] = 0;					/* Reset attribute */
			*(BYTE*)(dir+28)=(BYTE)(0); *((BYTE*)(dir+28)+1)=(BYTE)((WORD)(0)>>8); *((BYTE*)(dir+28)+2)=(BYTE)((DWORD)(0)>>16); *((BYTE*)(dir+28)+3)=(BYTE)((DWORD)(0)>>24);		/* size = 0 */
			cl = (((DWORD)(WORD)(((WORD)*((BYTE*)(dir+20)+1)<<8)|(WORD)*(BYTE*)(dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dir+26)+1)<<8)|(WORD)*(BYTE*)(dir+26)));					/* Get start cluster */
			{*(BYTE*)(dir+26)=(BYTE)(0); *((BYTE*)(dir+26)+1)=(BYTE)((WORD)(0)>>8); *(BYTE*)(dir+20)=(BYTE)((DWORD)0>>16); *((BYTE*)(dir+20)+1)=(BYTE)((WORD)((DWORD)0>>16)>>8);};					/* cluster = 0 */
			dj.fs->wflag = 1;
			if (cl) {							/* Remove the cluster chain if exist */
				dw = dj.fs->winsect;
				res = remove_chain(dj.fs, cl);
				if (res == FR_OK) {
					dj.fs->last_clust = cl - 1;	/* Reuse the cluster hole */
					res = move_window(dj.fs, dw);
				}
			}
		}
	}
	else {	/* Open an existing file */
		if (res == FR_OK) {						/* Follow succeeded */
			if (dir[11] & 0x10) {		/* It is a directory */
				res = FR_NO_FILE;
			} else {
				if ((mode & 0x02) && (dir[11] & 0x01)) /* R/O violation */
					res = FR_DENIED;
			}
		}
	}
	if (res == FR_OK) {
		if (mode & 0x08)			/* Set file change flag if created or overwritten */
			mode |= 0x20;
		fp->dir_sect = dj.fs->winsect;			/* Pointer to the directory entry */
		fp->dir_ptr = dir;
	}

	;

	if (res == FR_OK) {
		fp->flag = mode;					/* File access mode */
		fp->sclust = (((DWORD)(WORD)(((WORD)*((BYTE*)(dir+20)+1)<<8)|(WORD)*(BYTE*)(dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dir+26)+1)<<8)|(WORD)*(BYTE*)(dir+26)));			/* File start cluster */
		fp->fsize = (DWORD)(((DWORD)*((BYTE*)(dir+28)+3)<<24)|((DWORD)*((BYTE*)(dir+28)+2)<<16)|((WORD)*((BYTE*)(dir+28)+1)<<8)| *(BYTE*)(dir+28));	/* File size */
		fp->fptr = 0;						/* File pointer */
		fp->dsect = 0;
		fp->fs = dj.fs; fp->id = dj.fs->id;	/* Validate file object */
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL *fp, 		/* Pointer to the file object */
	void *buff,		/* Pointer to data buffer */
	UINT btr,		/* Number of bytes to read */
	UINT *br		/* Pointer to number of bytes read */
)
{
	FRESULT res;
	DWORD clst, sect, remain;
	UINT rcnt, cc;
	BYTE csect, *rbuff = buff;


	*br = 0;	/* Initialize byte counter */

	res = validate(fp->fs, fp->id);				/* Check validity */
	if (res != FR_OK) return res;
	if (fp->flag & 0x80)					/* Aborted file? */
		return FR_INT_ERR;
	if (!(fp->flag & 0x01)) 					/* Check access mode */
		return FR_DENIED;
	remain = fp->fsize - fp->fptr;
	if (btr > remain) btr = (UINT)remain;		/* Truncate btr by remaining bytes */

	for ( ;  btr;								/* Repeat until all data read */
		rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt) {
		if ((fp->fptr % 512U) == 0) {		/* On the sector boundary? */
			csect = (BYTE)(fp->fptr / 512U & (fp->fs->csize - 1));	/* Sector offset in the cluster */
			if (!csect) {						/* On the cluster boundary? */
				if (fp->fptr == 0) {			/* On the top of the file? */
					clst = fp->sclust;			/* Follow from the origin */
				} else {						/* Middle or end of the file */
						clst = get_fat(fp->fs, fp->clust);	/* Follow cluster chain on the FAT */
				}
				if (clst < 2) { fp->flag |= 0x80; return FR_INT_ERR; };
				if (clst == 0xFFFFFFFF) { fp->flag |= 0x80; return FR_DISK_ERR; };
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if (!sect) { fp->flag |= 0x80; return FR_INT_ERR; };
			sect += csect;
			cc = btr / 512U;				/* When remaining bytes >= sector size, */
			if (cc) {							/* Read maximum contiguous sectors directly */
				if (csect + cc > fp->fs->csize)	/* Clip at cluster boundary */
					cc = fp->fs->csize - csect;
				if (disk_read(fp->fs->drv, rbuff, sect, (BYTE)cc) != RES_OK)
					{ fp->flag |= 0x80; return FR_DISK_ERR; };
				if ((fp->flag & 0x40) && fp->dsect - sect < cc)
					mem_cpy(rbuff + ((fp->dsect - sect) * 512U), fp->buf, 512U);
				rcnt = 512U * cc;			/* Number of bytes transferred */
				continue;
			}
			if (fp->dsect != sect) {			/* Load data sector if not in cache */
				if (fp->flag & 0x40) {		/* Write-back dirty sector cache */
					if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
						{ fp->flag |= 0x80; return FR_DISK_ERR; };
					fp->flag &= ~0x40;
				}
				if (disk_read(fp->fs->drv, fp->buf, sect, 1) != RES_OK)	/* Fill sector cache */
					{ fp->flag |= 0x80; return FR_DISK_ERR; };
			}
			fp->dsect = sect;
		}
		rcnt = 512U - (fp->fptr % 512U);	/* Get partial sector data from sector buffer */
		if (rcnt > btr) rcnt = btr;
		mem_cpy(rbuff, &fp->buf[fp->fptr % 512U], rcnt);	/* Pick partial sector */
	}

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
	FIL *fp,			/* Pointer to the file object */
	const void *buff,	/* Pointer to the data to be written */
	UINT btw,			/* Number of bytes to write */
	UINT *bw			/* Pointer to number of bytes written */
)
{
	FRESULT res;
	DWORD clst, sect;
	UINT wcnt, cc;
	const BYTE *wbuff = buff;
	BYTE csect;


	*bw = 0;	/* Initialize byte counter */

	res = validate(fp->fs, fp->id);			/* Check validity */
	if (res != FR_OK) return res;
	if (fp->flag & 0x80)				/* Aborted file? */
		return FR_INT_ERR;
	if (!(fp->flag & 0x02))				/* Check access mode */
		return FR_DENIED;
	if ((DWORD)(fp->fsize + btw) < fp->fsize) btw = 0;	/* File size cannot reach 4GB */

	for ( ;  btw;							/* Repeat until all data written */
		wbuff += wcnt, fp->fptr += wcnt, *bw += wcnt, btw -= wcnt) {
		if ((fp->fptr % 512U) == 0) {	/* On the sector boundary? */
			csect = (BYTE)(fp->fptr / 512U & (fp->fs->csize - 1));	/* Sector offset in the cluster */
			if (!csect) {					/* On the cluster boundary? */
				if (fp->fptr == 0) {		/* On the top of the file? */
					clst = fp->sclust;		/* Follow from the origin */
					if (clst == 0)			/* When no cluster is allocated, */
						fp->sclust = clst = create_chain(fp->fs, 0);	/* Create a new cluster chain */
				} else {					/* Middle or end of the file */
						clst = create_chain(fp->fs, fp->clust);	/* Follow or stretch cluster chain on the FAT */
				}
				if (clst == 0) break;		/* Could not allocate a new cluster (disk full) */
				if (clst == 1) { fp->flag |= 0x80; return FR_INT_ERR; };
				if (clst == 0xFFFFFFFF) { fp->flag |= 0x80; return FR_DISK_ERR; };
				fp->clust = clst;			/* Update current cluster */
			}
			if (fp->flag & 0x40) {		/* Write-back sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					{ fp->flag |= 0x80; return FR_DISK_ERR; };
				fp->flag &= ~0x40;
			}
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if (!sect) { fp->flag |= 0x80; return FR_INT_ERR; };
			sect += csect;
			cc = btw / 512U;			/* When remaining bytes >= sector size, */
			if (cc) {						/* Write maximum contiguous sectors directly */
				if (csect + cc > fp->fs->csize)	/* Clip at cluster boundary */
					cc = fp->fs->csize - csect;
				if (disk_write(fp->fs->drv, wbuff, sect, (BYTE)cc) != RES_OK)
					{ fp->flag |= 0x80; return FR_DISK_ERR; };
				if (fp->dsect - sect < cc) { /* Refill sector cache if it gets invalidated by the direct write */
					mem_cpy(fp->buf, wbuff + ((fp->dsect - sect) * 512U), 512U);
					fp->flag &= ~0x40;
				}
				wcnt = 512U * cc;		/* Number of bytes transferred */
				continue;
			}
			if (fp->dsect != sect) {		/* Fill sector cache with file data */
				if (fp->fptr < fp->fsize &&
					disk_read(fp->fs->drv, fp->buf, sect, 1) != RES_OK)
						{ fp->flag |= 0x80; return FR_DISK_ERR; };
			}
			fp->dsect = sect;
		}
		wcnt = 512U - (fp->fptr % 512U);/* Put partial sector into file I/O buffer */
		if (wcnt > btw) wcnt = btw;
		mem_cpy(&fp->buf[fp->fptr % 512U], wbuff, wcnt);	/* Fit partial sector */
		fp->flag |= 0x40;
	}

	if (fp->fptr > fp->fsize) fp->fsize = fp->fptr;	/* Update file size if needed */
	fp->flag |= 0x20;						/* Set file change flag */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Synchronize the File Object                                           */
/*-----------------------------------------------------------------------*/

FRESULT f_sync (
	FIL *fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD tim;
	BYTE *dir;


	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	if (res == FR_OK) {
		if (fp->flag & 0x20) {	/* Has the file been written? */
			if (fp->flag & 0x40) {
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					return FR_DISK_ERR;
				fp->flag &= ~0x40;
			}
			/* Update the directory entry */
			res = move_window(fp->fs, fp->dir_sect);
			if (res == FR_OK) {
				dir = fp->dir_ptr;
				dir[11] |= 0x20;					/* Set archive bit */
				*(BYTE*)(dir+28)=(BYTE)(fp->fsize); *((BYTE*)(dir+28)+1)=(BYTE)((WORD)(fp->fsize)>>8); *((BYTE*)(dir+28)+2)=(BYTE)((DWORD)(fp->fsize)>>16); *((BYTE*)(dir+28)+3)=(BYTE)((DWORD)(fp->fsize)>>24);		/* Update file size */
				{*(BYTE*)(dir+26)=(BYTE)(fp->sclust); *((BYTE*)(dir+26)+1)=(BYTE)((WORD)(fp->sclust)>>8); *(BYTE*)(dir+20)=(BYTE)((DWORD)fp->sclust>>16); *((BYTE*)(dir+20)+1)=(BYTE)((WORD)((DWORD)fp->sclust>>16)>>8);};					/* Update start cluster */
				tim = get_fattime();						/* Update updated time */
				*(BYTE*)(dir+22)=(BYTE)(tim); *((BYTE*)(dir+22)+1)=(BYTE)((WORD)(tim)>>8); *((BYTE*)(dir+22)+2)=(BYTE)((DWORD)(tim)>>16); *((BYTE*)(dir+22)+3)=(BYTE)((DWORD)(tim)>>24);
				fp->flag &= ~0x20;
				fp->fs->wflag = 1;
				res = sync(fp->fs);
			}
		}
	}

	return res;
}





/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL *fp		/* Pointer to the file object to be closed */
)
{
	FRESULT res;

	res = f_sync(fp);		/* Flush cached data */
	if (res == FR_OK) fp->fs = 0;	/* Discard file object */
	return res;
}




/*-----------------------------------------------------------------------*/
/* Current Drive/Directory Handlings                                     */
/*-----------------------------------------------------------------------*/




/*-----------------------------------------------------------------------*/
/* Seek File R/W Pointer                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_lseek (
	FIL *fp,		/* Pointer to the file object */
	DWORD ofs		/* File pointer from top of file */
)
{
	FRESULT res;


	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	if (res != FR_OK) return res;
	if (fp->flag & 0x80)			/* Check abort flag */
		return FR_INT_ERR;


	/* Normal Seek */
	{
		DWORD clst, bcs, nsect, ifptr;

		if (ofs > fp->fsize					/* In read-only mode, clip offset with the file size */
			 && !(fp->flag & 0x02)
			) ofs = fp->fsize;

		ifptr = fp->fptr;
		fp->fptr = nsect = 0;
		if (ofs) {
			bcs = (DWORD)fp->fs->csize * 512U;	/* Cluster size (byte) */
			if (ifptr > 0 &&
				(ofs - 1) / bcs >= (ifptr - 1) / bcs) {	/* When seek to same or following cluster, */
				fp->fptr = (ifptr - 1) & ~(bcs - 1);	/* start from the current cluster */
				ofs -= fp->fptr;
				clst = fp->clust;
			} else {									/* When seek to back cluster, */
				clst = fp->sclust;						/* start from the first cluster */
				if (clst == 0) {						/* If no cluster chain, create a new chain */
					clst = create_chain(fp->fs, 0);
					if (clst == 1) { fp->flag |= 0x80; return FR_INT_ERR; };
					if (clst == 0xFFFFFFFF) { fp->flag |= 0x80; return FR_DISK_ERR; };
					fp->sclust = clst;
				}
				fp->clust = clst;
			}
			if (clst != 0) {
				while (ofs > bcs) {						/* Cluster following loop */
					if (fp->flag & 0x02) {			/* Check if in write mode or not */
						clst = create_chain(fp->fs, clst);	/* Force stretch if in write mode */
						if (clst == 0) {				/* When disk gets full, clip file size */
							ofs = bcs; break;
						}
					} else
						clst = get_fat(fp->fs, clst);	/* Follow cluster chain if not in write mode */
					if (clst == 0xFFFFFFFF) { fp->flag |= 0x80; return FR_DISK_ERR; };
					if (clst <= 1 || clst >= fp->fs->n_fatent) { fp->flag |= 0x80; return FR_INT_ERR; };
					fp->clust = clst;
					fp->fptr += bcs;
					ofs -= bcs;
				}
				fp->fptr += ofs;
				if (ofs % 512U) {
					nsect = clust2sect(fp->fs, clst);	/* Current sector */
					if (!nsect) { fp->flag |= 0x80; return FR_INT_ERR; };
					nsect += ofs / 512U;
				}
			}
		}
		if (fp->fptr % 512U && nsect != fp->dsect) {	/* Fill sector cache if needed */
			if (fp->flag & 0x40) {			/* Write-back dirty sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					{ fp->flag |= 0x80; return FR_DISK_ERR; };
				fp->flag &= ~0x40;
			}
			if (disk_read(fp->fs->drv, fp->buf, nsect, 1) != RES_OK)	/* Fill sector cache */
				{ fp->flag |= 0x80; return FR_DISK_ERR; };
			fp->dsect = nsect;
		}
		if (fp->fptr > fp->fsize) {			/* Set file change flag if the file size is extended */
			fp->fsize = fp->fptr;
			fp->flag |= 0x20;
		}
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Create a Directroy Object                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
	DIR *dj,			/* Pointer to directory object to create */
	const TCHAR *path	/* Pointer to the directory path */
)
{
	FRESULT res;
	BYTE sfn[12];


	res = chk_mounted(&path, &dj->fs, 0);
	if (res == FR_OK) {
		(*dj). fn = sfn;
		res = follow_path(dj, path);			/* Follow the path to the directory */
		;
		if (res == FR_OK) {						/* Follow completed */
			if (dj->dir) {						/* It is not the root dir */
				if (dj->dir[11] & 0x10) {	/* The object is a directory */
					dj->sclust = (((DWORD)(WORD)(((WORD)*((BYTE*)(dj->dir+20)+1)<<8)|(WORD)*(BYTE*)(dj->dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dj->dir+26)+1)<<8)|(WORD)*(BYTE*)(dj->dir+26)));
				} else {						/* The object is not a directory */
					res = FR_NO_PATH;
				}
			}
			if (res == FR_OK) {
				dj->id = dj->fs->id;
				res = dir_sdi(dj, 0);			/* Rewind dir */
			}
		}
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entry in Sequense                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
	DIR *dj,			/* Pointer to the open directory object */
	FILINFO *fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	BYTE sfn[12];


	res = validate(dj->fs, dj->id);			/* Check validity of the object */
	if (res == FR_OK) {
		if (!fno) {
			res = dir_sdi(dj, 0);			/* Rewind the directory object */
		} else {
			(*dj). fn = sfn;
			res = dir_read(dj);				/* Read an directory item */
			if (res == FR_NO_FILE) {		/* Reached end of dir */
				dj->sect = 0;
				res = FR_OK;
			}
			if (res == FR_OK) {				/* A valid entry is found */
				get_fileinfo(dj, fno);		/* Get the object information */
				res = dir_next(dj, 0);		/* Increment index for next */
				if (res == FR_NO_FILE) {
					dj->sect = 0;
					res = FR_OK;
				}
			}
			;
		}
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Get File Status                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_stat (
	const TCHAR *path,	/* Pointer to the file path */
	FILINFO *fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DIR dj;
	BYTE sfn[12];


	res = chk_mounted(&path, &dj.fs, 0);
	if (res == FR_OK) {
		(dj). fn = sfn;
		res = follow_path(&dj, path);	/* Follow the file path */
		if (res == FR_OK) {				/* Follow completed */
			if (dj.dir)		/* Found an object */
				get_fileinfo(&dj, fno);
			else			/* It is root dir */
				res = FR_INVALID_NAME;
		}
		;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Get Number of Free Clusters                                           */
/*-----------------------------------------------------------------------*/

FRESULT f_getfree (
	const TCHAR *path,	/* Pointer to the logical drive number (root dir) */
	DWORD *nclst,		/* Pointer to the variable to return number of free clusters */
	FATFS **fatfs		/* Pointer to pointer to corresponding file system object to return */
)
{
	FRESULT res;
	DWORD n, clst, sect, stat;
	UINT i;
	BYTE fat, *p;


	/* Get drive number */
	res = chk_mounted(&path, fatfs, 0);
	if (res == FR_OK) {
		/* If free_clust is valid, return it without full cluster scan */
		if ((*fatfs)->free_clust <= (*fatfs)->n_fatent - 2) {
			*nclst = (*fatfs)->free_clust;
		} else {
			/* Get number of free clusters */
			fat = (*fatfs)->fs_type;
			n = 0;
			if (fat == 1) {
				clst = 2;
				do {
					stat = get_fat(*fatfs, clst);
					if (stat == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }
					if (stat == 1) { res = FR_INT_ERR; break; }
					if (stat == 0) n++;
				} while (++clst < (*fatfs)->n_fatent);
			} else {
				clst = (*fatfs)->n_fatent;
				sect = (*fatfs)->fatbase;
				i = 0; p = 0;
				do {
					if (!i) {
						res = move_window(*fatfs, sect++);
						if (res != FR_OK) break;
						p = (*fatfs)->win;
						i = 512U;
					}
					if (fat == 2) {
						if ((WORD)(((WORD)*((BYTE*)(p)+1)<<8)|(WORD)*(BYTE*)(p)) == 0) n++;
						p += 2; i -= 2;
					} else {
						if (((DWORD)(((DWORD)*((BYTE*)(p)+3)<<24)|((DWORD)*((BYTE*)(p)+2)<<16)|((WORD)*((BYTE*)(p)+1)<<8)| *(BYTE*)(p)) & 0x0FFFFFFF) == 0) n++;
						p += 4; i -= 4;
					}
				} while (--clst);
			}
			(*fatfs)->free_clust = n;
			if (fat == 3) (*fatfs)->fsi_flag = 1;
			*nclst = n;
		}
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Truncate File                                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_truncate (
	FIL *fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD ncl;


	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	if (res == FR_OK) {
		if (fp->flag & 0x80) {			/* Check abort flag */
			res = FR_INT_ERR;
		} else {
			if (!(fp->flag & 0x02))		/* Check access mode */
				res = FR_DENIED;
		}
	}
	if (res == FR_OK) {
		if (fp->fsize > fp->fptr) {
			fp->fsize = fp->fptr;	/* Set file size to current R/W point */
			fp->flag |= 0x20;
			if (fp->fptr == 0) {	/* When set file size to zero, remove entire cluster chain */
				res = remove_chain(fp->fs, fp->sclust);
				fp->sclust = 0;
			} else {				/* When truncate a part of the file, remove remaining clusters */
				ncl = get_fat(fp->fs, fp->clust);
				res = FR_OK;
				if (ncl == 0xFFFFFFFF) res = FR_DISK_ERR;
				if (ncl == 1) res = FR_INT_ERR;
				if (res == FR_OK && ncl < fp->fs->n_fatent) {
					res = put_fat(fp->fs, fp->clust, 0x0FFFFFFF);
					if (res == FR_OK) res = remove_chain(fp->fs, ncl);
				}
			}
		}
		if (res != FR_OK) fp->flag |= 0x80;
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
	const TCHAR *path		/* Pointer to the file or directory path */
)
{
	FRESULT res;
	DIR dj, sdj;
	BYTE *dir;
	DWORD dclst;
	BYTE sfn[12];


	res = chk_mounted(&path, &dj.fs, 1);
	if (res == FR_OK) {
		(dj). fn = sfn;
		res = follow_path(&dj, path);		/* Follow the file path */
		if (0 && res == FR_OK && (dj.fn[11] & 0x20))
			res = FR_INVALID_NAME;			/* Cannot remove dot entry */
		if (res == FR_OK) {					/* The object is accessible */
			dir = dj.dir;
			if (!dir) {
				res = FR_INVALID_NAME;		/* Cannot remove the start directory */
			} else {
				if (dir[11] & 0x01)
					res = FR_DENIED;		/* Cannot remove R/O object */
			}
			dclst = (((DWORD)(WORD)(((WORD)*((BYTE*)(dir+20)+1)<<8)|(WORD)*(BYTE*)(dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dir+26)+1)<<8)|(WORD)*(BYTE*)(dir+26)));
			if (res == FR_OK && (dir[11] & 0x10)) {	/* Is it a sub-dir? */
				if (dclst < 2) {
					res = FR_INT_ERR;
				} else {
					mem_cpy(&sdj, &dj, sizeof(DIR));	/* Check if the sub-dir is empty or not */
					sdj.sclust = dclst;
					res = dir_sdi(&sdj, 2);		/* Exclude dot entries */
					if (res == FR_OK) {
						res = dir_read(&sdj);
						if (res == FR_OK			/* Not empty dir */
						) res = FR_DENIED;
						if (res == FR_NO_FILE) res = FR_OK;	/* Empty */
					}
				}
			}
			if (res == FR_OK) {
				res = dir_remove(&dj);		/* Remove the directory entry */
				if (res == FR_OK) {
					if (dclst)				/* Remove the cluster chain if exist */
						res = remove_chain(dj.fs, dclst);
					if (res == FR_OK) res = sync(dj.fs);
				}
			}
		}
		;
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Create a Directory                                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_mkdir (
	const TCHAR *path		/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir, n;
	DWORD dsc, dcl, pcl, tim = get_fattime();
	BYTE sfn[12];


	res = chk_mounted(&path, &dj.fs, 1);
	if (res == FR_OK) {
		(dj). fn = sfn;
		res = follow_path(&dj, path);			/* Follow the file path */
		if (res == FR_OK) res = FR_EXIST;		/* Any object with same name is already existing */
		if (0 && res == FR_NO_FILE && (dj.fn[11] & 0x20))
			res = FR_INVALID_NAME;
		if (res == FR_NO_FILE) {				/* Can create a new directory */
			dcl = create_chain(dj.fs, 0);		/* Allocate a cluster for the new directory table */
			res = FR_OK;
			if (dcl == 0) res = FR_DENIED;		/* No space to allocate a new cluster */
			if (dcl == 1) res = FR_INT_ERR;
			if (dcl == 0xFFFFFFFF) res = FR_DISK_ERR;
			if (res == FR_OK)					/* Flush FAT */
				res = move_window(dj.fs, 0);
			if (res == FR_OK) {					/* Initialize the new directory table */
				dsc = clust2sect(dj.fs, dcl);
				dir = dj.fs->win;
				mem_set(dir, 0, 512U);
				mem_set(dir+0, ' ', 8+3);	/* Create "." entry */
				dir[0] = '.';
				dir[11] = 0x10;
				*(BYTE*)(dir+22)=(BYTE)(tim); *((BYTE*)(dir+22)+1)=(BYTE)((WORD)(tim)>>8); *((BYTE*)(dir+22)+2)=(BYTE)((DWORD)(tim)>>16); *((BYTE*)(dir+22)+3)=(BYTE)((DWORD)(tim)>>24);
				{*(BYTE*)(dir+26)=(BYTE)(dcl); *((BYTE*)(dir+26)+1)=(BYTE)((WORD)(dcl)>>8); *(BYTE*)(dir+20)=(BYTE)((DWORD)dcl>>16); *((BYTE*)(dir+20)+1)=(BYTE)((WORD)((DWORD)dcl>>16)>>8);};
				mem_cpy(dir+32, dir, 32); 	/* Create ".." entry */
				dir[33] = '.'; pcl = dj.sclust;
				if (dj.fs->fs_type == 3 && pcl == dj.fs->dirbase)
					pcl = 0;
				{*(BYTE*)(dir+32+26)=(BYTE)(pcl); *((BYTE*)(dir+32+26)+1)=(BYTE)((WORD)(pcl)>>8); *(BYTE*)(dir+32+20)=(BYTE)((DWORD)pcl>>16); *((BYTE*)(dir+32+20)+1)=(BYTE)((WORD)((DWORD)pcl>>16)>>8);};
				for (n = dj.fs->csize; n; n--) {	/* Write dot entries and clear following sectors */
					dj.fs->winsect = dsc++;
					dj.fs->wflag = 1;
					res = move_window(dj.fs, 0);
					if (res != FR_OK) break;
					mem_set(dir, 0, 512U);
				}
			}
			if (res == FR_OK) res = dir_register(&dj);	/* Register the object to the directoy */
			if (res != FR_OK) {
				remove_chain(dj.fs, dcl);			/* Could not register, remove cluster chain */
			} else {
				dir = dj.dir;
				dir[11] = 0x10;				/* Attribute */
				*(BYTE*)(dir+22)=(BYTE)(tim); *((BYTE*)(dir+22)+1)=(BYTE)((WORD)(tim)>>8); *((BYTE*)(dir+22)+2)=(BYTE)((DWORD)(tim)>>16); *((BYTE*)(dir+22)+3)=(BYTE)((DWORD)(tim)>>24);		/* Created time */
				{*(BYTE*)(dir+26)=(BYTE)(dcl); *((BYTE*)(dir+26)+1)=(BYTE)((WORD)(dcl)>>8); *(BYTE*)(dir+20)=(BYTE)((DWORD)dcl>>16); *((BYTE*)(dir+20)+1)=(BYTE)((WORD)((DWORD)dcl>>16)>>8);};					/* Table start cluster */
				dj.fs->wflag = 1;
				res = sync(dj.fs);
			}
		}
		;
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Change Attribute                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_chmod (
	const TCHAR *path,	/* Pointer to the file path */
	BYTE value,			/* Attribute bits */
	BYTE mask			/* Attribute mask to change */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	BYTE sfn[12];


	res = chk_mounted(&path, &dj.fs, 1);
	if (res == FR_OK) {
		(dj). fn = sfn;
		res = follow_path(&dj, path);		/* Follow the file path */
		;
		if (0 && res == FR_OK && (dj.fn[11] & 0x20))
			res = FR_INVALID_NAME;
		if (res == FR_OK) {
			dir = dj.dir;
			if (!dir) {						/* Is it a root directory? */
				res = FR_INVALID_NAME;
			} else {						/* File or sub directory */
				mask &= 0x01|0x02|0x04|0x20;	/* Valid attribute mask */
				dir[11] = (value & mask) | (dir[11] & (BYTE)~mask);	/* Apply attribute change */
				dj.fs->wflag = 1;
				res = sync(dj.fs);
			}
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Change Timestamp                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_utime (
	const TCHAR *path,	/* Pointer to the file/directory name */
	const FILINFO *fno	/* Pointer to the time stamp to be set */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	BYTE sfn[12];


	res = chk_mounted(&path, &dj.fs, 1);
	if (res == FR_OK) {
		(dj). fn = sfn;
		res = follow_path(&dj, path);	/* Follow the file path */
		;
		if (0 && res == FR_OK && (dj.fn[11] & 0x20))
			res = FR_INVALID_NAME;
		if (res == FR_OK) {
			dir = dj.dir;
			if (!dir) {					/* Root directory */
				res = FR_INVALID_NAME;
			} else {					/* File or sub-directory */
				*(BYTE*)(dir+22)=(BYTE)(fno->ftime); *((BYTE*)(dir+22)+1)=(BYTE)((WORD)(fno->ftime)>>8);
				*(BYTE*)(dir+24)=(BYTE)(fno->fdate); *((BYTE*)(dir+24)+1)=(BYTE)((WORD)(fno->fdate)>>8);
				dj.fs->wflag = 1;
				res = sync(dj.fs);
			}
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Rename File/Directory                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
	const TCHAR *path_old,	/* Pointer to the old name */
	const TCHAR *path_new	/* Pointer to the new name */
)
{
	FRESULT res;
	DIR djo, djn;
	BYTE buf[21], *dir;
	DWORD dw;
	BYTE sfn[12];


	res = chk_mounted(&path_old, &djo.fs, 1);
	if (res == FR_OK) {
		djn.fs = djo.fs;
		(djo). fn = sfn;
		res = follow_path(&djo, path_old);		/* Check old object */
		if (0 && res == FR_OK && (djo.fn[11] & 0x20))
			res = FR_INVALID_NAME;
		if (res == FR_OK) {						/* Old object is found */
			if (!djo.dir) {						/* Is root dir? */
				res = FR_NO_FILE;
			} else {
				mem_cpy(buf, djo.dir+11, 21);		/* Save the object information except for name */
				mem_cpy(&djn, &djo, sizeof(DIR));		/* Check new object */
				res = follow_path(&djn, path_new);
				if (res == FR_OK) res = FR_EXIST;		/* The new object name is already existing */
				if (res == FR_NO_FILE) { 				/* Is it a valid path and no name collision? */
/* Start critical section that any interruption or error can cause cross-link */
					res = dir_register(&djn);			/* Register the new entry */
					if (res == FR_OK) {
						dir = djn.dir;					/* Copy object information except for name */
						mem_cpy(dir+13, buf+2, 19);
						dir[11] = buf[0] | 0x20;
						djo.fs->wflag = 1;
						if (djo.sclust != djn.sclust && (dir[11] & 0x10)) {		/* Update .. entry in the directory if needed */
							dw = clust2sect(djn.fs, (((DWORD)(WORD)(((WORD)*((BYTE*)(dir+20)+1)<<8)|(WORD)*(BYTE*)(dir+20))<<16) | (WORD)(((WORD)*((BYTE*)(dir+26)+1)<<8)|(WORD)*(BYTE*)(dir+26))));
							if (!dw) {
								res = FR_INT_ERR;
							} else {
								res = move_window(djn.fs, dw);
								dir = djn.fs->win+32;	/* .. entry */
								if (res == FR_OK && dir[1] == '.') {
									dw = (djn.fs->fs_type == 3 && djn.sclust == djn.fs->dirbase) ? 0 : djn.sclust;
									{*(BYTE*)(dir+26)=(BYTE)(dw); *((BYTE*)(dir+26)+1)=(BYTE)((WORD)(dw)>>8); *(BYTE*)(dir+20)=(BYTE)((DWORD)dw>>16); *((BYTE*)(dir+20)+1)=(BYTE)((WORD)((DWORD)dw>>16)>>8);};
									djn.fs->wflag = 1;
								}
							}
						}
						if (res == FR_OK) {
							res = dir_remove(&djo);		/* Remove old entry */
							if (res == FR_OK)
								res = sync(djo.fs);
						}
					}
/* End critical section */
				}
			}
		}
		;
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Forward data to the stream directly (available on only tiny cfg)      */
/*-----------------------------------------------------------------------*/







