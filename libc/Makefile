ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
LIBC_HEADERS:=${INCLUDE}

SUPPORTED=\
string

REQUIRED=$(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.c))

build: 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) \
	-I$(LIBC_HEADERS) \
	$(REQUIRED)  
	mv *.o obj/

clean:
	rm obj/*.o

