FLAGS += -g -Wall 

INCLUDE += -I. -I/home/jackie/local/include
LIB   += -L/home/jackie/local/lib/ -levent
CC    := gcc
C++    := g++
LIBPROFILE := -lprofiler

ECHOSVR_V4   := echosvr_v4
ECHOSVR_V4_OBJ   := echosvr_v4.o utils.o
ECHOCLIENT_V4 := client_v4
ECHOCLIENT_V4_OBJ   := client_v4.o
VIEWSTATS := viewstats
VIEWSTATS_OBJ   := viewstats.o utils.o

IO_CONN   := io_conn
IO_CONN_OBJ   := io_conn.o utils.o mq.o shm.o flow.o
ECHOSVR_V5   := echosvr_v5
ECHOSVR_V5_OBJ   := echosvr_v5.o utils.o mq.o shm.o

all: $(ECHOSVR_V4) $(ECHOCLIENT_V4) $(VIEWSTATS) $(ECHOSVR_V5) $(IO_CONN)

$(ECHOSVR_V4): $(ECHOSVR_V4_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V4): $(ECHOCLIENT_V4_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(VIEWSTATS): $(VIEWSTATS_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(IO_CONN): $(IO_CONN_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOSVR_V5): $(ECHOSVR_V5_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)


%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.cc
	$(C++) $(FLAGS) $(INCLUDE) -c -o $@ $<

clean: 
	rm -rf *.o *.out *~ core*
	rm -rf $(ECHOSVR_V4) $(ECHOSVR_V4_OBJ) $(ECHOCLIENT_V4) $(ECHOCLIENT_V4_OBJ)
	rm -rf $(ECHOSVR_V5) $(ECHOSVR_V5_OBJ) $(IO_CONN) $(IO_CONN_OBJ)
	rm -rf $(VIEWSTATS_OBJ) $(VIEWSTATS)





