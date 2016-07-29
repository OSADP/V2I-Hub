#ifndef _DEAMON_H_
#define _DEAMON_H_


enum {LOGCPY = 1, SCPCPY, USBUSAGE, CERTCPY,USBPURGE,ETH_LOGCPY,USBCALUSAGE};
typedef struct{
	int cmd; // 1-logcpy, 2-scpcpy, 3-usbusage, 4-certcpy 5-usbpurge
	int ret;
	char fname[255];
}usbsock_cmd;

#endif
