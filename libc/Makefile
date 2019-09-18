ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
LIBC_HEADERS:=${INCLUDE}

LIBK_FLAGS=-D__is_libk

SUPPORTED=\
string

REQUIRED=$(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.c))
LIBC_OBJ=$(REQUIRED:.c=.libc.o)
LIBK_OBJ=$(REQUIRED:.c=.libk.o)

build: $(SUPPORTED)

$(SUPPORTED): $(LIBC_OBJ) $(LIBK_OBJ)
	mkdir lib
	$(AR) rcs $@.libc.a $(LIBC_OBJ)
	$(AR) rcs $@.libk.a $(LIBK_OBJ)
	mv *.libc.a lib/
	mv *.libk.a lib/

%.libc.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) \
	-I$(LIBC_HEADERS) \
	$(REQUIRED) -o $@

%.libk.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) \
	-I$(LIBC_HEADERS) \
	$(REQUIRED) -o $@

clean:
	rm $(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.o)) 
	rm -rf lib/
