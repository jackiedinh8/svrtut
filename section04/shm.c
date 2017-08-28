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
 * @(#)shm.c
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "shm.h"

static const int SHM_DEFAULT_OPEN_FLAG = 0666;

shm_data_t* 
shmdata_open(key_t key, size_t size) {
  shm_data_t* shm = NULL;
  int id; 

  id = shmget(key, size, SHM_DEFAULT_OPEN_FLAG);
  if (id < 0) {
    return 0;
  }
  
  shm = (shm_data_t *) malloc(sizeof(shm_data_t));
  if ( shm == NULL ) {
    return 0;
  }
  shm->key = key;
  shm->size = size;
  shm->id = id;

  shm->addr = (char*)shmdata_at(id);

  if ( shm->addr == NULL ) {
    free(shm);
    return 0;
  }

  return shm;
}

shm_data_t* 
shmdata_create(key_t key, size_t size) {
  shm_data_t* shm = NULL;
  int id;

  id = shmget(key, size, SHM_DEFAULT_OPEN_FLAG | IPC_CREAT | IPC_EXCL);
  if (id < 0) {
    return 0;
  }

  shm = (shm_data_t *) malloc(sizeof(shm_data_t));
  if ( shm == NULL ) {
    return 0;
  }
  shm->key = key;
  shm->size = size;
  shm->id = id;

  shm->addr = (char*)shmdata_at(id);

  if ( shm->addr == NULL ) {
    free(shm);
    return 0;
  }

  return shm;
}

char* 
shmdata_at(int _id) {
  char* p = (char*) shmat(_id, NULL, 0);
  if (p == (char*)-1) {
    return 0;
  }

  return p;
}

void 
shmdata_dt(char* _mem) {
  if (_mem == NULL)
    return;
  
  int ret = shmdt(_mem);
  if (ret < 0) {
    return;
  }
  return;
}

int
shmdata_alloc(shm_data_t *shm) {
  int  id;
   
  if ( shm->key == 0 )
    id = shmget(IPC_PRIVATE, shm->size, (SHM_R|SHM_W|IPC_CREAT));
  else
    id = shmget(shm->key, shm->size, (SHM_R|SHM_W|IPC_CREAT));

  if (id == -1) {
    return -1;
  }

  shm->addr = (char*)shmat(id, NULL, 0);

  if (shm->addr == (void *) -1) {
    return -1;
  }

  return (shm->addr == (void *) -1) ? -3 : 0;
}

void shmdata_free(shm_data_t *shm) {
  if (shmdt(shm->addr) == -1) {
    // TODO print log
  }
  return;
}

