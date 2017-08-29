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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>

#include "mempool.h"

mempool_t*
mempool_create(size_t chunk_size, size_t total_size, int is_hugepage) {
   int res;
   mempool_t *mp;

   if (chunk_size < sizeof(mem_chunk)) {
      return NULL;
   }
   if (chunk_size % 4 != 0) {
      return NULL;
   }

   //assert(chunk_size <= 2*1024*1024);
   mp = (mempool_t *)calloc(1, sizeof(mempool_t));
   if (mp == NULL) {
      exit(0);
   }
   mp->mp_type = is_hugepage;
   mp->mp_chunk_size = chunk_size;
   mp->mp_free_chunks = ((total_size + (chunk_size -1))/chunk_size);
   mp->mp_total_chunks = mp->mp_free_chunks;
   total_size = chunk_size * ((size_t)mp->mp_free_chunks);

   /* allocate the big memory chunk */
   res = posix_memalign((void **)&mp->mp_startptr, getpagesize(), total_size);
   if (res != 0) {
      assert(0);
      if (mp) free(mp);
      return (NULL);
   }
   mp->mp_freeptr = (mem_chunk_t)mp->mp_startptr;
   mp->mp_freeptr->mc_free_chunks = mp->mp_free_chunks;
   mp->mp_freeptr->mc_next = NULL;

   return mp;
}

void*
mempool_allocate(mempool_t *mp) {
   mem_chunk_t p = mp->mp_freeptr;

   if (mp->mp_free_chunks == 0)
      return (NULL);
   assert(p->mc_free_chunks > 0 && p->mc_free_chunks <= p->mc_free_chunks);

   p->mc_free_chunks--;
   mp->mp_free_chunks--;
   if (p->mc_free_chunks) {
      /* move right by one chunk */
      mp->mp_freeptr = (mem_chunk_t)((char *)p + mp->mp_chunk_size);
      mp->mp_freeptr->mc_free_chunks = p->mc_free_chunks;
      mp->mp_freeptr->mc_next = p->mc_next;
   }
   else {
      mp->mp_freeptr = p->mc_next;
   }

   return p;
}

void
mempool_free(mempool_t *mp, void *p) {
   mem_chunk_t mcp = (mem_chunk_t)p;

   if (!mp || !p)
      return;

   assert(((char *)p - mp->mp_startptr) % mp->mp_chunk_size == 0);

   mcp->mc_free_chunks = 1;
   mcp->mc_next = mp->mp_freeptr;
   mp->mp_freeptr = mcp;
   mp->mp_free_chunks++;
   return;
}

void
mempool_destroy(mempool_t *mp) {
   if (!mp) return;
   free(mp->mp_startptr);
   free(mp);
   return;
}

int
mempool_capacity(mempool_t *mp) {
   return mp->mp_free_chunks;
}





