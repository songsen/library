BIN = test
OBJ = test.o iomodel.o

CC = gcc
CFLAGS = -g
LDFLAGS = 

all:$(BIN)

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OBJ):%o:%c 
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm $(OBJ) $(BIN)