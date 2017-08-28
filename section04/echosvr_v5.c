#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <time.h>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "stats.h"
#include "utils.h"


int
main(int argc, char **argv) {
  struct event_base *base;
  struct evconnlistener *listener;
  struct stats *stats = NULL;
  time_t now;
  struct sockaddr_in sin;
  short port = 4680;

  base = event_base_new();
  if (!base) {
    printf("Couldn't open event base");
    return 1;
  }
  stats = (struct stats*)create_shm(STATS_SHM_KEY, 
      sizeof(struct stats),
      SHM_DEFAULT_OPEN_FLAG | IPC_CREAT | IPC_EXCL);
  if (!stats) {
    printf("Couldn't create statistics");
    return 1;
  }

  memset(stats,0,sizeof(struct stats));
  time(&now);
  stats->start_time = now;

  //init message queue

  event_base_dispatch(base);
  return 0;
}


