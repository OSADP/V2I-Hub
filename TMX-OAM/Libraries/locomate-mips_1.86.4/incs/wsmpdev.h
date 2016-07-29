#ifndef _WSMPDEV_H_
#define _WSMPDEV_H_
#define WSMPQSTOP -2

#include "wsmp.h"
#ifdef ACQUILA_CODE_BASE
#include "ieee80211_mlme_wave.h"
#else
#include "ieee80211_mlme.h"
#endif

#if 0
#define NBUF 10
#define SUCCESS 0
#define DEVICE_NAME "wsmp"
#ifndef WSMP_MAX_MSG_LEN
#define WSMP_MAX_MSG_LEN 1400
#endif
#ifndef WSMP_MINLEN
#define WSMP_MINLEN 10
#endif
#ifndef BUFSIZE
#define BUFSIZE 1400
#endif 

#define WME_RX_LOCK_T spinlock_t
#define WME_RX_LOCK_INIT(_tq) spin_lock_init(&(_tq)->rxlock)
#define WME_RX_LOCK(_tq) spin_lock(&(_tq)->rxlock)
#define WME_RX_UNLOCK(_tq) spin_unlock(&(tq)->rxlock)
#define WME_RX_LOCK_BH(_tq) spin_lock_bh(&(_tq)->rxlock)
#define WME_RX_UNLOCK_BH(_tq) spin_unlock_bh(&(_tq)->rxlock)

#define WME_WSMP_LOCK_T spinlock_t
#define WME_WSMP_LOCK_INIT(_tq) spin_lock_init(&(_tq)->wsmplock)
#define WME_WSMP_LOCK(_tq) spin_lock(&(_tq)->wsmplock)
#define WME_WSMP_UNLOCK(_tq) spin_unlock(&(_tq)->wsmplock)
#define WME_WSMP_LOCK_BH(_tq) spin_lock_bh(&(_tq)->wsmplock)
#define WME_WSMP_UNLOCK_BH(_tq) spin_unlock_bh(&(_tq)->wsmplock)

#define WME_WSA_LOCK_T spinlock_t
#define WME_WSA_LOCK_INIT(_tq) spin_lock_init(&(_tq)->wsalock)
#define WME_WSA_LOCK(_tq) spin_lock(&(_tq)->wsalock)
#define WME_WSA_UNLOCK(_tq) spin_unlock(&(_tq)->wsalock)
#define WME_WSA_LOCK_BH(_tq) spin_lock_bh(&(_tq)->wsalock)
#define WME_WSA_UNLOCK_BH(_tq) spin_unlock_bh(&(_tq)->wsalock)

#endif

#define MAX_RADIO 2
#define AUTO_RADIO_MODE 1
#define CONFIG_RADIO_MODE 2
#define SINGLE_RADIO_MODE 3


struct wsmp_dev
{
	struct radio *radio[MAX_RADIO];
    struct cdev *cdev;
	
	struct task_struct *udpthread;
	struct socket *udpsock;
	struct socket *txwsasock;
	struct socket *rxwsasock;
	struct socket *gpswsasock;
#if 0
	struct fasync_struct *async_queue;
#endif
	struct wme wme;
	struct mlmeWsaIndication mlmewsa;
	char oldSecuredWsa[MAX_WSA_SIZE];
	char oldUnsignedWsa[MAX_WSA_SIZE];
	uint16_t oldUnsignedWsaLen;
	//WSA oldSecWsa;
	int (*mlmeRequest)(void *vap, struct mlmeHandler *);
	uint64_t (*gettsf)(void *vap);
	uint64_t (*settsf)(void *vap, uint64_t tsf);
	uint64_t (*incrementtsf)(void *vap, int inc);
	int8_t (*canceltx)(void *vap, uint8_t channel, uint8_t aci);
	
//	struct pidInfo *wrsspidinfo;

	WME_WSA_LOCK_T wsalock;
	WME_RX_LOCK_T wsmindlock;
	WME_RX_LOCK_T udpresultlock;
	WME_WSA_LOCK_T udpqlock;

	struct pid_table wme_pidtable;
	struct workqueue_struct *wq;
	struct workqueue_struct *udpq;
	struct workqueue_struct *wsawq;
	//struct workqueue_struct *usrwq;
	
	uint8_t condition; //has to remove
	struct WME_WORK_THREAD parsewsatask;
	struct WME_WORK_THREAD sendudptask;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
	struct WME_WORK_THREAD WsaUpdateTask;
#else
	struct WME_DELAYED_WORK_THREAD WsaUpdateTask;
#endif
/*#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
	struct WME_WORK_THREAD UsrUpdateTask;
#else
	struct WME_DELAYED_WORK_THREAD UsrUpdateTask;
#endif*/


	uint8_t linkconfirm;//has to remove
    uint8_t servicedataready;
    uint8_t wsmprxbufs; /*No. of Rx buffers used*/
    uint8_t tsfready;
    uint8_t macready;
    uint8_t noRadio;
//	uint8_t resultsent;
	TAILQ_HEAD(, udp_buf) udpbufq;
	TAILQ_HEAD(, udp_buf) udpqhead;

//has to remove. kept them to avoid compilation errors
    struct net_device *dev;
    void *vap;
    wait_queue_head_t waitqueue;
    uint8_t notifready;
    uint8_t wrssready;
	uint8_t wrsssent;
    uint8_t appindready;


};

typedef struct  {
    	uint32_t      actual_time;        //no. of sec from jan 1, 1970 00:00:00
        uint32_t      time;
        uint64_t      local_tod;
        uint64_t      local_tsf;
        long          latitude;
        char          latdir;
        long          longitude;
        char          longdir;
        uint16_t      altitude;
        char          altunit;
        uint16_t      course;
        uint16_t      speed;
        uint64_t      climb;
        uint64_t      tee;
        uint64_t      hee;
        uint64_t      vee;
        uint64_t      cee;
        uint64_t      see;
        uint64_t      clee;
        uint64_t      hdop;
        uint64_t      vdop;
        uint8_t       numsats;
        uint8_t       fix;
        uint64_t      tow;
        uint32_t      date;
        uint64_t      epx;
        uint64_t      epy; 
        uint64_t      epv;
            
}__attribute__ ((packed)) GPSSAEData;   


#define ATOMIC_SET(_x, _y)	atomic_set(&(_x), (_y))
#endif 
