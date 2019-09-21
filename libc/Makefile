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


.SUFFIXES: .libc.o libk.o .c

build: $(SUPPORTED)

$(SUPPORTED): $(LIBK_OBJ) $(LIBC_OBJ) 
	mkdir lib
	$(AR) rcs $@.libc.a $(LIBC_OBJ)
	$(AR) rcs $@.libk.a $(LIBK_OBJ)
	mv *.libc.a lib/
	mv *.libk.a lib/

$(LIBC_OBJ): $(REQUIRED_LIBC)

$(LIBK_OBJ): $(REQUIRED_LIBK)

%.libc.o: %.c 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $@

%.libk.o: %.c 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $@

clean:
	rm $(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.o)) 
	rm -rf lib/
