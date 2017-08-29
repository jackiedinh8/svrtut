#ifndef _SNOW_CORE_FLOW_H_
#define _SNOW_CORE_FLOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "linux_list.h"

typedef struct flow flow_t;
struct flow {
   struct list_head  list;
   uint32_t  flowid;
   void     *obj;
};

typedef struct flowset flowset_t;
struct flowset {
   struct list_head  freelist;
   struct list_head  usedlist;
   uint32_t          totalnum;
   uint32_t          usednum;
   uint32_t          baseidx;

   flow_t       *data;
};

flowset_t*
flowset_init(uint32_t num);

uint32_t
flowset_getid(flowset_t *s);

void
flowset_freeid(flowset_t *s, uint32_t id);

void
flowset_setobj(flowset_t *s, uint32_t id, void *obj);

void*
flowset_getobj(flowset_t *s, uint32_t id);

void
flowset_free(flowset_t *set);


#ifdef __cplusplus
}
#endif

#endif //_SNOW_CORE_FLOW_H_
