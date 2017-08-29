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
 * @(#)mq.c
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#ifdef linux
#include <time.h>
#endif

#include "mq.h"

int 
shmmq_init(shm_mq_t *mq, const char* fifo_path, 
      int32_t wait_sec, int32_t wait_usec, 
      int32_t shm_key, int32_t shm_size) {
   int ret = 0;
   int val;
   char *mem_addr = NULL;
   int mode = 0666 | O_NONBLOCK | O_NDELAY;

   if (mq == NULL)  {
      return -1;
   }

   errno = 0;
   if ((mkfifo(fifo_path, mode)) < 0) {
     if (errno != EEXIST) {
        ret = -1;
        goto done;
     }
   }

   if ((mq->_fd = open(fifo_path, O_RDWR)) < 0) {
     ret = -2;
     goto done;
   }

  if (mq->_fd > 1024) {
    close(mq->_fd);
    ret = -3;
    goto done;
  }
    
  val = fcntl(mq->_fd, F_GETFL, 0);
  
  if (val == -1) {
    ret = errno ? -errno : val;
      goto done;
  }

  if (val & O_NONBLOCK) {
    ret = 0;
    goto done;
  }
  
  ret = fcntl(mq->_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY);

  if (ret < 0) {
      ret = errno ? -errno : ret;
      goto done;
   } else
      ret = 0;

  assert(shm_size > SHM_HEAD_SIZE);

  mq->_shm = shmdata_create(shm_key, shm_size);

  if ( mq->_shm == NULL ) {
     mq->_shm = shmdata_open(shm_key, shm_size);
     if ( mq->_shm == NULL ) {
       ret = -1;
       goto done;
     }
     mem_addr = mq->_shm->addr;
     goto setup;
  } else {
     mem_addr = mq->_shm->addr;
  }

  // init head portion of shared meme.
  memset(mem_addr, 0, SHM_HEAD_SIZE);

  mq->_wait_sec = wait_sec;
  mq->_wait_usec = wait_usec;
 
setup:

  mq->_head = (uint32_t*)mem_addr;
  mq->_tail = mq->_head+1;
  mq->_block = (char*) (mq->_tail+1);
  mq->_block_size = shm_size - SHM_HEAD_SIZE;

  ret = 0;
done:
   return ret;
}

void 
release_shmmq(shm_mq_t *mq) {
   // TODO
}

int
write_mq(shm_mq_t *mq, const void* data, uint32_t data_len, uint32_t flow) {
  uint32_t head;
  uint32_t tail;
  uint32_t free_len;
  uint32_t tail_len;
  char msg_header[SHM_HEAD_SIZE] = {0};
  uint32_t total_len;
  int ret = 0;

  if (mq == NULL) return -1;

  head = *mq->_head;
  tail = *mq->_tail;
  free_len = head>tail? head-tail : head + mq->_block_size - tail;
  tail_len = mq->_block_size - tail;
  total_len = data_len + SHM_HEAD_SIZE;

  // the queue is full?
  if (free_len <= total_len) {
    ret = -1;
    goto done;
  }

  memcpy(msg_header, &total_len, sizeof(uint32_t));
  memcpy(msg_header + sizeof(uint32_t), &flow, sizeof(uint32_t));
  if (tail_len >= total_len) {
    memcpy(mq->_block+tail, msg_header, SHM_HEAD_SIZE);
    memcpy(mq->_block+tail+ SHM_HEAD_SIZE, data, data_len);
    *mq->_tail += data_len + SHM_HEAD_SIZE;
  }
  else if (tail_len >= SHM_HEAD_SIZE && tail_len < SHM_HEAD_SIZE+data_len) {
    uint32_t first_len = 0;
    uint32_t second_len = 0;
    int32_t wrapped_tail = 0;
    memcpy(mq->_block+tail, msg_header, SHM_HEAD_SIZE);
    first_len = tail_len - SHM_HEAD_SIZE;
    memcpy(mq->_block+tail+ SHM_HEAD_SIZE, data, first_len);
    second_len = data_len - first_len;
    memcpy(mq->_block, ((char*)data) + first_len, second_len);

    wrapped_tail = *mq->_tail + data_len + SHM_HEAD_SIZE - mq->_block_size;
    *mq->_tail = wrapped_tail;
  }
  else {
    uint32_t second_len = 0;
    memcpy(mq->_block+tail, msg_header, tail_len);
    second_len = SHM_HEAD_SIZE - tail_len;
    memcpy(mq->_block, msg_header + tail_len, second_len);
    memcpy(mq->_block + second_len, data, data_len);
    *mq->_tail = second_len + data_len;
  }
  
  if(free_len == mq->_block_size) 
    return 1;
  else
    return 0;
done:
   return ret;
}

int 
shmmq_enqueue(shm_mq_t *mq, 
      const time_t cur_time, const void* data, 
      uint32_t data_len, uint32_t flow) {
   int ret = 0;

   if (mq == NULL)
      return -1;

   ret = write_mq(mq, data, data_len, flow);
   if (ret < 0) return ret;

   errno = 0;
   ret = write(mq->_fd, "\0", 1);
   return ret;
}

int
read_mq(shm_mq_t *mq, void* buf, uint32_t buf_size, 
     uint32_t *data_len, uint32_t *flow) {
  int ret = 0;
  char data[SHM_HEAD_SIZE];
  uint32_t used_len;
  uint32_t total_len;
  uint32_t head = *mq->_head;
  uint32_t tail = *mq->_tail;

  if (head == tail) {
    *data_len = 0;
    ret = 0;
    goto done;
  }

  used_len = tail>head ? tail-head : tail+mq->_block_size-head;
  
  //  if head + 8 > block_size
  if (head+SHM_HEAD_SIZE > mq->_block_size) {
    uint32_t first_size = mq->_block_size - head;
    uint32_t second_size = SHM_HEAD_SIZE - first_size;
    memcpy(data, mq->_block + head, first_size);
    memcpy(data + first_size, mq->_block, second_size);
    head = second_size;
  } else {
    memcpy(data, mq->_block + head, SHM_HEAD_SIZE);
    head += SHM_HEAD_SIZE;
  }
  
  //  get meta data
  total_len  = *(uint32_t*) (data);
  *flow = *(uint32_t*) (data+sizeof(uint32_t));

  assert(total_len <= used_len);
  
  *data_len = total_len-SHM_HEAD_SIZE;

  if (*data_len > buf_size) {
    ret = -1;
    goto done;
  }
  if (head+*data_len > mq->_block_size) {
    uint32_t first_size = mq->_block_size - head;
    uint32_t second_size = *data_len - first_size;
    memcpy(buf, mq->_block + head, first_size);
    memcpy(((char*)buf) + first_size, mq->_block, second_size);
    *mq->_head = second_size;
  } else {
    memcpy(buf, mq->_block + head, *data_len);
    *mq->_head = head+*data_len;
  }
done:
  return ret;
};

int 
shmmq_select_fifo(int _fd, unsigned _wait_sec, 
      unsigned _wait_usec) {
   fd_set readfd;
   struct timeval tv;
   int ret = 0; 

   FD_ZERO(&readfd);
   FD_SET(_fd, &readfd);
   tv.tv_sec = _wait_sec;
   tv.tv_usec = _wait_usec;
   errno = 0;
   ret = select(_fd+1, &readfd, NULL, NULL, &tv);
   if (ret > 0) {
     if(FD_ISSET(_fd, &readfd))
       return ret;
     else
       return -1;
   } else if (ret == 0) {
     return 0;
   } else {
     if (errno != EINTR) {
       close(_fd);
     }
     return -1;
  }
}

int 
shmmq_dequeue(shm_mq_t *mq, void* buf, 
      uint32_t buf_size, uint32_t *data_len, uint32_t *flow) {
  int ret;

  if (mq == NULL) return -1;

  // try to get data from queue.
  ret = read_mq(mq, buf, buf_size, data_len, flow); 
  if (ret || *data_len) return ret;

  // wait on fifo.
  ret = shmmq_select_fifo(mq->_fd, mq->_wait_sec, mq->_wait_usec);
  if (ret == 0) {
    data_len = 0;
    return ret;
  } else if (ret < 0) {
    return -1;
  }

  // remove all data from fifo
  {
    static const int32_t buf_len = 1<<10;
    char buffer[buf_len];
    ret = read(mq->_fd, buffer, buf_len);
    if (ret < 0 && errno != EAGAIN) {
      return -1;
    }
  }  

  // get data
  ret = read_mq(mq, buf, buf_size, data_len, flow);

  return ret;
}


