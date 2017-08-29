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

#include "flow.h"
#include "mq.h"
#include "stats.h"
#include "utils.h"


typedef struct global_data global_data_t;
struct global_data {
  shm_mq_t *mq_io_2_md;
  shm_mq_t *mq_md_2_io;
  flowset_t *flowset;
};

struct connect_ctx {
  int flowid;
  struct bufferevent *bev;
  global_data_t *g_data;
};

static void
echo_read_cb(struct bufferevent *bev, void *ctx) {
   static char data[BUFFER_SIZE];
   size_t recv_input_len = 0;
   struct evbuffer *input = bufferevent_get_input(bev);
   struct connect_ctx *conn = (struct connect_ctx*)ctx;

   recv_input_len = evbuffer_get_length(input);
   if (recv_input_len <= 0) return;

   /* Copy all the data from the input buffer to the output buffer. */
   evbuffer_remove(input, data, recv_input_len); 

   //printf("Receive message from client, flowid=%u, len=%lu\n",conn->flowid, recv_input_len);
   shmmq_enqueue(conn->g_data->mq_io_2_md,0,data,recv_input_len,conn->flowid);

   return;
}

static void
echo_event_cb(struct bufferevent *bev, short events, void *ctx) {
   if (events & BEV_EVENT_ERROR)
      perror("Error from bufferevent");
   if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
      bufferevent_free(bev);
   }
}

static void
accept_conn_cb(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *ctx) {
   struct connect_ctx *conn = NULL;
   struct global_data *g_data = (struct global_data*)ctx;
   struct event_base *base = evconnlistener_get_base(listener);
   struct bufferevent *bev = bufferevent_socket_new(
           base, fd, BEV_OPT_CLOSE_ON_FREE);
   uint32_t flowid = 0;

   //TODO: build a map from fd to bufferevent

   /* We got a new connection! Set up a bufferevent for it. */
   //printf("Setup new connection, fd=%d\n",fd);
   flowid = flowset_getid(g_data->flowset);
   if (flowid == 0) return;

   conn = (struct connect_ctx*)malloc(sizeof(struct connect_ctx));
   if (!conn) return;
   conn->flowid = flowid;
   conn->bev = bev;
   conn->g_data = g_data;
   flowset_setobj(g_data->flowset, flowid, conn); 
   
   bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, conn);
   bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void
accept_error_cb(struct evconnlistener *listener, void *ctx) {
   struct event_base *base = evconnlistener_get_base(listener);
   int err = EVUTIL_SOCKET_ERROR();
   fprintf(stderr, "Got an error %d (%s) on the listener. "
           "Shutting down.\n", err, evutil_socket_error_to_string(err));

   event_base_loopexit(base, NULL);
}

int
initialize_mq(struct global_data *g_data) {
  int ret = 0;

  g_data->mq_io_2_md = (shm_mq_t *)
         malloc(sizeof(*g_data->mq_io_2_md));
  if (g_data->mq_io_2_md == 0) {
    return -1;
  }
  ret = shmmq_init(g_data->mq_io_2_md, 
    "/tmp/echosvr_io_2_mq.fifo", 0, 0,
    IO_2_MD_KEY, MQ_SIZE);
  if (ret < 0) {
    return -2;
  }  

  g_data->mq_md_2_io = (shm_mq_t *)
         malloc(sizeof(*g_data->mq_md_2_io));
  if (g_data->mq_md_2_io == 0) {
    return -1;
  }
  ret = shmmq_init(g_data->mq_md_2_io, 
    "/tmp/echosvr_mq_2_io.fifo", 0, 0,
    MD_2_IO_KEY, MQ_SIZE);
  if (ret < 0) {
    return -3;
  }  

  return ret;
}

void
handle_msg_from_md(int fd, short int event,void* ctx) {
   static char buf[BUFFER_SIZE];
   struct global_data *g_data = (struct global_data*)ctx;
   struct connect_ctx *conn = NULL;
   uint32_t len = 0;
   uint32_t flowid = 0;
   uint32_t cnt = 0;
   int ret = 0;  
   
   while(1){
     len = 0;
     flowid = 0;
     cnt++;
     if ( cnt >= 100) break;

     ret = shmmq_dequeue(g_data->mq_md_2_io, buf, BUFFER_SIZE, &len, &flowid);
     //printf("md_2_io fd=%d, ret=%d, len=%u, flowid=%u\n",
     //       g_data->mq_md_2_io->_fd, ret, len, flowid);
     if ( (len == 0 && ret == 0) || (ret < 0) )
        return;
    
     buf[len] = 0;
     conn = (struct connect_ctx*)flowset_getobj(g_data->flowset, flowid);
     if (!conn || !conn->bev) continue;
     //printf("send msg to client, fd=%d, ret=%d, len=%u, flowid=%u\n",
     //       g_data->mq_md_2_io->_fd, ret, len, flowid);
     bufferevent_write(conn->bev,buf,len);
   }

   return;
}

int
main(int argc, char **argv) {
  struct event_base *base;
  struct event *ev = NULL;
  struct evconnlistener *listener;
  struct global_data *g_data = NULL;
  struct sockaddr_in sin;
  short port = 4680;

  base = event_base_new();
  if (!base) {
    printf("Couldn't open event base\n");
    return 1;
  }

  //TODO: init message queue
  g_data = (struct global_data*)malloc(sizeof(struct global_data));
  if (!g_data) {
    printf("Couldn't create global data\n");
    return 2;
  }
  initialize_mq(g_data);
  ev = event_new(base,g_data->mq_md_2_io->_fd, 
       EV_TIMEOUT|EV_READ|EV_PERSIST, handle_msg_from_md, g_data);
  event_add(ev, NULL);
 
  g_data->flowset = flowset_init(CONNECTION_NUM);
  if (!g_data->flowset) {
    printf("Couldn't create flow set\n");
    return 2;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(port);

  printf("Server is listening on port %d\n",port);
  listener = evconnlistener_new_bind(base, accept_conn_cb, g_data,
      LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
      (struct sockaddr*)&sin, sizeof(sin));
  if (!listener) {
     perror("Couldn't create listener");
     return 1;
  }
  printf("Server listens on port %d\n",port);
  evconnlistener_set_error_cb(listener, accept_error_cb);

  event_base_dispatch(base);
  return 0;
}


