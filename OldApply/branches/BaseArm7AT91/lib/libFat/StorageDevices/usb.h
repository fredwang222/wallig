#ifndef USB_H_
#define USB_H_
int USB_disk_initialize(void);
int USB_disk_status(void);
int USB_disk_read(BYTE *, DWORD ,BYTE);
int USB_disk_write(const BYTE *, DWORD ,BYTE);
int USB_disk_ioctl(BYTE, void *);
#endif /*USB_H_*/
