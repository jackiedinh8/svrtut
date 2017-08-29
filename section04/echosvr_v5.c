#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "mq.h"
#include "stats.h"
#include "utils.h"

static struct timespec process_time;

typedef struct server_ctx server_ctx_t;
struct server_ctx {
  shm_mq_t *mq_io_2_md;
  shm_mq_t *mq_md_2_io;
  struct stats *stats;
};

int
initialize_mq(struct server_ctx *ctx) {
  int ret = 0;

  ctx->mq_io_2_md = (shm_mq_t *)
         malloc(sizeof(*ctx->mq_io_2_md));
  if (ctx->mq_io_2_md == 0) {
    return -1;
  }
  ret = shmmq_init(ctx->mq_io_2_md, 
    "/tmp/echosvr_io_2_mq.fifo", 0, 0,
    IO_2_MD_KEY, MQ_SIZE);
  if (ret < 0) {
    return -2;
  }  

  ctx->mq_md_2_io = (shm_mq_t *)
         malloc(sizeof(*ctx->mq_md_2_io));
  if (ctx->mq_md_2_io == 0) {
    return -1;
  }
  ret = shmmq_init(ctx->mq_md_2_io, 
    "/tmp/echosvr_mq_2_io.fifo", 0, 0,
    MD_2_IO_KEY, MQ_SIZE);
  if (ret < 0) {
    return -3;
  }  

  return ret;
}

void
handle_msg_from_io(int fd, short int event,void* data) {
   static char buf[BUFFER_SIZE];
   struct server_ctx *ctx = (struct server_ctx*)data;
   uint32_t len = 0;
   uint32_t flowid = 0;
   uint32_t cnt = 0;
   int ret = 0;  
   
   while(1){
     len = 0;
     flowid = 0;
     cnt++;
     if ( cnt >= 100) break;

     ret = shmmq_dequeue(ctx->mq_io_2_md, buf, BUFFER_SIZE, &len, &flowid);
     //printf("io_2_md fd=%d, ret=%d, len=%u, flowid=%u\n",
     //       ctx->mq_io_2_md->_fd, ret, len, flowid);
     if ( (len == 0 && ret == 0) || (ret < 0) )
        return;
    
     buf[len] = 0;
     ctx->stats->msg_cnt++;
     nanosleep(&process_time,0);

     //printf("Receive msg from io, msg=%s\n",buf);
     shmmq_enqueue(ctx->mq_md_2_io,0,buf,len,flowid);
   }

   return;
}



int
main(int argc, char **argv) {
  struct event_base *base;
  struct event *ev = NULL;
  struct stats *stats = NULL;
  struct server_ctx *ctx = NULL;
  time_t now;

  base = event_base_new();
  if (!base) {
    printf("Couldn't open event base\n");
    return 1;
  }
  stats = (struct stats*)create_shm(STATS_SHM_KEY, 
      sizeof(struct stats),
      SHM_DEFAULT_OPEN_FLAG | IPC_CREAT | IPC_EXCL);
      
  if (!stats) {
    printf("Couldn't create statistics\n");
    return 1;
  }
  memset(stats,0,sizeof(struct stats));

  ctx = (struct server_ctx*)malloc(sizeof(struct server_ctx));
  if (!ctx) {
    printf("Couldn't create server context\n");
     return 1;
  }
  ctx->stats = stats;
 
  time(&now);
  stats->start_time = now;

  //init message queue
  initialize_mq(ctx);
  ev = event_new(base,ctx->mq_io_2_md->_fd, 
       EV_TIMEOUT|EV_READ|EV_PERSIST, handle_msg_from_io, ctx);
  event_add(ev, NULL);
 
  process_time.tv_sec = 0;
  process_time.tv_nsec = PROC_TIMEOUT;

  event_base_dispatch(base);
  return 0;
}


