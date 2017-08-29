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
 * @(#)mq.h
 */

#ifndef _SVR_CORE_MQ_H_
#define _SVR_CORE_MQ_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#include "shm.h"

//#define SHM_HEAD_SIZE 8
#define SHM_HEAD_SIZE sizeof(void*) * 2

#define MQ_SIZE 33554432
#define IO_2_MD_KEY 1168647512
#define MD_2_IO_KEY 1168647513

typedef struct shm_mq shm_mq_t;
struct shm_mq
{
	 shm_data_t*       _shm;
   uint32_t        _fd;
   uint32_t        _wait_sec;
   uint32_t        _wait_usec;

   uint32_t*       _head;
   uint32_t*       _tail;
   char*           _block;
   uint32_t        _block_size;
}__attribute__((packed));

int 
shmmq_init(shm_mq_t *mq, const char* fifo_path, 
      int32_t wait_sec, int32_t wait_usec, 
      int32_t shm_key, int32_t shm_size);

void 
shmmq_release(shm_mq_t *mq);

int 
shmmq_enqueue(shm_mq_t *mq, 
      const time_t uiCurTime, const void* data, 
      uint32_t data_len, uint32_t flow);

int 
shmmq_dequeue(shm_mq_t *mq, void* buf, 
      uint32_t buf_size, uint32_t *data_len, uint32_t *flow);

#ifdef __cplusplus
}
#endif

#endif//_SVR_CORE_MQ_H_

