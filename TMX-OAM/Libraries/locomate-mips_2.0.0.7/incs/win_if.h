/*//--------------------------if.h-------------------------------//*/
#ifndef _WIN_IF_H_
#define _WIN_IF_H_
#include <winsock2.h>
#include <ws2tcpip.h>
#define uint8_t				unsigned __int8	
#define	uint16_t			unsigned __int16	
#define	uint32_t			unsigned __int32	
#define	uint64_t			unsigned __int64
#define	u_int8_t			unsigned __int8	
#define	u_int16_t			unsigned __int16	
#define	u_int32_t			unsigned __int32	
#define	u_int64_t			unsigned __int64


struct ifmap
{
	u_int64_t mem_start;
	u_int64_t mem_end;
	u_int16_t base_addr;
	unsigned char	 irq;
	unsigned char	 dma;
	unsigned char	 port;
};
	
struct ifreq{
#define IFHWADDRLEN		6
#define IF_NAMESIZE		16
#define IFNAMSIZ		IF_NAMESIZE
	
	union
	{
		char	ifrn_name[IFNAMSIZ];
	}ifr_ifrn;
	
	union{ 
		struct sockaddr ifru_addr;
		struct sockaddr ifru_dstaddr;	
		struct sockaddr ifru_broadaddr;
		struct sockaddr ifru_netmask;
		struct sockaddr ifru_hwaddr;
		short int ifru_flags;
		int ifru_ivalue;
		int ifru_mtu;
		struct ifmap ifru_map;
		char ifru_slave[IFNAMSIZ];
		char ifru_newname[IFNAMSIZ];
		char *ifru_data;
	}ifr_ifru;
};

#define ifr_name		ifr_ifrn.ifrn_name		 // interface name
#define ifr_hwaddr		ifr_ifru.ifru_hwaddr	 // MAC address	
#define ifr_addr		ifr_ifru.ifru_addr		 // address	
#define ifr_dstaddr		ifr_ifru.ifru_dstaddr	 // other end of p-p link	
#define ifr_broadaddr	ifr_ifru.ifru_boardaddr	 // broadcast address
#define ifr_netmask		ifr_ifru.ifru_netmask	 // interface net mask
#define ifr_flags		ifr_ifru.ifru_flags		 // flags
#define ifr_metric		ifr_ifru.ifru_ivalue	 // metric
#define ifr_mtu			ifr_ifru.ifru_mtu		 // mtu
#define ifr_map			ifr_ifru.ifru_map		 // display map
#define ifr_slave		ifr_ifru.ifru_slave		 // slave device
#define ifr_data		ifr_ifru.ifru_data		 // for use by interface	
#define ifr_ifindex		ifr_ifru.ifru_ivalue	 // interface index
#define ifr_bandwidth	ifr_ifru.ifru_ivalue	 // link bandwidth 
#define ifr_qlen		ifr_ifru.ifru_ivalue	 // Queue length
#define ifr_newname		ifr_ifru.ifru_newname	 // New name

struct ifconf
{
	int ifc_len;
	union
	{
		char *		ifcu_buf;
		//struct 		*ifcu_req;
	}ifc_ifcu;
};

#define ifc_buf	ifc_ifcu.ifcu_buf
#define ifc_req	ifc_ifcu.ifcu_req
#define _IOT_ifconf _IOT(_IOTS(struct ifconf),1,0,0,0,0)
#endif 

//------------------------------end of  IF.H   ---------------------------//
