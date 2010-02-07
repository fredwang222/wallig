#ifndef MMC_H_
#define MMC_H_
/* MMC/SD command (in SPI) */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD58	(0x40+58)	/* READ_OCR */

DSTATUS MMC_disk_initialize(void);
DSTATUS MMC_disk_status(void);
DRESULT MMC_disk_read(BYTE *, DWORD ,BYTE);
DRESULT MMC_disk_write(const BYTE *, DWORD ,BYTE);
DRESULT MMC_disk_ioctl(BYTE, void *);
#endif /*MMC_H_*/
