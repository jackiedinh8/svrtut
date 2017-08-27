FLAGS += -g -Wall 

INCLUDE += -I. -I/home/jackie/local/include
LIB   += -L/home/jackie/local/lib/ -levent
CC    := gcc
LIBPROFILE := -lprofiler

ECHOSVR_V4   := echosvr_v4
ECHOSVR_V4_OBJ   := echosvr_v4.o utils.o
ECHOCLIENT_V4 := client_v4
ECHOCLIENT_V4_OBJ   := client_v4.o
VIEWSTATS := viewstats
VIEWSTATS_OBJ   := viewstats.o utils.o


all: $(ECHOSVR_V4) $(ECHOCLIENT_V4) $(VIEWSTATS)

$(ECHOSVR_V4): $(ECHOSVR_V4_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V4): $(ECHOCLIENT_V4_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(VIEWSTATS): $(VIEWSTATS_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)


%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

clean: 
	rm -rf *.o *.out *~ core*
	rm -rf $(ECHOSVR_V4_OBJ) $(ECHOCLIENT_V4_OBJ) $(ECHOSVR_V4) $(ECHOCLIENT_V4)
	rm -rf $(VIEWSTATS_OBJ) $(VIEWSTATS)


