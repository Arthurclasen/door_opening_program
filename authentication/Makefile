CC = gcc
TARGET = authentication_out
SOURCE = common/network/network.c common/network/cJSON/cJSON.c authentication/authentication.c
LIBS = -lmysqlclient

all : $(TARGET)

$(TARGET): $(SOURCE) 
	$(CC) $(SOURCE) -o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)