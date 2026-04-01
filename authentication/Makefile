CC = gcc
TARGET = authentication_out
SOURCE = common/network/network.c common/cJSON/cJSON.c common/security/tls.c authentication/authentication.c
LIBS = -lmysqlclient -lssl -lcrypto

all : $(TARGET)

$(TARGET): $(SOURCE) 
	$(CC) $(SOURCE) -o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)