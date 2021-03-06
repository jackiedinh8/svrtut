/*
 * Copyright (c) 2015 Jackie Dinh <jackiedinh8@gmail.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1 Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  2 Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution.
 *  3 Neither the name of the <organization> nor the 
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @(#)cache.h
 */

#ifndef _SVR_CORE_CACHE_H_
#define _SVR_CORE_CACHE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*eqfn) (const void *, const void *);
typedef int (*keyfn) (const void *);
typedef int (*isemptyfn) (const void *);
typedef int (*setemptyfn) (const void *);


typedef struct hashbase hashbase_t;
struct hashbase {
   uint32_t   hb_time;
   uint32_t   hb_len;
   uint32_t   hb_objsize;
   uint32_t  *hb_base;
   uint32_t   hb_size;
   char      *hb_cache;
   eqfn       hb_eqfn;
   keyfn      hb_keyfn;
   isemptyfn  hb_isemptyfn;
   setemptyfn hb_setemptyfn;
};

int
cache_init(hashbase_t *base, uint32_t key, 
      uint32_t hashtime, uint32_t hashlen, uint32_t objsize, 
      uint32_t create, eqfn equal_fn, keyfn key_fn,
      isemptyfn isempty_fn, setemptyfn setempty_fn);

void*
cache_get(hashbase_t *base, void *sitem, int *is_new);

void*
cache_search(hashbase_t *base, void *sitem);

void*
cache_insert(hashbase_t *base, void *sitem);

int
cache_remove(hashbase_t *base, void *sitem);

int
cache_finit(hashbase_t *base);

void*
cache_search_new(hashbase_t *base, void *sitem, eqfn _eqfn);

#define CACHE_GET(base, item, is_new, type) (type)(cache_get(base, item, is_new));
#define CACHE_SEARCH(base, item, type) (type)(cache_search(base, item));
#define CACHE_INSERT(base, item, type) (type)cache_insert(base, item);
#define CACHE_REMOVE(base, item) cache_remove(base, item);

/*
int 
cache_size();

void 
cache_status(float& usage_percent);

void 
cache_itemcount();
*/

#ifdef __cplusplus
}
#endif

#endif //_SVR_CORE_CACHE_H_
