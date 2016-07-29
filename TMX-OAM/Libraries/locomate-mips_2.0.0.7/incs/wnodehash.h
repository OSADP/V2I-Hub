#ifndef __WNODEHASH_H__
#define __WNODEHASH_H__

#define MAX_HASH_SIZE 32
#define MAX_LIST 256
#define IEEE80211_ADDR_LEN 6

// Value '0' is indicated as NULL for idx fields

struct wnodehash {
  uint8_t head;
  uint8_t tail;
}__attribute__((__packed__));


struct wnode {
   uint8_t validationstatus;
   uint8_t nextidx;
   uint8_t macaddr[IEEE80211_ADDR_LEN];
}__attribute__((__packed__));



//Forward Declarations 

uint8_t ParseFreeWnode();
uint8_t AllocateWnode(struct wnode* , uint8_t ,  uint8_t* );
int8_t LookupHash(struct wnodehash* ,struct wnode* , uint8_t* ); 
uint8_t InsertHash(struct wnodehash* , struct wnode* , uint8_t , uint8_t ); 
void    FreeHash(struct wnodehash* , struct wnode* );


#endif

