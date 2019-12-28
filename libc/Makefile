
ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}
ROOT:=${ROOT}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
LIBC_HEADERS:=${INCLUDE}
KINCLUDE:=${KINCLUDE}
LIBK_FLAGS=-D__is_libk -I$(KINCLUDE)


SUPPORTED=string stdio

LIB_DEST=$(ROOT)/bin/libc

REQUIRED=$(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.cpp))

LIBC_OBJ=$(REQUIRED:.cpp=.libc.o)
LIBC_OUT=$(foreach file, $(LIBC_OBJ), $(LIB_DEST)/$(shell basename $(file)))

LIBK_OBJ=$(REQUIRED:.cpp=.libk.o)
LIBK_OUT=$(foreach file, $(LIBK_OBJ), $(LIB_DEST)/$(shell basename $(file)))

TO_BUILD=$(LIBK_OBJ) $(LIBC_OBJ)

.SUFFIXES: .libc.o libk.o .cpp

build: $(TO_BUILD) 
	$(AR) rcs $(ROOT)/bin/libc/$@.libc.a $(LIBC_OUT)
	$(AR) rcs $(ROOT)/bin/libc/$@.libk.a $(LIBK_OUT)

$(LIBC_OBJ): $(REQUIRED_LIBC)

$(LIBK_OBJ): $(REQUIRED_LIBK)

%.libc.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $(ROOT)/bin/libc/$$(basename $@)

%.libk.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $(ROOT)/bin/libc/$$(basename $@)
