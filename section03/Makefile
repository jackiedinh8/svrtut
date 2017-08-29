FLAGS += -g -Wall 

INCLUDE += -I. -I/home/jackie/local/include
LIB   += -L/home/jackie/local/lib -levent
CC    := gcc
LIBPROFILE := -lprofiler

ECHOSVR_V1   := echosvr_v1
ECHOSVR_V1_OBJ   := echosvr_v1.o
ECHOCLIENT_V1 := client_v1
ECHOCLIENT_V1_OBJ   := client_v1.o

ECHOSVR_V2   := echosvr_v2
ECHOSVR_V2_OBJ   := echosvr_v2.o
ECHOCLIENT_V2 := client_v2
ECHOCLIENT_V2_OBJ   := client_v2.o

all: $(ECHOSVR_V1) $(ECHOCLIENT_V1) $(ECHOSVR_V2) $(ECHOCLIENT_V2)

$(ECHOSVR_V1): $(ECHOSVR_V1_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V1): $(ECHOCLIENT_V1_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOSVR_V2): $(ECHOSVR_V2_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V2): $(ECHOCLIENT_V2_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

clean: 
	rm -rf *.o *.out *~ core*
	rm -rf $(ECHOSVR_V1_OBJ) $(ECHOCLIENT_V1_OBJ) $(ECHOSVR_V1) $(ECHOCLIENT_V1)
	rm -rf $(ECHOSVR_V2_OBJ) $(ECHOCLIENT_V2_OBJ) $(ECHOSVR_V2) $(ECHOCLIENT_V2)


