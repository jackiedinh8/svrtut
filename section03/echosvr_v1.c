#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define BUFFER_SIZE 1024 * 4
static void
echo_read_cb(struct bufferevent *bev, void *ctx) {
   static char data[BUFFER_SIZE];
   size_t recv_input_len = 0;
   struct evbuffer *input = bufferevent_get_input(bev);

   recv_input_len = evbuffer_get_length(input);
   if (recv_input_len <= 0) return;

   printf("Receive message from client, len=%lu\n",recv_input_len);

   /* Copy all the data from the input buffer to the output buffer. */
   evbuffer_remove(input, data, recv_input_len); 
   bufferevent_write(bev,data,recv_input_len);
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
   struct event_base *base = evconnlistener_get_base(listener);
   struct bufferevent *bev = bufferevent_socket_new(
           base, fd, BEV_OPT_CLOSE_ON_FREE);

   /* We got a new connection! Set up a bufferevent for it. */
   printf("Setup new connection, fd=%d\n",fd);
   bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
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
main(int argc, char **argv) {
  struct event_base *base;
  struct evconnlistener *listener;
  struct sockaddr_in sin;
  short port = 4680;

  base = event_base_new();
  if (!base) {
    printf("Couldn't open event base");
    return 1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(port);

  printf("Server is listening on port %d\n",port);
  listener = evconnlistener_new_bind(base, accept_conn_cb, NULL,
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


