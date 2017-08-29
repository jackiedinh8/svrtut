
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char*
create_shm(key_t key, int size, int flags) {
  int shmid;
  char *data;
 
  /* create the shared memory segment */
  shmid = shmget(key, size, flags);
  if (shmid < 0) {
     perror("shmget");
     return 0;
  }

  data = (char*)shmat(shmid, NULL, 0);
  if (data == (char*)-1) {
    return 0;
  }

  return data;
}

