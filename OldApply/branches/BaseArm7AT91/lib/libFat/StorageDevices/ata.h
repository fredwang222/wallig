#ifndef ATA_H_
#define ATA_H_
int ATA_disk_initialize(void);
int ATA_disk_status(void);
int ATA_disk_read(BYTE *, DWORD ,BYTE);
int ATA_disk_write(const BYTE *, DWORD ,BYTE);
int ATA_disk_ioctl(BYTE, void *);
#endif /*ATA_H_*/
