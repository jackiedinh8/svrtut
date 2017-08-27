FLAGS += -g -Wall 

INCLUDE += -I.
LIB   += -levent
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

ECHOSVR_V3   := echosvr_v3
ECHOSVR_V3_OBJ   := echosvr_v3.o
ECHOCLIENT_V3 := client_v3
ECHOCLIENT_V3_OBJ   := client_v3.o

all: $(ECHOSVR_V1) $(ECHOCLIENT_V1) $(ECHOSVR_V2) $(ECHOCLIENT_V2) $(ECHOSVR_V3) $(ECHOCLIENT_V3)

$(ECHOSVR_V1): $(ECHOSVR_V1_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V1): $(ECHOCLIENT_V1_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOSVR_V2): $(ECHOSVR_V2_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOCLIENT_V2): $(ECHOCLIENT_V2_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)

$(ECHOSVR_V3): $(ECHOSVR_V3_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB) $(LIBPROFILE)

$(ECHOCLIENT_V3): $(ECHOCLIENT_V3_OBJ)
	$(CC) $(FLAGS) -o $@ $(INCLUDE) $^ $(LIB)


%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<

clean: 
	rm -rf *.o *.out *~ core*
	rm -rf $(ECHOSVR_V1_OBJ) $(ECHOCLIENT_V1_OBJ) $(ECHOSVR_V1) $(ECHOCLIENT_V1)
	rm -rf $(ECHOSVR_V2_OBJ) $(ECHOCLIENT_V2_OBJ) $(ECHOSVR_V2) $(ECHOCLIENT_V2)
	rm -rf $(ECHOSVR_V3_OBJ) $(ECHOCLIENT_V3_OBJ) $(ECHOSVR_V3) $(ECHOCLIENT_V3)


