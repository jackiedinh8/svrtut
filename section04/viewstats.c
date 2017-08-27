
#include <sys/shm.h>
#include <time.h>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "stats.h"
#include "utils.h"


void timeout_cb(evutil_socket_t fd, short what, void *ctx) {
   struct stats *stats = (struct stats*)ctx;
   time_t now;

   time(&now);
   stats->last_time = now;

   printf("statictis: \n");
   printf("     msg_cnt: %u\n", stats->msg_cnt);
   printf("    conn_cnt: %u\n", stats->conn_cnt);
   printf("  start_time: %lu\n", stats->start_time);
   printf("   last_time: %lu\n", stats->last_time);
   return;
}

int
main(int argc, char **argv) {
  struct event_base *base;
  struct event *ev = NULL;
  struct timeval timeout_interval = {TIMEOUT_SEC,0};
  struct stats *stats = NULL;

  base = event_base_new();
  if (!base) {
    printf("Couldn't open event base");
    return 1;
  }

  stats = (struct stats*)create_shm(STATS_SHM_KEY, 
      sizeof(struct stats), SHM_DEFAULT_OPEN_FLAG);
  if (!stats) {
    printf("Couldn't open statistics");
    return 1;
  }

  ev = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, timeout_cb, stats); 
  if (!ev) {
     perror("Couldn't create timeout event");
     return 1;
  }
  event_add(ev, &timeout_interval);
  event_base_dispatch(base);

  return 0;
}

