#include <stdlib.h>
#include <string.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

#define BUFFER_SIZE 4 * 1024
void send_message(struct bufferevent *bev, char *data, size_t data_len) {
   if (!bev || !data || data_len <= 0) return;
   bufferevent_write(bev,data,data_len);
   return;
}

void readcb(struct bufferevent *bev, void *ptr) {
   static char data[BUFFER_SIZE];
   size_t data_len = 0;
   size_t line_len = 0;
   struct evbuffer *input = bufferevent_get_input(bev);
   char *line = NULL;

   // remove data from buffer of socket
   data_len = evbuffer_get_length(input);
   if (data_len > 0) {
      evbuffer_remove(input, data, data_len);
      data[data_len] = '\0'; 
      printf("Receive data from server, len=%lu, data=%s\n", 
              data_len, data);
   }

   while(1) { 
      if (getline(&line, &line_len, stdin) == -1) {
         printf("No line, exit program.\n");
         exit(-1);
      } 
      if (strlen(line) > 1) break;
   }
   
   send_message(bev,line,strlen(line));
   return;
}

void eventcb(struct bufferevent *bev, short events, void *ptr){
   // some events occur
   if (events & BEV_EVENT_ERROR)
      perror("Error from bufferevent");
   if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
      bufferevent_free(bev);
   }
   if(events & BEV_EVENT_CONNECTED) {
      printf("Connected to echo server\n");
      printf("Enter message to send: ");
      readcb(bev,NULL);
   }
   return;
}

int main(int argc, char** argv) {
   short port = 4680;
   struct bufferevent *bev;
   struct sockaddr_in sin;
   struct event_base *base = event_base_new();

   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */
   sin.sin_port = htons(port); /* Port 4680 */

   bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
   bufferevent_setcb(bev, readcb, NULL, eventcb, NULL);
   bufferevent_enable(bev, EV_READ|EV_WRITE);

   if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
      /* failed to start connection */
      bufferevent_free(bev);
      return -1;
   }

   event_base_dispatch(base);
   return 0;
}

