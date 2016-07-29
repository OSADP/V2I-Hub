#ifndef __WSMP_H__
#define __WSMP_H__

#ifndef LINUX_VERSION_CODE
# include <linux/version.h>
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
# include <linux/config.h>
#else
# include <linux/autoconf.h>
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fcntl.h>
#include <net/scm.h>
#include <net/sock.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,41)
#else
#include <linux/cdev.h>
#endif
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/current.h>

#ifdef ACQUILA_CODE_BASE
#include "if_upperproto.h"
#else
#include "if_ethersubr.h" 
#endif

#ifdef ar6000
#include <queue.h>
#else
#include <sys/queue.h>
#endif

#include "common.h"


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,41)
#include <linux/tqueue.h>
#define WME_WORK_THREAD         tq_struct
#define WME_SCHEDULE_TASK(t)        schedule_task((t))
#define WME_INIT_SCHED_TASK(t, f, d)    do { memset((t),0,sizeof(struct tq_struct)); \
                        (t)->routine = (void (*)(void*)) (f); \
                        (t)->data=(void *) (d); } while (0)
#define WME_FLUSH_TASKS         flush_scheduled_tasks
#else
#include <linux/workqueue.h>
#define WME_SCHEDULE_TASK(t)        schedule_work((t))
#define WME_SCHEDULE_DELAYED_TASK(t, delay)        schedule_delayed_work((t),delay)
//#define WME_INIT_SCHED_TASK(t, f, d)  (DECLARE_WORK((t), (f), (d)))
#if 0
#define WME_INIT_SCHED_TASK(t, f, d)    do { memset(((void *) (t)),0,sizeof(struct work_struct)); \
       PREPARE_WORK((t),((void (*)(void*))(f)),((void *) (d))); } while (0)
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#define WME_INIT_SCHED_TASK(t, f, d)  do { INIT_WORK((t),(f),(d));\
	PREPARE_WORK((t),(f),(d)); } while(0)
#else
#define WME_INIT_SCHED_TASK(t, f)  do { INIT_WORK((t),(f));\
	PREPARE_WORK((t),(f)); } while(0)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#define WME_INIT_DELAYED_SCHED_TASK(t, f, d) do { INIT_WORK((t),(f),(d));\
        PREPARE_WORK((t),(f),(d)); } while(0)
#else
#define WME_INIT_DELAYED_SCHED_TASK(t, f) do { INIT_DELAYED_WORK((t),(f));\
        PREPARE_DELAYED_WORK((t),(f)); } while(0)
#endif
#define WME_WORK_THREAD         work_struct
#define WME_DELAYED_WORK_THREAD delayed_work
#define WME_FLUSH_TASKS         flush_scheduled_work
#endif /* KERNEL_VERSION < 2.5.41 */


#define	IEEE80211_ADDR_COPY(dst,src)	memcpy(dst,src,IEEE80211_ADDR_LEN)
#define WSMP_MAX_MSG_LEN 697


typedef rwlock_t wme_tablelock_t;
#define WME_TABLE_LOCK_INIT(_nt) rwlock_init(&(_nt)->lock)
#define WME_TABLE_LOCK_DESTROY(_nt)
#define WME_TABLE_LOCK(_nt) write_lock(&(_nt)->lock)
#define WME_TABLE_UNLOCK(_nt)   write_unlock(&(_nt)->lock)
#define WME_TABLE_LOCK_BH(_nt)  write_lock_bh(&(_nt)->lock)
#define WME_TABLE_UNLOCK_BH(_nt)    write_unlock_bh(&(_nt)->lock)

#define HASHSIZE 32
#define DIGESTLEN 8
#define NBUF 10
#define SUCCESS 0
#define DEVICE_NAME "wsmp"

#define UDP_PORT 9999
#define UDP_THREAD_NAME "wme_udp_server"
#define UDP_TIMEOUT 100

#ifndef WSMP_MAX_MSG_LEN
#define WSMP_MAX_MSG_LEN 1400
#endif
#ifndef WSMP_MINLEN
#define WSMP_MINLEN 10
#endif
#ifndef BUFSIZE
#define BUFSIZE 1400
#endif 
#ifndef WSMP_MAX_RXBUFS
#define WSMP_MAX_RXBUFS 100
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
#ifndef WME_PRINTF
#ifdef DEBUG
#define WME_PRINTF( ... ) printk(KERN_INFO __VA_ARGS__)
#else
#define WME_PRINTF(_fmt, ...)
#endif
#endif

struct rx_buf 
{
	int len;
	char *data;
	TAILQ_ENTRY(rx_buf) entries;	
};

struct wsmp_buf 
{
	int len;
	char *data;
	TAILQ_ENTRY(wsmp_buf) entries;	
};

struct udp_buf 
{
	int len;
	void *data;
	TAILQ_ENTRY(udp_buf) entries;	
};

struct radio{
    struct net_device *dev;
    void *vap;
    uint8_t servicechan; // Channel in Tx Profile
    wait_queue_head_t waitqueue;
    wait_queue_head_t channelwaitqueue;
    int wsmpnobuff; //implementation of blocking read
    atomic_t cchqueuestopped;
    atomic_t schqueuestopped;
    uint8_t notifready;
    uint8_t wrssready;
    uint8_t appindready;
    uint8_t wrsssent;
    uint8_t schmac[IEEE80211_ADDR_LEN];  	
    uint8_t cchmac[IEEE80211_ADDR_LEN];  	
    uint8_t flag_mac_random;  
    //extra info
    uint8_t priority;
    uint8_t chAccessMode;
    uint8_t oldServicechan;
    int32_t inUse;
};

uint8_t wme_table_hash (uint32_t psid);

uint8_t wmeGetChannelIdx(uint8_t channel);
uint8_t wmeGetChannel(uint8_t channelIdx);
uint8_t acm_equal(wme_acm acm1, wme_acm acm2);

typedef enum { message_anonymous=0, message_identified_not_localized =1,message_identified_localized =2,message_csr = 3,wsa =4,wsa_csr =5,message_ca=6, wsa_ca =7, crl_signer=8,message_ra =9,root_ca=255} SubjectType;
typedef enum {from_issuer=0, specified=1} ArrayType;
typedef enum {from_issuer_region =0, circle =1, rectangle =2, polygon =3, none =4} RegionType;

typedef struct {
    uint8_t certInfo[256];
    int16_t certLen;
    uint8_t version_and_type;
    uint8_t subject_type;
    uint8_t cf; 
    uint8_t signature_alg;
    uint16_t signer_id;
    int16_t ad_len; //additional_data length
    uint16_t ad;//additional_data
    int16_t scopelen;
    uint16_t scope;
    uint16_t expiration;
    uint16_t start_validity_or_lft;
    uint16_t crl_series; 
    uint16_t verification_key;
    uint16_t encryption_key;
    uint16_t sig_RV;
} Certificate;

#endif
