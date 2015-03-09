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
	WCHAR*	lfn;			/* Pointer to the LFN working buffer */
	WORD	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
} DIR;



/* File status structure (FILINFO) */

typedef struct {
	DWORD	fsize;			/* File size */
	WORD	fdate;			/* Last modified date */
	WORD	ftime;			/* Last modified time */
	BYTE	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR */
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
WCHAR ff_convert (WCHAR, UINT);		/* OEM-Unicode bidirectional conversion */
WCHAR ff_wtoupper (WCHAR);			/* Unicode upper-case conversion */

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





WCHAR ff_convert (	/* Converted character, Returns zero on error */
	WCHAR	src,	/* Character code to be converted */
	UINT	dir		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
)
{
	WCHAR c;


	if (src < 0x80) {	/* ASCII */
		c = src;

	} else {
		if (dir) {		/* OEMCP to Unicode */
			c = (src >= 0x100) ? 0 : Tbl[src - 0x80];

		} else {		/* Unicode to OEMCP */
			for (c = 0; c < 0x80; c++) {
				if (src == Tbl[c]) break;
			}
			c = (c + 0x80) & 0xFF;
		}
	}

	return c;
}


WCHAR ff_wtoupper (	/* Upper converted character */
	WCHAR chr		/* Input character */
)
{
	static const WCHAR tbl_lower[] = { 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xA1, 0x00A2, 0x00A3, 0x00A5, 0x00AC, 0x00AF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x0FF, 0x101, 0x103, 0x105, 0x107, 0x109, 0x10B, 0x10D, 0x10F, 0x111, 0x113, 0x115, 0x117, 0x119, 0x11B, 0x11D, 0x11F, 0x121, 0x123, 0x125, 0x127, 0x129, 0x12B, 0x12D, 0x12F, 0x131, 0x133, 0x135, 0x137, 0x13A, 0x13C, 0x13E, 0x140, 0x142, 0x144, 0x146, 0x148, 0x14B, 0x14D, 0x14F, 0x151, 0x153, 0x155, 0x157, 0x159, 0x15B, 0x15D, 0x15F, 0x161, 0x163, 0x165, 0x167, 0x169, 0x16B, 0x16D, 0x16F, 0x171, 0x173, 0x175, 0x177, 0x17A, 0x17C, 0x17E, 0x192, 0x3B1, 0x3B2, 0x3B3, 0x3B4, 0x3B5, 0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BB, 0x3BC, 0x3BD, 0x3BE, 0x3BF, 0x3C0, 0x3C1, 0x3C3, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F, 0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F, 0x451, 0x452, 0x453, 0x454, 0x455, 0x456, 0x457, 0x458, 0x459, 0x45A, 0x45B, 0x45C, 0x45E, 0x45F, 0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0 };
	static const WCHAR tbl_upper[] = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x21, 0xFFE0, 0xFFE1, 0xFFE5, 0xFFE2, 0xFFE3, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x178, 0x100, 0x102, 0x104, 0x106, 0x108, 0x10A, 0x10C, 0x10E, 0x110, 0x112, 0x114, 0x116, 0x118, 0x11A, 0x11C, 0x11E, 0x120, 0x122, 0x124, 0x126, 0x128, 0x12A, 0x12C, 0x12E, 0x130, 0x132, 0x134, 0x136, 0x139, 0x13B, 0x13D, 0x13F, 0x141, 0x143, 0x145, 0x147, 0x14A, 0x14C, 0x14E, 0x150, 0x152, 0x154, 0x156, 0x158, 0x15A, 0x15C, 0x15E, 0x160, 0x162, 0x164, 0x166, 0x168, 0x16A, 0x16C, 0x16E, 0x170, 0x172, 0x174, 0x176, 0x179, 0x17B, 0x17D, 0x191, 0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399, 0x39A, 0x39B, 0x39C, 0x39D, 0x39E, 0x39F, 0x3A0, 0x3A1, 0x3A3, 0x3A4, 0x3A5, 0x3A6, 0x3A7, 0x3A8, 0x3A9, 0x3AA, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E, 0x41F, 0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A, 0x40B, 0x40C, 0x40E, 0x40F, 0x2160, 0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A, 0x216B, 0x216C, 0x216D, 0x216E, 0x216F, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0 };
	int i;


	for (i = 0; tbl_lower[i] && chr != tbl_lower[i]; i++) ;

	return tbl_lower[i] ? tbl_upper[i] : chr;
}
