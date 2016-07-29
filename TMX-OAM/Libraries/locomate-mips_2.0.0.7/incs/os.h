#ifndef __OS_H__
#define __OS_H__
#ifdef	WIN32
#include <winsock2.h>
#include <winioctl.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>	
#include <pthread.h>
#include <io.h>
#include <Mswsock.h>
#include <sys/stat.h>
#include <iphlpapi.h>
#include "win_if.h"
#include "win_poll.h"
#include "win_getopt.h"


#define uint8_t				unsigned __int8	
#define	uint16_t			unsigned __int16	
#define	uint32_t			unsigned __int32	
#define	uint64_t			unsigned __int64
#define	u_int8_t			unsigned __int8	
#define	u_int16_t			unsigned __int16	
#define	u_int32_t			unsigned __int32	
#define	u_int64_t			unsigned __int64
#define int64_t				__int64
#define NAN					0

#define int8_t				signed  __int8
#define int16_t                         signed  __int16
#define __attribute__(x)
#define close _close
#define perror(x) printf(x " Error No %d\n",WSAGetLastError())
//#define inet_pton InetPton

#define	WIN_SOCK_DLL_INVOKE	{WSADATA wsaData;\
		int iResult;\
		iResult=WSAStartup(MAKEWORD(2,2),&wsaData);\
		if(iResult!=0)\
			printf("WSASTARTUP Failed: %d\n",iResult);\
	}



#define	SIOCGIFHWADDR		0x8915
#define	snprintf			_snprintf
#define	strcasecmp			_stricmp
#define	sleep(x)			Sleep(x * 1000)
#define	getpid				_getpid

#ifndef GPSCONFIG
#define GPSCONFIG	"\\gps-wave.config"
#endif
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/in_systm.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <getopt.h>
#include <unistd.h>
#include <termio.h>
#include <sys/stat.h>
#include <sys/queue.h>
#ifdef __kernel__
#include <aio.h>
#endif
#include <sys/poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/errno.h>
#include <bits/nan.h>
#ifndef GPSCONFIG
#define GPSCONFIG		 "/etc/gps-wave.config"
#define DEVNAME			 "/dev/wsmp"
#endif
#define WIN_SOCK_DLL_INVOKE
#include <ifaddrs.h>
#include<stdint.h>
#endif
#endif //__OS_H__
