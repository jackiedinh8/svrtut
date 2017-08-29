#include <stdio.h>
#include <string.h>

#include "cache.h"
#include "connection.h"
#include "utils.h"

int
connection_key(const void *item)
{  
   connection_t *so =  (connection_t *)item;
   return so->flowid;
}

int
connection_eq(const void *arg1, const void *arg2)
{  
   connection_t *item1 = (connection_t *)arg1;
   connection_t *item2 = (connection_t *)arg2;
   return (item1->flowid == item2->flowid);
}

int
connection_isempty(const void *arg)
{
   connection_t *item = (connection_t *)arg;
   return (item->flowid == 0);
}

int            
connection_setempty(const void *arg) {
   connection_t *item = (connection_t *)arg;
   item->flowid = 0;
   return 0;
}


int
connection_init(hashbase_t *cache) {
   cache_init(cache, CONNECTION_SHM_KEY, CONNECTION_HASHTIME, 
         CONNECTION_HASHLEN, sizeof(connection_t),1, connection_eq, 
         connection_key, connection_isempty, connection_setempty);

   return 0;
}

connection_t*
connection_get(hashbase_t *cache, uint32_t flowid, int *is_new) {
   connection_t key;
   connection_t *so;
  
   if (!cache) return 0;
    
   key.flowid = flowid;
   so = CACHE_GET(cache, &key, is_new, connection_t*);

   if (so == 0)
      return 0;

   if (!(*is_new)) {
      return so;
   }

   // reset new session
   memset(so, 0, sizeof(connection_t));
   so->flowid = flowid;

   return so;
}

connection_t*
connection_search(hashbase_t *cache, uint32_t flowid) {
   connection_t sitem;
   sitem.flowid = flowid;
   return (connection_t*)cache_search(cache, &sitem);
}

connection_t*
connection_insert(hashbase_t *cache, connection_t *sitem) {
   return (connection_t*)cache_insert(cache, sitem);
}

int 
connection_remove(hashbase_t *cache, connection_t *sitem) {
   return cache_remove(cache, sitem);
}



