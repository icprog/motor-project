/*------------------------------------------------------------------------*/
/* Unicode - Local code bidirectional converter  (C)ChaN, 2009            */
/* (SBCS code pages)                                                      */
/*------------------------------------------------------------------------*/
/*  437   U.S. (OEM)
/   720   Arabic (OEM)
/   1256  Arabic (Windows)
/   737   Greek (OEM)
/   1253  Greek (Windows)
/   1250  Central Europe (Windows)
/   775   Baltic (OEM)
/   1257  Baltic (Windows)
/   850   Multilingual Latin 1 (OEM)
/   852   Latin 2 (OEM)
/   1252  Latin 1 (Windows)
/   855   Cyrillic (OEM)
/   1251  Cyrillic (Windows)
/   866   Russian (OEM)
/   857   Turkish (OEM)
/   1254  Turkish (Windows)
/   858   Multilingual Latin 1 + Euro (OEM)
/   862   Hebrew (OEM)
/   1255  Hebrew (Windows)
/   874   Thai (OEM, Windows)
/   1258  Vietnam (OEM, Windows)
*/

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





static
const WCHAR Tbl[] = {	/*  CP437(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
	0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};



