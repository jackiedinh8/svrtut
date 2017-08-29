FLAGS += -g -Wall 

INCLUDE += -I. -I/home/jackie/local/include
LIB   += -lm -L/home/jackie/local/lib/ -levent
CC    := gcc
C++    := g++
LIBPROFILE := -lprofiler

ECHOCLIENT_V6 := client_v6
ECHOCLIENT_V6_OBJ   := client_v6.o
VIEWSTATS := viewstats
VIEWSTATS_OBJ   := viewstats.o utils.o

IO_CONN   := io_conn
IO_CONN_OBJ   := io_conn.o utils.o mq.o shm.o flow.o
ECHOSVR_V6   := echosvr_v6
ECHOSVR_V6_OBJ   := echosvr_v6.o utils.o mq.o shm.o connection.o cache.o

all: $(ECHOSVR_V6) $(ECHOCLIENT_V6) $(VIEWSTATS) $(IO_CONN)

$(ECHOSVR_V6): $(ECHOSVR_V6_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V6): $(ECHOCLIENT_V6_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(VIEWSTATS): $(VIEWSTATS_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(IO_CONN): $(IO_CONN_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.cc
	$(C++) $(FLAGS) $(INCLUDE) -c -o $@ $<

clean: 
	rm -rf *.o *.out *~ core*
	rm -rf $(ECHOSVR_V6) $(ECHOSVR_V6_OBJ) $(ECHOCLIENT_V6) $(ECHOCLIENT_V6_OBJ)
	rm -rf $(ECHOSVR_V5) $(ECHOSVR_V5_OBJ) $(IO_CONN) $(IO_CONN_OBJ)
	rm -rf $(VIEWSTATS_OBJ) $(VIEWSTATS)





