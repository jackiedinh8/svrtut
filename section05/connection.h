#ifndef _SVR_CONNECTION_H_
#define _SVR_CONNECTION_H_

#include <stdint.h>

typedef struct connection connection_t;
struct connection {
   uint32_t flowid;
   uint32_t msg_cnt;
};


int
connection_init(hashbase_t *ctx);

connection_t*
connection_get(hashbase_t *ctx, uint32_t flowid, int *is_new);

connection_t*
connection_search(hashbase_t *ctx, uint32_t flowid);

connection_t*
connection_insert(hashbase_t *ctx, connection_t *sitem);

int 
connection_remove(hashbase_t *ctx, connection_t *sitem);

#endif //_SVR_CONNECTION_H_


